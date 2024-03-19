#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include "atomic_queue.h"
#include "spin_lock.h"
#include "vrt_comm.h"
#include "vrt_node.h"

namespace vrt {

template <class ValueType>
class Vrt;

template <class ValueType>
class VrtImpl {
 public:
  VrtImpl() = delete;
  VrtImpl(Vrt<ValueType> *vrt) : root_(nullptr), vrt_(vrt), free_queue_(), root_parent_() {}
  VrtImpl(VrtNode *root, Vrt<ValueType> *vrt) : root_(root), vrt_(vrt), free_queue_(), root_parent_() {}
  VrtImpl(const VrtImpl &) = delete;
  void operator=(const VrtImpl &) = delete;
  VrtImpl(VrtImpl &&) = default;
  ~VrtImpl() = default;

  // 查找
  bool Find(std::string_view key, ValueType *value);
  // 插入
  template <class... Args>
  bool Insert(std::string_view key, ValueType *old_value, Args &&...args);
  // 更新
  template <class... Args>
  bool Update(std::string_view key, Args &&...args);
  // 插入或更新
  template <class... Args>
  bool Upsert(std::string_view key, Args &&...args);
  // 删除
  bool Delete(std::string_view key);
  void DestroyTree();

 private:
  bool FindImpl(VrtNode *node, std::string_view key, ValueType *value);
  template <class... Args>
  bool InsertImpl(VrtNode *&node, VrtNode *parent, std::string_view key, ValueType *old_value, Args &&...args);
  template <class... Args>
  bool UpdateImpl(VrtNode *&node, VrtNode *parent, std::string_view key, Args &&...args);
  template <class... Args>
  bool UpsertImpl(VrtNode *&node, VrtNode *parent, std::string_view key, Args &&...args);
  // 删除
  bool DeleteImpl(VrtNode *&node, VrtNode *parent, std::string_view key);

  void UpdateVrt();
  void FreeNode(VrtNode *node);

  VrtNode *root_;
  VrtNode root_parent_;
  Vrt<ValueType> *vrt_;
  // std::queue<VrtNode *> free_list_;
  // SpinLock free_list_lock_;
  AtomicQueue<VrtNode *> free_queue_;
};

template <class ValueType>
class Vrt {
 public:
  Vrt() : impl_(std::make_shared<VrtImpl<ValueType>>(this)), impl_lock_(){};
  Vrt(const Vrt &) = delete;
  Vrt(Vrt &&) = default;
  ~Vrt();

  bool Find(std::string_view key, ValueType *value);
  template <class... Args>
  bool Insert(std::string_view key, ValueType *old_value, Args &&...args);
  template <class... Args>
  bool Update(std::string_view key, Args &&...args);
  template <class... Args>
  bool Upsert(std::string_view key, Args &&...args);
  bool Delete(std::string_view key);

  friend class VrtImpl<ValueType>;

