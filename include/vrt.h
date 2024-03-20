#pragma once

#include <memory>
#include "atomic_queue.h"
#include "spin_lock.h"
#include "vrt_comm.h"
#include "vrt_node.h"

namespace vrt {

template <class ValueType, bool kWriteLock = true>
class Vrt;

template <class ValueType, bool kWriteLock = true>
class VrtImpl {
 public:
  VrtImpl() = delete;
  VrtImpl(Vrt<ValueType, kWriteLock> *vrt) : root_(nullptr), vrt_(vrt), free_queue_(), root_parent_() {}
  VrtImpl(VrtNode<kWriteLock> *root, Vrt<ValueType, kWriteLock> *vrt)
      : root_(root), vrt_(vrt), free_queue_(), root_parent_() {}
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
  bool FindImpl(VrtNode<kWriteLock> *node, std::string_view key, ValueType *value);
  template <class... Args>
  bool InsertImpl(VrtNode<kWriteLock> *&node, VrtNode<kWriteLock> *parent, std::string_view key, ValueType *old_value,
                  Args &&...args);
  template <class... Args>
  bool UpdateImpl(VrtNode<kWriteLock> *&node, VrtNode<kWriteLock> *parent, std::string_view key, Args &&...args);
  template <class... Args>
  bool UpsertImpl(VrtNode<kWriteLock> *&node, VrtNode<kWriteLock> *parent, std::string_view key, Args &&...args);
  // 删除
  bool DeleteImpl(VrtNode<kWriteLock> *&node, VrtNode<kWriteLock> *parent, std::string_view key);

  void UpdateVrt();
  void FreeNode(VrtNode<kWriteLock> *node);

  VrtNode<kWriteLock> *root_;
  VrtNode<kWriteLock> root_parent_;
  Vrt<ValueType, kWriteLock> *vrt_;
  AtomicQueue<VrtNode<kWriteLock> *> free_queue_;
};

template <class ValueType, bool kWriteLock>
class Vrt {
 public:
  Vrt() : impl_(std::make_shared<VrtImpl<ValueType, kWriteLock>>(this)), impl_lock_(){};
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

  friend class VrtImpl<ValueType, kWriteLock>;

