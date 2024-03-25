#pragma once

#include <array>
#include <atomic>
#include <functional>

namespace vrt {

constexpr uint8_t kEpochSize = 3;
constexpr uint8_t kCacheLineSize = 64;

struct TLS {
  TLS() = delete;
  TLS(std::function<void(TLS *)> r_f, std::function<void(TLS *)> ur_f)
      : active(false), epoch(0), unregister_func(std::move(ur_f)) {
    r_f(this);
  }
  TLS(TLS &) = delete;
  TLS(TLS &&) = delete;
  void operator=(const TLS &) = delete;
  ~TLS() { unregister_func(this); }
  std::atomic_flag active;
  std::atomic<uint8_t> epoch;
  uint32_t tid;
  std::function<void(TLS *)> unregister_func;
} __attribute__((aligned(kCacheLineSize)));

template <class RCObject, class DestroyClass, uint32_t kReadThreadNum>
class EbrManager {
 public:
  EbrManager(const EbrManager<RCObject, DestroyClass, kReadThreadNum> &) = delete;
  EbrManager(EbrManager<RCObject, DestroyClass, kReadThreadNum> &&) = delete;
  EbrManager &operator=(const EbrManager<RCObject, DestroyClass, kReadThreadNum> &) = delete;
  ~EbrManager() {
    for (int i = 0; i < kEpochSize; i++) {
      ClearRetireList(i);
    }
  }

  EbrManager() : global_epoch_(0), update_(false), write_cnt_(0), thread_cnt_(0) {
    std::fill(tls_list_.begin(), tls_list_.end(), nullptr);
    for (int i = 0; i < kEpochSize; i++) {
      retire_list_[i].store(nullptr, std::memory_order_release);
    }
  }

  inline void StartRead() {
    auto &tls = GetTLS();
    tls.active.test_and_set(std::memory_order_release);
    tls.epoch.store(global_epoch_.load(std::memory_order_acquire), std::memory_order_release);
  }

  inline void EndRead() { GetTLS().active.clear(std::memory_order_release); }

  inline void FreeObject(RCObject *object) {
    auto epoch = global_epoch_.load(std::memory_order_acquire);
    auto *node = new RetireNode;
    node->obj = object;
    do {
      node->next = retire_list_[epoch].load(std::memory_order_acquire);
    } while (!retire_list_[epoch].compare_exchange_weak(node->next, node, std::memory_order_acq_rel));

    auto write_cnt = write_cnt_.fetch_add(1, std::memory_order_relaxed);
    if (write_cnt > kReadThreadNum) {
      if (!update_.test_and_set(std::memory_order_acq_rel)) {
        TryGC();
        update_.clear(std::memory_order_release);
      }
    }
  }

 private:
  inline void TLSRegister(TLS *tls) {
    uint32_t thread_id;
    do {
      thread_id = thread_cnt_.fetch_add(1, std::memory_order_relaxed);
    } while (tls_list_[thread_id] != nullptr);
    tls->tid = thread_id;
    tls_list_[thread_id] = tls;
  }

  inline void TLSUnRegister(TLS *tls) { tls_list_[tls->tid] = nullptr; }

  inline TLS &GetTLS() {
    thread_local TLS tls(
        std::bind(&EbrManager<RCObject, DestroyClass, kReadThreadNum>::TLSRegister, this, std::placeholders::_1),
        std::bind(&EbrManager<RCObject, DestroyClass, kReadThreadNum>::TLSUnRegister, this, std::placeholders::_1));
    return tls;
  }

  inline void TryGC() {
    auto epoch = global_epoch_.load(std::memory_order_acquire);
    // TODO 记录上一次搜索到的位置
    for (int i = 0; i < tls_list_.size(); i++) {
      if (tls_list_[i] == nullptr) {
        continue;
      }
      if (tls_list_[i]->active.test() && tls_list_[i]->epoch.load(std::memory_order::memory_order_acquire) != epoch) {
        return;
      }
    }
    global_epoch_.store((epoch + 1) % kEpochSize, std::memory_order_release);
    ClearRetireList((epoch + 2) % kEpochSize);
    write_cnt_.store(0, std::memory_order_relaxed);
  }

  inline void ClearRetireList(int index) {
    auto *retire_node = retire_list_[index].load(std::memory_order_acquire);
    while (retire_node != nullptr) {
      DestroyClass destroy(retire_node->obj);
      auto *old_node = retire_node;
      retire_node = retire_node->next;
      delete old_node;
    }
    retire_list_[index].store(nullptr, std::memory_order_release);
  }

  struct RetireNode {
    RCObject *obj;
    RetireNode *next;
  };
  std::array<TLS *, kReadThreadNum> tls_list_;
  std::atomic<uint8_t> global_epoch_;
  char padding[kCacheLineSize - sizeof(std::atomic<uint8_t>)];  // TODO 待验证是否成功隔离
  std::atomic_flag update_;
  std::atomic<uint32_t> write_cnt_;
  std::atomic<RetireNode *> retire_list_[kEpochSize];
  std::atomic<uint32_t> thread_cnt_;
};

}  // namespace vrt