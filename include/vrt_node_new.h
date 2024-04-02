#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include "spin_lock.h"
#include "vrt_comm.h"

namespace vrt {

enum VrtNodeType {
  Node4 = 1,
  Node16 = 2,
  Node48 = 3,
  Node256 = 4,
  LeafNode = 5,
};

template <bool kWriteLock = true>
struct VrtNode {
  SpinLock spin_lock;
  uint32_t type : 3;
  uint32_t has_value : 1;
  uint32_t key_length : 20;
  uint32_t child_cnt : 8;

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
struct VrtNode4 : public VrtNode<kWriteLock> {
  char edge[kFour];
  VrtNode<kWriteLock> *childs[kFour];
  char data[0];
};

template <bool kWriteLock = true>
struct VrtNode16 : public VrtNode<kWriteLock> {
  char edge[kSixteen];
  VrtNode<kWriteLock> *childs[kSixteen];
  char data[0];
};

template <bool kWriteLock = true>
struct VrtNode48 : public VrtNode<kWriteLock> {
  char childs_index[kTwoFiveSix];
  VrtNode<kWriteLock> *childs[kFortyEight];
  char data[0];
};

template <bool kWriteLock = true>
struct VrtNode256 : public VrtNode<kWriteLock> {
  VrtNode<kWriteLock> *childs[kTwoFiveSix];
  char data[0];
};

template <bool kWriteLock = true>
struct VrtLeafNode : public VrtNode<kWriteLock> {
  char data[0];
};

template <bool kWriteLock = true>
class VrtNodeHelper {
 public:
  inline static uint32_t CheckSamePrefixLength(VrtNode<kWriteLock> *node, std::string_view key) {
    char *data;
    switch (node->type) {
      case Node4: {
        data = static_cast<VrtNode4<kWriteLock> *>(node)->data;
      } break;
      case Node16: {
        data = static_cast<VrtNode16<kWriteLock> *>(node)->data;
      } break;
      case Node48: {
        data = static_cast<VrtNode48<kWriteLock> *>(node)->data;
      } break;
      case Node256: {
        data = static_cast<VrtNode256<kWriteLock> *>(node)->data;
      } break;
      case LeafNode: {
        data = static_cast<VrtLeafNode<kWriteLock> *>(node)->data;
      } break;
      default:
        assert(false);
    }
    size_t same_prefix_length = 0;
    size_t cmp_size = std::min(static_cast<size_t>(node->key_length), key.length());
    while ((cmp_size--) != 0U) {
      if (data[same_prefix_length] != key[same_prefix_length]) {
        break;
      }
      same_prefix_length++;
    }
    return same_prefix_length;
  }

  inline static std::string GetKey(VrtNode<kWriteLock> *node) {
    switch (node->type) {
      case Node4: {
        return std::string(static_cast<VrtNode4<kWriteLock> *>(node)->data, node->key_length);
      } break;
      case Node16: {
        return std::string(static_cast<VrtNode16<kWriteLock> *>(node)->data, node->key_length);
      } break;
      case Node48: {
        return std::string(static_cast<VrtNode48<kWriteLock> *>(node)->data, node->key_length);
      } break;
      case Node256: {
        return std::string(static_cast<VrtNode256<kWriteLock> *>(node)->data, node->key_length);
      } break;
      case LeafNode: {
        return std::string(static_cast<VrtLeafNode<kWriteLock> *>(node)->data, node->key_length);
      } break;
      default:
        assert(false);
    }
    return "";
  }