 private:
  std::shared_ptr<VrtImpl<ValueType, kWriteLock>> impl_;
  SpinLock impl_lock_;
};

template <class ValueType, bool kWriteLock>
Vrt<ValueType, kWriteLock>::~Vrt() {
  impl_->DestroyTree();
}

template <class ValueType, bool kWriteLock>
void VrtImpl<ValueType, kWriteLock>::DestroyTree() {
  if (root_ == nullptr) {
    return;
  }
  VrtNodeHelper<kWriteLock>::template DestroyTree<ValueType>(root_);
}

template <class ValueType, bool kWriteLock>
void VrtImpl<ValueType, kWriteLock>::UpdateVrt() {
  if constexpr (kWriteLock) {
    vrt_->impl_lock_.lock();
  }
  vrt_->impl_ = std::make_shared<VrtImpl<ValueType, kWriteLock>>(root_, vrt_);
  if constexpr (kWriteLock) {
    vrt_->impl_lock_.unlock();
  }
}

template <class ValueType, bool kWriteLock>
void VrtImpl<ValueType, kWriteLock>::FreeNode(VrtNode<kWriteLock> *node) {
  free_queue_.Push(node);
}

template <class ValueType, bool kWriteLock>
bool Vrt<ValueType, kWriteLock>::Find(std::string_view key, ValueType *value) {
  if (unlikely(key.empty())) {
    return false;
  }
  auto impl_ptr = impl_;
  return impl_ptr->Find(key, value);
}

template <class ValueType, bool kWriteLock>
bool VrtImpl<ValueType, kWriteLock>::Find(std::string_view key, ValueType *value) {
  if (nullptr == root_) {
    return false;
  }
  return FindImpl(root_, key, value);
}

template <class ValueType, bool kWriteLock>
bool VrtImpl<ValueType, kWriteLock>::FindImpl(VrtNode<kWriteLock> *node, std::string_view key, ValueType *value) {
  auto same_prefix_length = VrtNodeHelper<kWriteLock>::CheckSamePrefixLength(node, key);
  if (same_prefix_length < node->key_length) {
    return false;
  }
  if (key.length() == same_prefix_length) {
    if (node->has_value) {
      *value = VrtNodeHelper<kWriteLock>::template GetValue<ValueType>(node);
      return true;
    }
    return false;
  }
  if (VrtNode<kWriteLock> *&next_node = VrtNodeHelper<kWriteLock>::FindChild(node, key[same_prefix_length]);
      next_node != nullptr) {
    key.remove_prefix(same_prefix_length + 1);
    return FindImpl(next_node, key, value);
  }
  return false;
}

template <class ValueType, bool kWriteLock>
template <class... Args>
bool Vrt<ValueType, kWriteLock>::Insert(std::string_view key, ValueType *old_value, Args &&...args) {
  if (unlikely(key.empty() || key.size() >= kMaxKeySize)) {
    return false;
  }
  auto impl_ptr = impl_;
  return impl_ptr->Insert(key, old_value, std::forward<Args>(args)...);
}

template <class ValueType, bool kWriteLock>
template <class... Args>
bool VrtImpl<ValueType, kWriteLock>::Insert(std::string_view key, ValueType *old_value, Args &&...args) {
  root_parent_.Lock();
  if (nullptr == root_) {
    root_ = VrtNodeHelper<kWriteLock>::template CreateVrtNode<ValueType>(key, std::forward<Args>(args)...);
    root_parent_.Unlock();
    return true;
  }
  return InsertImpl(root_, &root_parent_, key, old_value, std::forward<Args>(args)...);
}

template <class ValueType, bool kWriteLock>
template <class... Args>
bool VrtImpl<ValueType, kWriteLock>::InsertImpl(VrtNode<kWriteLock> *&node, VrtNode<kWriteLock> *parent,
                                                std::string_view key, ValueType *old_value, Args &&...args) {
  node->Lock();
  auto same_prefix_length = VrtNodeHelper<kWriteLock>::CheckSamePrefixLength(node, key);
  if (same_prefix_length < key.length() && same_prefix_length < node->key_length) {
    // 同前缀部分作为父节点，旧节点去掉相同部分后作为child1，key剩余部分新建结点作为child2。
    std::string_view new_node_key = key.substr(0, same_prefix_length);
    auto *new_node = VrtNodeHelper<kWriteLock>::CreateVrtNodeWithoutValue(new_node_key);
    auto *child =
        VrtNodeHelper<kWriteLock>::template CreateVrtNodeByRemovePrefix<ValueType>(node, same_prefix_length + 1);
    new_node->AddChild(node->data[same_prefix_length], child);
    char next_char = key[same_prefix_length];
    key.remove_prefix(same_prefix_length + 1);
    child = VrtNodeHelper<kWriteLock>::template CreateVrtNode<ValueType>(key, std::forward<Args>(args)...);
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
    auto *new_node =
        VrtNodeHelper<kWriteLock>::template CreateVrtNode<ValueType>(new_node_key, std::forward<Args>(args)...);
    auto *child =
        VrtNodeHelper<kWriteLock>::template CreateVrtNodeByRemovePrefix<ValueType>(node, same_prefix_length + 1);
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
      *old_value = VrtNodeHelper<kWriteLock>::template GetValue<ValueType>(node);
      node->Unlock();
      parent->Unlock();
      return false;
    }
    auto *old_node = node;
    auto *new_node =
        VrtNodeHelper<kWriteLock>::template CreateVrtNodeByAddValue<ValueType>(node, std::forward<Args>(args)...);
    node = new_node;
    parent->Unlock();
    UpdateVrt();
    FreeNode(old_node);
    return true;
  }
  parent->Unlock();
  // 继续搜索，没有找到对应子节点，增新增叶子挂在当前节点上
  if (VrtNode<kWriteLock> *&next_node = VrtNodeHelper<kWriteLock>::FindChild(node, key[same_prefix_length]);
      next_node != nullptr) {
    key.remove_prefix(same_prefix_length + 1);
    return InsertImpl(next_node, node, key, old_value, std::forward<Args>(args)...);
  }
  char next_char = key[same_prefix_length];
  key.remove_prefix(same_prefix_length + 1);
  auto *new_node = VrtNodeHelper<kWriteLock>::template CreateVrtNode<ValueType>(key, std::forward<Args>(args)...);
  node->AddChild(next_char, new_node);
  node->Unlock();
  return true;
}

