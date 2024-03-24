#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include "spin_lock.h"
#include "vrt_comm.h"

namespace vrt {

// 注意，key最多只能2^23次方的长度，否则会出错

// memory layout
// mutex_flat ---- has_value 1bit ---- key_length 20bit ---- child_cnt 8bit ---- childs ---- key ---- value ----

template <bool kWriteLock = true>
struct VrtNode {
  SpinLock spin_lock;
  uint32_t has_value : 1;
  uint32_t key_length : 23;
  uint32_t child_cnt : 8;
  VrtNode *childs[kNodeChildMaxCnt];
  char data[0];

  inline void AddChild(char edge, VrtNode *child) {
    childs[static_cast<uint8_t>(edge)] = child;
    child_cnt++;
  }
  inline void Lock() {
    if constexpr (kWriteLock) {
      spin_lock.lock();
    }
  }
  inline void Unlock() {
    if constexpr (kWriteLock) {
      spin_lock.unlock();
    }
  }
};

template <bool kWriteLock = true>
class VrtNodeHelper {
 public:
  inline static uint32_t CheckSamePrefixLength(VrtNode<kWriteLock> *node, std::string_view key) {
    size_t same_prefix_length = 0;
    size_t cmp_size = std::min(static_cast<size_t>(node->key_length), key.length());
    while ((cmp_size--) != 0U) {
      if (node->data[same_prefix_length] != key[same_prefix_length]) {
        break;
      }
      same_prefix_length++;
    }
    return same_prefix_length;
  }

  inline static std::string GetKey(VrtNode<kWriteLock> *node) { return std::string(node->data, node->key_length); }

  template <class ValueType>
  inline static ValueType GetValue(VrtNode<kWriteLock> *node) {
    return *(reinterpret_cast<ValueType *>(node->data + node->key_length));
  }

  inline static VrtNode<kWriteLock> *&FindChild(VrtNode<kWriteLock> *node, char find_char) {
    auto index = static_cast<uint8_t>(find_char);
    return node->childs[index];
  }

  template <class ValueType, class... Args>
  inline static VrtNode<kWriteLock> *CreateVrtNode(std::string_view key, Args &&...args) {
    VrtNode<kWriteLock> *new_node =
        reinterpret_cast<VrtNode<kWriteLock> *>(malloc(sizeof(VrtNode<kWriteLock>) + key.length() + sizeof(ValueType)));
    new_node->has_value = 1;
    new_node->key_length = key.length();
    new_node->child_cnt = 0;
    memset(new_node->childs, 0, sizeof(new_node->childs));
    memcpy(new_node->data, key.data(), new_node->key_length);
    new (new_node->data + new_node->key_length) ValueType(std::forward<Args>(args)...);
    new (&new_node->spin_lock) SpinLock();
    return new_node;
  }

  inline static VrtNode<kWriteLock> *CreateVrtNodeWithoutValue(std::string_view key) {
    VrtNode<kWriteLock> *new_node =
        reinterpret_cast<VrtNode<kWriteLock> *>(malloc(sizeof(VrtNode<kWriteLock>) + key.length()));
    new_node->has_value = 0;
    new_node->key_length = key.length();
    new_node->child_cnt = 0;
    memset(new_node->childs, 0, sizeof(new_node->childs));
    memcpy(new_node->data, key.data(), new_node->key_length);
    new (&new_node->spin_lock) SpinLock();
    return new_node;
  }

  template <class ValueType>
  inline static VrtNode<kWriteLock> *CreateVrtNodeByRemovePrefix(VrtNode<kWriteLock> *node, size_t remove_size) {
    size_t new_node_size =
        sizeof(VrtNode<kWriteLock>) + node->key_length - remove_size + (node->has_value ? sizeof(ValueType) : 0);
    VrtNode<kWriteLock> *new_node = reinterpret_cast<VrtNode<kWriteLock> *>(malloc(new_node_size));
    new_node->has_value = node->has_value;
    new_node->key_length = node->key_length - remove_size;
    new_node->child_cnt = node->child_cnt;
    memcpy(new_node->childs, node->childs, sizeof(new_node->childs));
    memcpy(new_node->data, node->data + remove_size, new_node->key_length);
    if (new_node->has_value) {
      new (new_node->data + new_node->key_length)
          ValueType(*reinterpret_cast<ValueType *>(node->data + node->key_length));
    }
    new (&new_node->spin_lock) SpinLock();
    return new_node;
  }

  template <class ValueType, class... Args>
  inline static VrtNode<kWriteLock> *CreateVrtNodeByAddValue(VrtNode<kWriteLock> *node, Args &&...args) {
    VrtNode<kWriteLock> *new_node = reinterpret_cast<VrtNode<kWriteLock> *>(
        malloc(sizeof(VrtNode<kWriteLock>) + node->key_length + sizeof(ValueType)));
    new_node->has_value = true;
    new_node->key_length = node->key_length;
    new_node->child_cnt = node->child_cnt;
    memcpy(new_node->childs, node->childs, sizeof(new_node->childs));
    memcpy(new_node->data, node->data, node->key_length);
    new (new_node->data + new_node->key_length) ValueType(std::forward<Args>(args)...);
    new (&new_node->spin_lock) SpinLock();
    return new_node;
  }

  template <class ValueType>
  inline static VrtNode<kWriteLock> *CreateVrtNodeByDeleteValue(VrtNode<kWriteLock> *node) {
    VrtNode<kWriteLock> *new_node =
        reinterpret_cast<VrtNode<kWriteLock> *>(malloc(sizeof(VrtNode<kWriteLock>) + node->key_length));
    new_node->has_value = false;
    new_node->key_length = node->key_length;
    new_node->child_cnt = node->child_cnt;
    memcpy(new_node->childs, node->childs, sizeof(new_node->childs));
    memcpy(new_node->data, node->data, node->key_length);
    new (&new_node->spin_lock) SpinLock();
    return new_node;
  }

  template <class ValueType>
  inline static void DestroyNode(VrtNode<kWriteLock> *node) {
    if (node->has_value) {
      auto *value = reinterpret_cast<ValueType *>(node->data + node->key_length);
      value->~ValueType();
    }
    node->spin_lock.~SpinLock();
    free(node);
  }

  template <class ValueType>
  inline static void DestroyTree(VrtNode<kWriteLock> *node) {
    for (int i = 0; i < kNodeChildMaxCnt; i++) {
      if (nullptr == node->childs[i]) {
        continue;
      }
      DestroyTree<ValueType>(node->childs[i]);
    }
    DestroyNode<ValueType>(node);
  }
};

template <bool kWriteLock = true>
class VrtNodeDestroy {
  VrtNodeDestroy() = delete;
  VrtNodeDestroy(VrtNode<kWriteLock> *node) { VrtNodeHelper<kWriteLock>::DestroyNode(node); }
  VrtNodeDestroy(const VrtNodeDestroy &) = delete;
  void operator=(const VrtNodeDestroy &) = delete;  // TODO check operator=
  VrtNodeDestroy(VrtNodeDestroy &&) = delete;
}

}  // namespace vrt