  template <class ValueType>
  inline static ValueType GetValue(VrtNode<kWriteLock> *node) {
    switch (node->type) {
      case Node4: {
        return *(reinterpret_cast<ValueType *>(static_cast<VrtNode4<kWriteLock> *>(node)->data + node->key_length));
      } break;
      case Node16: {
        return *(reinterpret_cast<ValueType *>(static_cast<VrtNode16<kWriteLock> *>(node)->data + node->key_length));
      } break;
      case Node48: {
        return *(reinterpret_cast<ValueType *>(static_cast<VrtNode48<kWriteLock> *>(node)->data + node->key_length));
      } break;
      case Node256: {
        return *(reinterpret_cast<ValueType *>(static_cast<VrtNode256<kWriteLock> *>(node)->data + node->key_length));
      } break;
      case LeafNode: {
        return *(reinterpret_cast<ValueType *>(static_cast<VrtLeafNode<kWriteLock> *>(node)->data + node->key_length));
      } break;
      default:
        assert(false);
    }
    return ValueType();
  }

  inline static VrtNode<kWriteLock> *&FindChild(VrtNode<kWriteLock> *node, char find_char) {
    switch (node->type) {
      case Node4: {
        auto *node4 = static_cast<VrtNode4<kWriteLock> *>(node);
        for (int i = 0; i < node4->child_cnt_; i++) {
          if (node4->edge_[i] == find_char) {
            return node4->childs_[i];
          }
        }
      } break;
      case Node16: {
        auto *node16 = static_cast<VrtNode16<kWriteLock> *>(node);
        for (int i = 0; i < node16->child_cnt; i++) {
          if (node16->edge[i] == find_char) {
            return node16->childs_[i];
          }
        }
      } break;
      case Node48: {
        auto *node48 = static_cast<VrtNode48<kWriteLock> *>(node);
        if (auto index = node48->childs_index[static_cast<uint8_t>(find_char)]; index != -1) {
          return node48->childs[static_cast<uint8_t>(index)];
        }
      } break;
      case Node256: {
        auto *node256 = static_cast<VrtNode256<kWriteLock> *>(node);
        auto index = static_cast<uint8_t>(find_char);
        return node256->childs[index];
      } break;
      default:
        assert(false);
    }
    return kVrtNodeNullObject;
  }

  // TODO addchild

  template <VrtNodeType node_type, class ValueType, class... Args>
  inline static VrtNode<kWriteLock> *CreateVrtNode(std::string_view key, Args &&...args) {
    if constexpr (Node4 == node_type) {
      auto *new_node = reinterpret_cast<VrtNode4<kWriteLock> *>(
          malloc(sizeof(VrtNode4<kWriteLock>) + key.length() + sizeof(ValueType)));
      new_node->type = Node4;
      new_node->has_value = 1;
      new_node->key_length = key.length();
      new_node->child_cnt = 0;
      memcpy(new_node->data, key.data(), new_node->key_length);
      new (new_node->data + new_node->key_length) ValueType(std::forward<Args>(args)...);
      new (&new_node->spin_lock) SpinLock();
      return new_node;
    } else if constexpr (Node16 == node_type) {
      auto *new_node = reinterpret_cast<VrtNode16<kWriteLock> *>(
          malloc(sizeof(VrtNode16<kWriteLock>) + key.length() + sizeof(ValueType)));
      new_node->type = Node16;
      new_node->has_value = 1;
      new_node->key_length = key.length();
      new_node->child_cnt = 0;
      memcpy(new_node->data, key.data(), new_node->key_length);
      new (new_node->data + new_node->key_length) ValueType(std::forward<Args>(args)...);
      new (&new_node->spin_lock) SpinLock();
      return new_node;
    } else if constexpr (Node48 == node_type) {
      auto *new_node = reinterpret_cast<VrtNode48<kWriteLock> *>(
          malloc(sizeof(VrtNode48<kWriteLock>) + key.length() + sizeof(ValueType)));
      memset(new_node->childs_index, -1, sizeof(new_node->childs_index));
      new_node->type = Node48;
      new_node->has_value = 1;
      new_node->key_length = key.length();
      new_node->child_cnt = 0;
      memcpy(new_node->data, key.data(), new_node->key_length);
      new (new_node->data + new_node->key_length) ValueType(std::forward<Args>(args)...);
      new (&new_node->spin_lock) SpinLock();
      return new_node;
    } else if constexpr (Node256 == node_type) {
      auto *new_node = reinterpret_cast<VrtNode256<kWriteLock> *>(
          malloc(sizeof(VrtNode256<kWriteLock>) + key.length() + sizeof(ValueType)));
      memset(new_node->childs, 0, sizeof(new_node->childs));
      new_node->type = Node256;
      new_node->has_value = 1;
      new_node->key_length = key.length();
      new_node->child_cnt = 0;
      memcpy(new_node->data, key.data(), new_node->key_length);
      new (new_node->data + new_node->key_length) ValueType(std::forward<Args>(args)...);
      new (&new_node->spin_lock) SpinLock();
      return new_node;
    } else if constexpr (LeafNode == node_type) {
      auto *new_node = reinterpret_cast<VrtLeafNode<kWriteLock> *>(
          malloc(sizeof(VrtLeafNode<kWriteLock>) + key.length() + sizeof(ValueType)));
      new_node->type = LeafNode;
      new_node->has_value = 1;
      new_node->key_length = key.length();
      new_node->child_cnt = 0;
      memcpy(new_node->data, key.data(), new_node->key_length);
      new (new_node->data + new_node->key_length) ValueType(std::forward<Args>(args)...);
      new (&new_node->spin_lock) SpinLock();
      return new_node;
    }
  }

