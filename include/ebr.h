#pragma once

#include <array>
#include <atomic>
#include <mutex>
#include <vector>

namespace vrt {

constexpr uint8_t kEpochSize = 3;
constexpr uint8_t kCacheLineSize = 64;

// 思路，有一个全局的线程id分配器，维护了n个线程id。
// GetThreadID时创建一个线程唯一变量，这个变量初始化会从线程id分配器里获取一个可用的线程id，销毁时会释放这个线程id

class ThreadIdManager;

struct ThreadID {
  ThreadID() {
    tid = ThreadIdManager::AcquireThreadID();
  }
  ~ThreadID() {
    ThreadIdManager::ReleaseThreadID(tid);
  }
  uint32_t tid;
};

class ThreadIdManager {
 public:
  static uint32_t AcquireThreadID() {
    std::lock_guard<std::mutex> lock(tid_list_mutex_);
    auto tid = tid_list_.back();
    tid_list_.pop_back();
    return tid;
  }
  static uint32_t ReleaseThreadID(const uint32_t tid) {
    std::lock_guard<std::mutex> lock(tid_list_mutex_);
    tid_list_.emplace_back(tid);
  }

 private:
  static std::vector<uint32_t> tid_list_;
  static std::mutex tid_list_mutex_;
};

template <class RCObject, class DestroyClass, uint32_t thread_num>
class EbrManager {
 public:
  EbrManager(const EbrManager<RCObject, DestroyClass, thread_num> &) = delete;
  EbrManager(EbrManager<RCObject, DestroyClass, thread_num> &&) = delete;
  void operator=(const EbrManager<RCObject, DestroyClass, thread_num> &) = delete;
  ~EbrManager() {
    for (int i = 0; i < kEpochSize; i++) {
      ClearRetireList(i);
    }
  }

  EbrManager() : tls_list_(), global_epoch_(0), update_(false) {
    for (int i = 0; i < kEpochSize; i++) {
      retire_list_[i].store(nullptr, std::memory_order_release);
    }
  }

  inline void StartRead() {
    auto thread_id = GetThreadID();
    tls_list_[thread_id].active.test_and_set(std::memory_order_release);
    tls_list_[thread_id].epoch.store(global_epoch_.load(std::memory_order_acquire), std::memory_order_release);
  }

  inline void EndRead() {
    auto thread_id = GetThreadID();
    tls_list_[thread_id].active.clear(std::memory_order_release);
  }

  inline void FreeObject(RCObject *object) {
    auto epoch = global_epoch_.load(std::memory_order_acquire);
    auto *node = new RetireNode;
    node->obj = object;
    do {
      node->next = retire_list_[epoch].load(std::memory_order_acquire);
    } while (!retire_list_.compare_exchange_weak(node->next, node, std::memory_order_acq_rel));

    // TODO 考虑是否聚合一定节点后再回收
    if (!update_.test_and_set(std::memory_order_acq_rel)) {
      TryGC();
      update_.clear(std::memory_order_release);
    }
  }

 private:
  static inline uint32_t GetThreadID() {
    thread_local ThreadID id;
    return id.tid;
  }

  inline void TryGC() {
    auto epoch = global_epoch_.load(std::memory_order_acquire);
    auto *tls_node = tls_list_.load(std::memory_order_acquire);
    while (tls_node != nullptr) {
      if (tls_node->tls->active.test() && tls_node->tls->epoch.load(std::memory_order_acquire) != epoch) {
        return;
      }
      tls_node = tls_node->next;
    }
    global_epoch_.store((epoch + 1) % kEpochSize, std::memory_order_release);
    ClearRetireList((epoch + 2) % kEpochSize);
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

  struct TLS {
    TLS() : active(false), epoch(0) {}
    TLS(TLS &) = delete;
    TLS(TLS &&) = delete;
    void operator=(const TLS &) = delete;
    std::atomic_flag active;
    std::atomic<uint8_t> epoch;
  } __attribute__((aligned(kCacheLineSize)));

  struct RetireNode {
    RCObject *obj;
    RetireNode *next;
  };
  std::array<TLS, thread_num> tls_list_;
  std::atomic<uint8_t> global_epoch_;
  char padding[kCacheLineSize - sizeof(std::atomic<uint8_t>)];  // TODO 待验证是否成功隔离
  std::atomic_flag update_;
  std::atomic<RetireNode *> retire_list_[kEpochSize];
  static std::atomic<uint32_t> thread_cnt_;
};

}  // namespace vrt