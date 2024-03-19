#pragma once

#include <atomic>

namespace vrt {

template <class ValueType>
class AtomicQueue {
 public:
  struct Node {
    Node() : next(nullptr) {}
    template <class... Args>
    Node(Args &&...args) : value(std::forward<Args>(args)...) {}
    Node(const Node &) = delete;
    void operator=(const Node &) = delete;
    Node(Node &&) = default;

    ValueType value;
    std::atomic<Node *> next;
  };
  AtomicQueue() : head_() {}
  AtomicQueue(const AtomicQueue &) = delete;
  void operator=(const AtomicQueue &) = delete;
  AtomicQueue(AtomicQueue &&) = default;
  ~AtomicQueue() {
    auto *cur = head_.next.load(std::memory_order_acquire);
    while (cur != nullptr) {
      auto *old = cur;
      cur = cur->next.load(std::memory_order_acquire);
      delete old;
    }
  }

  template <class... Args>
  void Push(Args &&...args) {
    auto *new_node = new Node(std::forward<Args>(args)...);
    Node *cur_first_node = head_.next.load(std::memory_order_acquire);
    do {
      new_node->next.store(cur_first_node, std::memory_order_release);
    } while (!head_.next.compare_exchange_strong(cur_first_node, new_node, std::memory_order_release,
                                                 std::memory_order_relaxed));
  }

 private:
  Node head_;
};

}  // namespace vrt