  template <VrtNodeType node_type>
  inline static VrtNode<kWriteLock> *CreateVrtNodeWithoutValue(std::string_view key) {
    if constexpr (Node4 == node_type) {
      auto *new_node = reinterpret_cast<VrtNode4<kWriteLock> *>(malloc(sizeof(VrtNode4<kWriteLock>) + key.length()));
      new_node->type = Node4;
      new_node->has_value = 0;
      new_node->key_length = key.length();
      new_node->child_cnt = 0;
      memcpy(new_node->data, key.data(), new_node->key_length);
      new (&new_node->spin_lock) SpinLock();
      return new_node;
    } else if constexpr (Node16 == node_type) {
      auto *new_node = reinterpret_cast<VrtNode16<kWriteLock> *>(malloc(sizeof(VrtNode16<kWriteLock>) + key.length()));
      new_node->type = Node16;
      new_node->has_value = 0;
      new_node->key_length = key.length();
      new_node->child_cnt = 0;
      memcpy(new_node->data, key.data(), new_node->key_length);
      new (&new_node->spin_lock) SpinLock();
      return new_node;
    } else if constexpr (Node48 == node_type) {
      auto *new_node = reinterpret_cast<VrtNode48<kWriteLock> *>(malloc(sizeof(VrtNode48<kWriteLock>) + key.length()));
      new_node->type = Node48;
      memset(new_node->childs_index, -1, sizeof(new_node->childs_index));
      new_node->has_value = 0;
      new_node->key_length = key.length();
      new_node->child_cnt = 0;
      memcpy(new_node->data, key.data(), new_node->key_length);
      new (&new_node->spin_lock) SpinLock();
      return new_node;
    } else if constexpr (Node256 == node_type) {
      auto *new_node =
          reinterpret_cast<VrtNode256<kWriteLock> *>(malloc(sizeof(VrtNode256<kWriteLock>) + key.length()));
      new_node->type = Node256;
      memset(new_node->childs, 0, sizeof(new_node->childs));
      new_node->has_value = 0;
      new_node->key_length = key.length();
      new_node->child_cnt = 0;
      memcpy(new_node->data, key.data(), new_node->key_length);
      new (&new_node->spin_lock) SpinLock();
      return new_node;
    } else if constexpr (LeafNode == node_type) {
      auto *new_node =
          reinterpret_cast<VrtLeafNode<kWriteLock> *>(malloc(sizeof(VrtLeafNode<kWriteLock>) + key.length()));
      new_node->type = LeafNode;
      new_node->has_value = 0;
      new_node->key_length = key.length();
      new_node->child_cnt = 0;
      memcpy(new_node->data, key.data(), new_node->key_length);
      new (&new_node->spin_lock) SpinLock();
      return new_node;
    }
  }