// 更新
template <class ValueType, bool kWriteLock>
template <class... Args>
bool Vrt<ValueType, kWriteLock>::Update(std::string_view key, Args &&...args) {
  if (unlikely(key.empty() || key.size() >= kMaxKeySize)) {
    return false;
  }
  auto impl_ptr = impl_;
  return impl_ptr->Update(key, std::forward<Args>(args)...);
}

template <class ValueType, bool kWriteLock>
template <class... Args>
bool VrtImpl<ValueType, kWriteLock>::Update(std::string_view key, Args &&...args) {
  root_parent_.Lock();
  if (nullptr == root_) {
    root_parent_.Unlock();
    return false;
  }
  return UpdateImpl(root_, &root_parent_, key, std::forward<Args>(args)...);
}

template <class ValueType, bool kWriteLock>
template <class... Args>
bool VrtImpl<ValueType, kWriteLock>::UpdateImpl(VrtNode<kWriteLock> *&node, VrtNode<kWriteLock> *parent,
                                                std::string_view key, Args &&...args) {
  node->Lock();
  auto same_prefix_length = VrtNodeHelper<kWriteLock>::CheckSamePrefixLength(node, key);
  if (same_prefix_length < node->key_length) {
    node->Unlock();
    parent->Unlock();
    return false;
  }
  if (key.length() == same_prefix_length) {
    if (node->has_value) {
      auto *new_node =
          VrtNodeHelper<kWriteLock>::template CreateVrtNodeByAddValue<ValueType>(node, std::forward<Args>(args)...);
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
  if (VrtNode<kWriteLock> *&next_node = VrtNodeHelper<kWriteLock>::FindChild(node, key[same_prefix_length]);
      next_node != nullptr) {
    key.remove_prefix(same_prefix_length + 1);
    return UpdateImpl(next_node, node, key, std::forward<Args>(args)...);
  }
  node->Unlock();
  return false;
}

// 插入或更新
template <class ValueType, bool kWriteLock>
template <class... Args>
bool Vrt<ValueType, kWriteLock>::Upsert(std::string_view key, Args &&...args) {
  if (unlikely(key.empty() || key.size() >= kMaxKeySize)) {
    return false;
  }
  auto impl_ptr = impl_;
  return impl_ptr->Upsert(key, std::forward<Args>(args)...);
}

template <class ValueType, bool kWriteLock>
template <class... Args>
bool VrtImpl<ValueType, kWriteLock>::Upsert(std::string_view key, Args &&...args) {
  root_parent_.Lock();
  if (nullptr == root_) {
    root_ = VrtNodeHelper<kWriteLock>::template CreateVrtNode<ValueType>(key, std::forward<Args>(args)...);
    root_parent_.Unlock();
    return true;
  }
  return UpsertImpl(root_, &root_parent_, key, std::forward<Args>(args)...);
}

template <class ValueType, bool kWriteLock>
template <class... Args>
bool VrtImpl<ValueType, kWriteLock>::UpsertImpl(VrtNode<kWriteLock> *&node, VrtNode<kWriteLock> *parent,
                                                std::string_view key, Args &&...args) {
  node->Lock();
  auto same_prefix_length = VrtNodeHelper<kWriteLock>::CheckSamePrefixLength(node, key);
  if (same_prefix_length < key.length() && same_prefix_length < node->key_length) {
    // 同前缀部分作为父节点，旧节点去掉相同部分后作为child1，key剩余部分新建结点作为child2。
    std::string_view new_node_key = key.substr(0, same_prefix_length);
    auto *new_node = VrtNodeHelper<kWriteLock>::CreateVrtNodeWithoutValue(new_node_key);
    auto *child =
        VrtNodeHelper<kWriteLock>::template CreateVrtNodeByRemovePrefix<ValueType>(node, same_prefix_length + 1);
    new_node->AddChild(node->data[same_prefix_length], child);

    char next_char = key[same_prefix_length];
    key.remove_prefix(same_prefix_length + 1);
    child = VrtNodeHelper<kWriteLock>::template CreateVrtNode<ValueType>(key, std::forward<Args>(args)...);
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
    auto *new_node =
        VrtNodeHelper<kWriteLock>::template CreateVrtNode<ValueType>(new_node_key, std::forward<Args>(args)...);
    auto *child =
        VrtNodeHelper<kWriteLock>::template CreateVrtNodeByRemovePrefix<ValueType>(node, same_prefix_length + 1);
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
    auto *new_node =
        VrtNodeHelper<kWriteLock>::template CreateVrtNodeByAddValue<ValueType>(node, std::forward<Args>(args)...);
    node = new_node;
    parent->Unlock();
    UpdateVrt();
    FreeNode(old_node);
    return true;
  }
  parent->Unlock();
  // 继续搜索，没有找到对应子节点，增新增叶子挂在当前节点上
  if (VrtNode<kWriteLock> *&next_node = VrtNodeHelper<kWriteLock>::FindChild(node, key[same_prefix_length]);
      next_node != nullptr) {
    key.remove_prefix(same_prefix_length + 1);
    return UpsertImpl(next_node, node, key, std::forward<Args>(args)...);
  }
  char next_char = key[same_prefix_length];
  key.remove_prefix(same_prefix_length + 1);
  auto *new_node = VrtNodeHelper<kWriteLock>::template CreateVrtNode<ValueType>(key, std::forward<Args>(args)...);
  node->AddChild(next_char, new_node);
  node->Unlock();
  return true;
}

// 删除
template <class ValueType, bool kWriteLock>
bool Vrt<ValueType, kWriteLock>::Delete(std::string_view key) {
  if (unlikely(key.empty() || key.size() >= kMaxKeySize)) {
    return false;
  }
  auto impl_ptr = impl_;
  return impl_ptr->Delete(key);
}

template <class ValueType, bool kWriteLock>
bool VrtImpl<ValueType, kWriteLock>::Delete(std::string_view key) {
  root_parent_.Lock();
  if (nullptr == root_) {
    root_parent_.Unlock();
    return false;
  }
  return DeleteImpl(root_, &root_parent_, key);
}

template <class ValueType, bool kWriteLock>
bool VrtImpl<ValueType, kWriteLock>::DeleteImpl(VrtNode<kWriteLock> *&node, VrtNode<kWriteLock> *parent,
                                                std::string_view key) {
  node->Lock();
  auto same_prefix_length = VrtNodeHelper<kWriteLock>::CheckSamePrefixLength(node, key);
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
        auto *new_node = VrtNodeHelper<kWriteLock>::template CreateVrtNodeByDeleteValue<ValueType>(node);
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
  if (VrtNode<kWriteLock> *&next_node = VrtNodeHelper<kWriteLock>::FindChild(node, key[same_prefix_length]);
      next_node != nullptr) {
    key.remove_prefix(same_prefix_length + 1);
    return DeleteImpl(next_node, node, key);
  }
  node->Unlock();
  return false;
}

}  // namespace vrt