 private:
  std::shared_ptr<VrtImpl<ValueType>> impl_;
  SpinLock impl_lock_;
};

template <class ValueType>
Vrt<ValueType>::~Vrt() {
  impl_->DestroyTree();
}

template <class ValueType>
void VrtImpl<ValueType>::DestroyTree() {
  if (root_ == nullptr) {
    return;
  }
  VrtNodeHelper::DestroyTree<ValueType>(root_);
}

template <class ValueType>
void VrtImpl<ValueType>::UpdateVrt() {
  vrt_->impl_lock_.lock();
  vrt_->impl_ = std::make_shared<VrtImpl<ValueType>>(root_, vrt_);
  vrt_->impl_lock_.unlock();
}

template <class ValueType>
void VrtImpl<ValueType>::FreeNode(VrtNode *node) {
  free_queue_.Push(node);
  // free_list_lock_.lock();
  // free_list_.push(node);
  // free_list_lock_.unlock();
}

template <class ValueType>
bool Vrt<ValueType>::Find(std::string_view key, ValueType *value) {
  if (unlikely(key.empty())) {
    return false;
  }
  auto impl_ptr = impl_;
  return impl_ptr->Find(key, value);
}

template <class ValueType>
bool VrtImpl<ValueType>::Find(std::string_view key, ValueType *value) {
  if (nullptr == root_) {
    return false;
  }
  return FindImpl(root_, key, value);
}

template <class ValueType>
bool VrtImpl<ValueType>::FindImpl(VrtNode *node, std::string_view key, ValueType *value) {
  auto same_prefix_length = VrtNodeHelper::CheckSamePrefixLength(node, key);
  if (same_prefix_length < node->key_length) {
    return false;
  }
  if (key.length() == same_prefix_length) {
    if (node->has_value) {
      *value = VrtNodeHelper::GetValue<ValueType>(node);
      return true;
    }
    return false;
  }
  if (VrtNode *&next_node = VrtNodeHelper::FindChild(node, key[same_prefix_length]); next_node != nullptr) {
    key.remove_prefix(same_prefix_length + 1);
    return FindImpl(next_node, key, value);
  }
  return false;
}

template <class ValueType>
template <class... Args>
bool Vrt<ValueType>::Insert(std::string_view key, ValueType *old_value, Args &&...args) {
  if (unlikely(key.empty() || key.size() >= kMaxKeySize)) {
    return false;
  }
  auto impl_ptr = impl_;
  return impl_ptr->Insert(key, old_value, std::forward<Args>(args)...);
}

template <class ValueType>
template <class... Args>
bool VrtImpl<ValueType>::Insert(std::string_view key, ValueType *old_value, Args &&...args) {
  root_parent_.Lock();
  if (nullptr == root_) {
    root_ = VrtNodeHelper::CreateVrtNode<ValueType>(key, std::forward<Args>(args)...);
    root_parent_.Unlock();
    return true;
  }
  return InsertImpl(root_, &root_parent_, key, old_value, std::forward<Args>(args)...);
}

template <class ValueType>
template <class... Args>
bool VrtImpl<ValueType>::InsertImpl(VrtNode *&node, VrtNode *parent, std::string_view key, ValueType *old_value,
                                    Args &&...args) {
  node->Lock();
  auto same_prefix_length = VrtNodeHelper::CheckSamePrefixLength(node, key);
  if (same_prefix_length < key.length() && same_prefix_length < node->key_length) {
    // 同前缀部分作为父节点，旧节点去掉相同部分后作为child1，key剩余部分新建结点作为child2。
    std::string_view new_node_key = key.substr(0, same_prefix_length);
    auto *new_node = VrtNodeHelper::CreateVrtNodeWithoutValue(new_node_key);
    auto *child = VrtNodeHelper::CreateVrtNodeByRemovePrefix<ValueType>(node, same_prefix_length + 1);
    new_node->AddChild(node->data[same_prefix_length], child);
    char next_char = key[same_prefix_length];
    key.remove_prefix(same_prefix_length + 1);
    child = VrtNodeHelper::CreateVrtNode<ValueType>(key, std::forward<Args>(args)...);
    new_node->AddChild(next_char, child);
    auto *old_node = node;
    node = new_node;
    parent->Unlock();
    UpdateVrt();
    FreeNode(old_node);
    return true;
  }
  if (same_prefix_length == key.length() && same_prefix_length < node->key_length) {
    // 同前缀部分作为父节点并且插入值，旧节点去掉相同部分后作为child1
    std::string_view new_node_key = key.substr(0, same_prefix_length);
    auto *new_node = VrtNodeHelper::CreateVrtNode<ValueType>(new_node_key, std::forward<Args>(args)...);
    auto *child = VrtNodeHelper::CreateVrtNodeByRemovePrefix<ValueType>(node, same_prefix_length + 1);
    new_node->AddChild(node->data[same_prefix_length], child);
    auto *old_node = node;
    node = new_node;
    parent->Unlock();
    UpdateVrt();
    FreeNode(old_node);
    return true;
  }
  if (same_prefix_length == key.length() && same_prefix_length == node->key_length) {
    // 值挂在当前节点上
    if (node->has_value) {
      *old_value = VrtNodeHelper::GetValue<ValueType>(node);
      node->Unlock();
      parent->Unlock();
      return false;
    }
    auto *old_node = node;
    auto *new_node = VrtNodeHelper::CreateVrtNodeByAddValue<ValueType>(node, std::forward<Args>(args)...);
    node = new_node;
    parent->Unlock();
    UpdateVrt();
    FreeNode(old_node);
    return true;
  }
  parent->Unlock();
  // 继续搜索，没有找到对应子节点，增新增叶子挂在当前节点上
  if (VrtNode *&next_node = VrtNodeHelper::FindChild(node, key[same_prefix_length]); next_node != nullptr) {
    key.remove_prefix(same_prefix_length + 1);
    return InsertImpl(next_node, node, key, old_value, std::forward<Args>(args)...);
  }
  char next_char = key[same_prefix_length];
  key.remove_prefix(same_prefix_length + 1);
  auto *new_node = VrtNodeHelper::CreateVrtNode<ValueType>(key, std::forward<Args>(args)...);
  node->AddChild(next_char, new_node);
  node->Unlock();
  return true;
}

// 更新
template <class ValueType>
template <class... Args>
bool Vrt<ValueType>::Update(std::string_view key, Args &&...args) {
  if (unlikely(key.empty() || key.size() >= kMaxKeySize)) {
    return false;
  }
  auto impl_ptr = impl_;
  return impl_ptr->Update(key, std::forward<Args>(args)...);
}

template <class ValueType>
template <class... Args>
bool VrtImpl<ValueType>::Update(std::string_view key, Args &&...args) {
  root_parent_.Lock();
  if (nullptr == root_) {
    root_parent_.Unlock();
    return false;
  }
  return UpdateImpl(root_, &root_parent_, key, std::forward<Args>(args)...);
}

template <class ValueType>
template <class... Args>
bool VrtImpl<ValueType>::UpdateImpl(VrtNode *&node, VrtNode *parent, std::string_view key, Args &&...args) {
  node->Lock();
  auto same_prefix_length = VrtNodeHelper::CheckSamePrefixLength(node, key);
  if (same_prefix_length < node->key_length) {
    node->Unlock();
    parent->Unlock();
    return false;
  }
  if (key.length() == same_prefix_length) {
    if (node->has_value) {
      auto *new_node = VrtNodeHelper::CreateVrtNodeByAddValue<ValueType>(node, std::forward<Args>(args)...);
      auto *old_node = node;
      node = new_node;
      parent->Unlock();
      UpdateVrt();
      FreeNode(old_node);
      return true;
    }
    node->Unlock();
    parent->Unlock();
    return false;
  }
  parent->Unlock();
  if (VrtNode *&next_node = VrtNodeHelper::FindChild(node, key[same_prefix_length]); next_node != nullptr) {
    key.remove_prefix(same_prefix_length + 1);
    return UpdateImpl(next_node, node, key, std::forward<Args>(args)...);
  }
  node->Unlock();
  return false;
}

// 插入或更新
template <class ValueType>
template <class... Args>
bool Vrt<ValueType>::Upsert(std::string_view key, Args &&...args) {
  if (unlikely(key.empty() || key.size() >= kMaxKeySize)) {
    return false;
  }
  auto impl_ptr = impl_;
  return impl_ptr->Upsert(key, std::forward<Args>(args)...);
}

template <class ValueType>
template <class... Args>
bool VrtImpl<ValueType>::Upsert(std::string_view key, Args &&...args) {
  root_parent_.Lock();
  if (nullptr == root_) {
    root_ = VrtNodeHelper::CreateVrtNode<ValueType>(key, std::forward<Args>(args)...);
    root_parent_.Unlock();
    return true;
  }
  return UpsertImpl(root_, &root_parent_, key, std::forward<Args>(args)...);
}

template <class ValueType>
template <class... Args>
bool VrtImpl<ValueType>::UpsertImpl(VrtNode *&node, VrtNode *parent, std::string_view key, Args &&...args) {
  node->Lock();
  auto same_prefix_length = VrtNodeHelper::CheckSamePrefixLength(node, key);
  if (same_prefix_length < key.length() && same_prefix_length < node->key_length) {
    // 同前缀部分作为父节点，旧节点去掉相同部分后作为child1，key剩余部分新建结点作为child2。
    std::string_view new_node_key = key.substr(0, same_prefix_length);
    auto *new_node = VrtNodeHelper::CreateVrtNodeWithoutValue(new_node_key);
    auto *child = VrtNodeHelper::CreateVrtNodeByRemovePrefix<ValueType>(node, same_prefix_length + 1);
    new_node->AddChild(node->data[same_prefix_length], child);

    char next_char = key[same_prefix_length];
    key.remove_prefix(same_prefix_length + 1);
    child = VrtNodeHelper::CreateVrtNode<ValueType>(key, std::forward<Args>(args)...);
    new_node->AddChild(next_char, child);

    auto *old_node = node;
    node = new_node;
    parent->Unlock();
    ;
    UpdateVrt();
    FreeNode(old_node);
    return true;
  }
  if (same_prefix_length == key.length() && same_prefix_length < node->key_length) {
    // 同前缀部分作为父节点并且插入值，旧节点去掉相同部分后作为child1
    std::string_view new_node_key = key.substr(0, same_prefix_length);
    auto *new_node = VrtNodeHelper::CreateVrtNode<ValueType>(new_node_key, std::forward<Args>(args)...);
    auto *child = VrtNodeHelper::CreateVrtNodeByRemovePrefix<ValueType>(node, same_prefix_length + 1);
    new_node->AddChild(node->data[same_prefix_length], child);
    auto *old_node = node;
    node = new_node;
    parent->Unlock();
    UpdateVrt();
    FreeNode(old_node);
    return true;
  }
  if (same_prefix_length == key.length() && same_prefix_length == node->key_length) {
    // 值挂在当前节点上
    auto *old_node = node;
    auto *new_node = VrtNodeHelper::CreateVrtNodeByAddValue<ValueType>(node, std::forward<Args>(args)...);
    node = new_node;
    parent->Unlock();
    UpdateVrt();
    FreeNode(old_node);
    return true;
  }
  parent->Unlock();
  // 继续搜索，没有找到对应子节点，增新增叶子挂在当前节点上
  if (VrtNode *&next_node = VrtNodeHelper::FindChild(node, key[same_prefix_length]); next_node != nullptr) {
    key.remove_prefix(same_prefix_length + 1);
    return UpsertImpl(next_node, node, key, std::forward<Args>(args)...);
  }
  char next_char = key[same_prefix_length];
  key.remove_prefix(same_prefix_length + 1);
  auto *new_node = VrtNodeHelper::CreateVrtNode<ValueType>(key, std::forward<Args>(args)...);
  node->AddChild(next_char, new_node);
  node->Unlock();
  return true;
}

// 删除
template <class ValueType>
bool Vrt<ValueType>::Delete(std::string_view key) {
  if (unlikely(key.empty() || key.size() >= kMaxKeySize)) {
    return false;
  }
  auto impl_ptr = impl_;
  return impl_ptr->Delete(key);
}

template <class ValueType>
bool VrtImpl<ValueType>::Delete(std::string_view key) {
  root_parent_.Lock();
  if (nullptr == root_) {
    root_parent_.Unlock();
    return false;
  }
  return DeleteImpl(root_, &root_parent_, key);
}

template <class ValueType>
bool VrtImpl<ValueType>::DeleteImpl(VrtNode *&node, VrtNode *parent, std::string_view key) {
  node->Lock();
  auto same_prefix_length = VrtNodeHelper::CheckSamePrefixLength(node, key);
  if (same_prefix_length < node->key_length) {
    node->Unlock();
    parent->Unlock();
    return false;
  }
  if (key.length() == same_prefix_length) {
    if (node->has_value) {
      if (0 == node->child_cnt) {
        // 删除节点
        auto *old_node = node;
        node = nullptr;
        parent->child_cnt--;
        parent->Unlock();
        UpdateVrt();
        FreeNode(old_node);
      } else if (1 == node->child_cnt) {
        auto *old_node = node;
        for (auto *child : node->childs) {
          if (child != nullptr) {
            node = child;
            break;
          }
        }
        parent->Unlock();
        UpdateVrt();
        FreeNode(old_node);
      } else {
        auto *new_node = VrtNodeHelper::CreateVrtNodeByDeleteValue<ValueType>(node);
        auto *old_node = node;
        node = new_node;
        parent->Unlock();
        UpdateVrt();
        FreeNode(old_node);
      }
      return true;
    }
    node->Unlock();
    parent->Unlock();
    return false;
  }
  parent->Unlock();
  ;
  if (VrtNode *&next_node = VrtNodeHelper::FindChild(node, key[same_prefix_length]); next_node != nullptr) {
    key.remove_prefix(same_prefix_length + 1);
    return DeleteImpl(next_node, node, key);
  }
  node->Unlock();
  return false;
}

}  // namespace vrt