  template <class ValueType>
  inline static VrtNode<kWriteLock> *CreateVrtNodeByRemovePrefix(VrtNode<kWriteLock> *node, size_t remove_size) {
    switch (node->type) {
      case Node4: {
        size_t new_node_size =
            sizeof(VrtNode4<kWriteLock>) + node->key_length - remove_size + (node->has_value ? sizeof(ValueType) : 0);
        VrtNode4<kWriteLock> *new_node = reinterpret_cast<VrtNode4<kWriteLock> *>(malloc(new_node_size));
        auto *old_node = reinterpret_cast<VrtNode4<kWriteLock> *>(node);
        new_node->type = Node4;
        new_node->has_value = old_node->has_value;
        new_node->key_length = old_node->key_length - remove_size;
        new_node->child_cnt = old_node->child_cnt;
        memcpy(new_node->edge, old_node->edge, sizeof(new_node->edge));
        memcpy(new_node->childs, old_node->childs, sizeof(new_node->childs));
        memcpy(new_node->data, old_node->data + remove_size, new_node->key_length);
        if (new_node->has_value) {
          new (new_node->data + new_node->key_length)
              ValueType(*reinterpret_cast<ValueType *>(old_node->data + old_node->key_length));
        }
        new (&new_node->spin_lock) SpinLock();
        return new_node;
      } break;
      case Node16: {
        size_t new_node_size =
            sizeof(VrtNode16<kWriteLock>) + node->key_length - remove_size + (node->has_value ? sizeof(ValueType) : 0);
        VrtNode16<kWriteLock> *new_node = reinterpret_cast<VrtNode16<kWriteLock> *>(malloc(new_node_size));
        auto *old_node = reinterpret_cast<VrtNode16<kWriteLock> *>(node);
        new_node->type = Node16;
        new_node->has_value = old_node->has_value;
        new_node->key_length = old_node->key_length - remove_size;
        new_node->child_cnt = old_node->child_cnt;
        memcpy(new_node->edge, old_node->edge, sizeof(new_node->edge));
        memcpy(new_node->childs, old_node->childs, sizeof(new_node->childs));
        memcpy(new_node->data, old_node->data + remove_size, new_node->key_length);
        if (new_node->has_value) {
          new (new_node->data + new_node->key_length)
              ValueType(*reinterpret_cast<ValueType *>(old_node->data + old_node->key_length));
        }
        new (&new_node->spin_lock) SpinLock();
        return new_node;
      } break;
      case Node48: {
        size_t new_node_size =
            sizeof(VrtNode48<kWriteLock>) + node->key_length - remove_size + (node->has_value ? sizeof(ValueType) : 0);
        VrtNode48<kWriteLock> *new_node = reinterpret_cast<VrtNode48<kWriteLock> *>(malloc(new_node_size));
        auto *old_node = reinterpret_cast<VrtNode48<kWriteLock> *>(node);
        new_node->type = Node48;
        new_node->has_value = old_node->has_value;
        new_node->key_length = old_node->key_length - remove_size;
        new_node->child_cnt = old_node->child_cnt;
        memcpy(new_node->childs_index, old_node->childs_index, sizeof(new_node->childs_index));
        memcpy(new_node->childs, old_node->childs, sizeof(new_node->childs));
        memcpy(new_node->data, old_node->data + remove_size, new_node->key_length);
        if (new_node->has_value) {
          new (new_node->data + new_node->key_length)
              ValueType(*reinterpret_cast<ValueType *>(old_node->data + old_node->key_length));
        }
        new (&new_node->spin_lock) SpinLock();
        return new_node;
      } break;
      case Node256: {
        size_t new_node_size =
            sizeof(VrtNode256<kWriteLock>) + node->key_length - remove_size + (node->has_value ? sizeof(ValueType) : 0);
        VrtNode256<kWriteLock> *new_node = reinterpret_cast<VrtNode256<kWriteLock> *>(malloc(new_node_size));
        auto *old_node = reinterpret_cast<VrtNode256<kWriteLock> *>(node);
        new_node->type = Node256;
        new_node->has_value = old_node->has_value;
        new_node->key_length = old_node->key_length - remove_size;
        new_node->child_cnt = old_node->child_cnt;
        memcpy(new_node->childs, old_node->childs, sizeof(new_node->childs));
        memcpy(new_node->data, old_node->data + remove_size, new_node->key_length);
        if (new_node->has_value) {
          new (new_node->data + new_node->key_length)
              ValueType(*reinterpret_cast<ValueType *>(old_node->data + old_node->key_length));
        }
        new (&new_node->spin_lock) SpinLock();
        return new_node;
      } break;
      case LeafNode: {
        size_t new_node_size = sizeof(VrtLeafNode<kWriteLock>) + node->key_length - remove_size +
                               (node->has_value ? sizeof(ValueType) : 0);
        VrtLeafNode<kWriteLock> *new_node = reinterpret_cast<VrtLeafNode<kWriteLock> *>(malloc(new_node_size));
        auto *old_node = reinterpret_cast<VrtLeafNode<kWriteLock> *>(node);
        new_node->type = LeafNode;
        new_node->has_value = old_node->has_value;
        new_node->key_length = old_node->key_length - remove_size;
        new_node->child_cnt = old_node->child_cnt;
        memcpy(new_node->data, old_node->data + remove_size, new_node->key_length);
        if (new_node->has_value) {
          new (new_node->data + new_node->key_length)
              ValueType(*reinterpret_cast<ValueType *>(old_node->data + old_node->key_length));
        }
        new (&new_node->spin_lock) SpinLock();
        return new_node;
      } break;
      default:
        assert(false);
    }

    return nullptr;
  }

  template <class ValueType, class... Args>
  inline static VrtNode<kWriteLock> *CreateVrtNodeByAddValue(VrtNode<kWriteLock> *node, Args &&...args) {
    switch (node->type) {
      case Node4: {
        auto *old_node = reinterpret_cast<VrtNode4<kWriteLock> *>(node);
        auto *new_node = reinterpret_cast<VrtNode4<kWriteLock> *>(
            malloc(sizeof(VrtNode4<kWriteLock>) + old_node->key_length + sizeof(ValueType)));
        new_node->type = Node4;
        new_node->has_value = true;
        new_node->key_length = old_node->key_length;
        new_node->child_cnt = old_node->child_cnt;
        memcpy(new_node->edge, old_node->edge, sizeof(new_node->edge));
        memcpy(new_node->childs, old_node->childs, sizeof(new_node->childs));
        memcpy(new_node->data, old_node->data, old_node->key_length);
        new (new_node->data + new_node->key_length) ValueType(std::forward<Args>(args)...);
        new (&new_node->spin_lock) SpinLock();
        return new_node;
      } break;
      case Node16: {
        auto *old_node = reinterpret_cast<VrtNode16<kWriteLock> *>(node);
        auto *new_node = reinterpret_cast<VrtNode16<kWriteLock> *>(
            malloc(sizeof(VrtNode16<kWriteLock>) + old_node->key_length + sizeof(ValueType)));
        new_node->type = Node16;
        new_node->has_value = true;
        new_node->key_length = old_node->key_length;
        new_node->child_cnt = old_node->child_cnt;
        memcpy(new_node->edge, old_node->edge, sizeof(new_node->edge));
        memcpy(new_node->childs, old_node->childs, sizeof(new_node->childs));
        memcpy(new_node->data, old_node->data, old_node->key_length);
        new (new_node->data + new_node->key_length) ValueType(std::forward<Args>(args)...);
        new (&new_node->spin_lock) SpinLock();
        return new_node;
      } break;
      case Node48: {
        auto *old_node = reinterpret_cast<VrtNode48<kWriteLock> *>(node);
        auto *new_node = reinterpret_cast<VrtNode48<kWriteLock> *>(
            malloc(sizeof(VrtNode48<kWriteLock>) + old_node->key_length + sizeof(ValueType)));
        new_node->type = Node48;
        new_node->has_value = true;
        new_node->key_length = old_node->key_length;
        new_node->child_cnt = old_node->child_cnt;
        memcpy(new_node->childs_index, old_node->childs_index, sizeof(new_node->childs_index));
        memcpy(new_node->childs, old_node->childs, sizeof(new_node->childs));
        memcpy(new_node->data, old_node->data, old_node->key_length);
        new (new_node->data + new_node->key_length) ValueType(std::forward<Args>(args)...);
        new (&new_node->spin_lock) SpinLock();
        return new_node;
      } break;
      case Node256: {
        auto *old_node = reinterpret_cast<VrtNode256<kWriteLock> *>(node);
        auto *new_node = reinterpret_cast<VrtNode256<kWriteLock> *>(
            malloc(sizeof(VrtNode256<kWriteLock>) + old_node->key_length + sizeof(ValueType)));
        new_node->type = Node256;
        new_node->has_value = true;
        new_node->key_length = old_node->key_length;
        new_node->child_cnt = old_node->child_cnt;
        memcpy(new_node->childs, old_node->childs, sizeof(new_node->childs));
        memcpy(new_node->data, old_node->data, old_node->key_length);
        new (new_node->data + new_node->key_length) ValueType(std::forward<Args>(args)...);
        new (&new_node->spin_lock) SpinLock();
        return new_node;
      } break;
      case LeafNode: {
        auto *old_node = reinterpret_cast<VrtLeafNode<kWriteLock> *>(node);
        auto *new_node = reinterpret_cast<VrtLeafNode<kWriteLock> *>(
            malloc(sizeof(VrtLeafNode<kWriteLock>) + old_node->key_length + sizeof(ValueType)));
        new_node->type = LeafNode;
        new_node->has_value = true;
        new_node->key_length = old_node->key_length;
        new_node->child_cnt = old_node->child_cnt;
        memcpy(new_node->data, old_node->data, old_node->key_length);
        new (new_node->data + new_node->key_length) ValueType(std::forward<Args>(args)...);
        new (&new_node->spin_lock) SpinLock();
        return new_node;
      } break;
      default:
        assert(false);
    }
    return nullptr;
  }

  template <class ValueType>
  inline static VrtNode<kWriteLock> *CreateVrtNodeByDeleteValue(VrtNode<kWriteLock> *node) {
    switch (node->type) {
      case Node4: {
        auto *old_node = reinterpret_cast<VrtNode4<kWriteLock> *>(node);
        auto *new_node =
            reinterpret_cast<VrtNode4<kWriteLock> *>(malloc(sizeof(VrtNode4<kWriteLock>) + old_node->key_length));
        new_node->type = Node4;
        new_node->has_value = false;
        new_node->key_length = old_node->key_length;
        new_node->child_cnt = old_node->child_cnt;
        memcpy(new_node->edge, old_node->edge, sizeof(new_node->edge));
        memcpy(new_node->childs, old_node->childs, sizeof(new_node->childs));
        memcpy(new_node->data, old_node->data, old_node->key_length);
        new (&new_node->spin_lock) SpinLock();
        return new_node;
      } break;
      case Node16: {
        auto *old_node = reinterpret_cast<VrtNode16<kWriteLock> *>(node);
        auto *new_node =
            reinterpret_cast<VrtNode16<kWriteLock> *>(malloc(sizeof(VrtNode16<kWriteLock>) + old_node->key_length));
        new_node->type = Node16;
        new_node->has_value = false;
        new_node->key_length = old_node->key_length;
        new_node->child_cnt = old_node->child_cnt;
        memcpy(new_node->edge, old_node->edge, sizeof(new_node->edge));
        memcpy(new_node->childs, old_node->childs, sizeof(new_node->childs));
        memcpy(new_node->data, old_node->data, old_node->key_length);
        new (&new_node->spin_lock) SpinLock();
        return new_node;
      } break;
      case Node48: {
        auto *old_node = reinterpret_cast<VrtNode48<kWriteLock> *>(node);
        auto *new_node =
            reinterpret_cast<VrtNode48<kWriteLock> *>(malloc(sizeof(VrtNode48<kWriteLock>) + old_node->key_length));
        new_node->type = Node48;
        new_node->has_value = false;
        new_node->key_length = old_node->key_length;
        new_node->child_cnt = old_node->child_cnt;
        memcpy(new_node->childs_index, old_node->childs_index, sizeof(new_node->childs_index));
        memcpy(new_node->childs, old_node->childs, sizeof(new_node->childs));
        memcpy(new_node->data, old_node->data, old_node->key_length);
        new (&new_node->spin_lock) SpinLock();
        return new_node;
      } break;
      case Node256: {
        auto *old_node = reinterpret_cast<VrtNode256<kWriteLock> *>(node);
        auto *new_node =
            reinterpret_cast<VrtNode256<kWriteLock> *>(malloc(sizeof(VrtNode256<kWriteLock>) + old_node->key_length));
        new_node->type = Node256;
        new_node->has_value = false;
        new_node->key_length = old_node->key_length;
        new_node->child_cnt = old_node->child_cnt;
        memcpy(new_node->childs, old_node->childs, sizeof(new_node->childs));
        memcpy(new_node->data, old_node->data, old_node->key_length);
        new (&new_node->spin_lock) SpinLock();
        return new_node;
      } break;
      case LeafNode: {
        auto *old_node = reinterpret_cast<VrtLeafNode<kWriteLock> *>(node);
        auto *new_node =
            reinterpret_cast<VrtLeafNode<kWriteLock> *>(malloc(sizeof(VrtLeafNode<kWriteLock>) + old_node->key_length));
        new_node->type = LeafNode;
        new_node->has_value = false;
        new_node->key_length = old_node->key_length;
        new_node->child_cnt = old_node->child_cnt;
        memcpy(new_node->data, old_node->data, old_node->key_length);
        new (&new_node->spin_lock) SpinLock();
        return new_node;
      } break;
      default:
        assert(false);
    }
    return nullptr;
  }

  template <class ValueType>
  inline static void DestroyNode(VrtNode<kWriteLock> *node) {
    switch (node->type) {
      case Node4: {
        auto *real_node = reinterpret_cast<VrtNode4<kWriteLock> *>(node);
        if (node->has_value) {
          auto *value = reinterpret_cast<ValueType *>(real_node->data + real_node->key_length);
          value->~ValueType();
        }
        real_node->spin_lock.~SpinLock();
        free(real_node);
      } break;
      case Node16: {
        auto *real_node = reinterpret_cast<VrtNode16<kWriteLock> *>(node);
        if (node->has_value) {
          auto *value = reinterpret_cast<ValueType *>(real_node->data + real_node->key_length);
          value->~ValueType();
        }
        real_node->spin_lock.~SpinLock();
        free(real_node);
      } break;
      case Node48: {
        auto *real_node = reinterpret_cast<VrtNode48<kWriteLock> *>(node);
        if (node->has_value) {
          auto *value = reinterpret_cast<ValueType *>(real_node->data + real_node->key_length);
          value->~ValueType();
        }
        real_node->spin_lock.~SpinLock();
        free(real_node);
      } break;
      case Node256: {
        auto *real_node = reinterpret_cast<VrtNode256<kWriteLock> *>(node);
        if (node->has_value) {
          auto *value = reinterpret_cast<ValueType *>(real_node->data + real_node->key_length);
          value->~ValueType();
        }
        real_node->spin_lock.~SpinLock();
        free(real_node);
      } break;
      case LeafNode: {
        auto *real_node = reinterpret_cast<VrtLeafNode<kWriteLock> *>(node);
        if (node->has_value) {
          auto *value = reinterpret_cast<ValueType *>(real_node->data + real_node->key_length);
          value->~ValueType();
        }
        real_node->spin_lock.~SpinLock();
        free(real_node);
      } break;
      default:
        assert(false);
    }
  }

  template <class ValueType>
  inline static void DestroyTree(VrtNode<kWriteLock> *node) {
    switch (node->type) {
      case Node4: {
        auto *real_node = reinterpret_cast<VrtNode4<kWriteLock> *>(node);
        for (int i = 0; i < real_node->child_cnt; i++) {
          DestroyTree<ValueType>(real_node->childs[i]);
        }
        if (node->has_value) {
          auto *value = reinterpret_cast<ValueType *>(real_node->data + real_node->key_length);
          value->~ValueType();
        }
        real_node->spin_lock.~SpinLock();
        free(real_node);
      } break;
      case Node16: {
        auto *real_node = reinterpret_cast<VrtNode16<kWriteLock> *>(node);
        for (int i = 0; i < real_node->child_cnt; i++) {
          DestroyTree<ValueType>(real_node->childs[i]);
        }
        if (node->has_value) {
          auto *value = reinterpret_cast<ValueType *>(real_node->data + real_node->key_length);
          value->~ValueType();
        }
        real_node->spin_lock.~SpinLock();
        free(real_node);
      } break;
      case Node48: {
        auto *real_node = reinterpret_cast<VrtNode48<kWriteLock> *>(node);
        for (int i = 0; i < real_node->child_cnt; i++) {
          DestroyTree<ValueType>(real_node->childs[i]);
        }
        if (node->has_value) {
          auto *value = reinterpret_cast<ValueType *>(real_node->data + real_node->key_length);
          value->~ValueType();
        }
        real_node->spin_lock.~SpinLock();
        free(real_node);
      } break;
      case Node256: {
        auto *real_node = reinterpret_cast<VrtNode256<kWriteLock> *>(node);
        for (int i = 0; i < kNodeChildMaxCnt; i++) {
          if (nullptr == real_node->childs[i]) {
            continue;
          }
          DestroyTree<ValueType>(real_node->childs[i]);
        }
        if (node->has_value) {
          auto *value = reinterpret_cast<ValueType *>(real_node->data + real_node->key_length);
          value->~ValueType();
        }
        real_node->spin_lock.~SpinLock();
        free(real_node);
      } break;
      case LeafNode: {
        auto *real_node = reinterpret_cast<VrtLeafNode<kWriteLock> *>(node);
        if (node->has_value) {
          auto *value = reinterpret_cast<ValueType *>(real_node->data + real_node->key_length);
          value->~ValueType();
        }
        real_node->spin_lock.~SpinLock();
        free(real_node);
      } break;
      default:
        assert(false);
    }
  }

 private:
  static VrtNode<kWriteLock> *kVrtNodeNullObject;
};

template <class ValueType, bool kWriteLock = true>
class VrtNodeDestroy {
 public:
  VrtNodeDestroy() = delete;
  VrtNodeDestroy(VrtNode<kWriteLock> *node) { VrtNodeHelper<kWriteLock>::template DestroyNode<ValueType>(node); }
  VrtNodeDestroy(const VrtNodeDestroy &) = delete;
  VrtNodeDestroy &operator=(const VrtNodeDestroy &) = delete;
  VrtNodeDestroy(VrtNodeDestroy &&) = delete;
};

template <bool kWriteLock>
VrtNode<kWriteLock> *VrtNodeHelper<kWriteLock>::kVrtNodeNullObject{nullptr};

}  // namespace vrt