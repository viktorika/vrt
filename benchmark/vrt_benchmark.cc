/*
 * @Author: viktorika 
 * @Date: 2024-04-05 19:01:54 
 * @Last Modified by:   viktorika 
 * @Last Modified time: 2024-04-05 19:01:54 
 */
#include "benchmark/benchmark.h"
#include <unistd.h>
#include <random>
#include <thread>
#include <utility>
#include "phmap.h"
#include "vrt.h"
#include "vrt_node.h"

uint32_t kKeySize = 1000000;
uint32_t kKeyLength = 20;
std::vector<std::string> keys(kKeySize);
constexpr uint32_t thread_num = 5;

static int GenKeys() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> distrib(32, 126);
  for (int i = 0; i < kKeySize; i++) {
    keys[i].resize(kKeyLength);
    for (int j = 0; j < kKeyLength; j++) {
      keys[i][j] = distrib(gen);
    }
  }
  return 0;
}

static void RunInsertDeConstructPhmapByMutex(benchmark::State& state) {
  for (auto _ : state) {
    phmap::parallel_flat_hash_map<std::string, std::string, phmap::priv::hash_default_hash<std::string>,
                                  phmap::priv::hash_default_eq<std::string>,
                                  std::allocator<std::pair<const std::string, std::string>>, 8, std::mutex>
        map;
    auto func = [&](int start, int end) {
      for (int i = start; i < end; i++) {
        map.try_emplace(keys[i], "123");
      }
    };
    std::vector<std::thread> ts(thread_num);
    auto batch = kKeySize / thread_num;
    for (int i = 0; i < thread_num; i++) {
      ts[i] = std::thread(func, i * batch, (i + 1) * batch);
    }
    for (int i = 0; i < thread_num; i++) {
      ts[i].join();
    }
  }
}

static void RunInsertDeConstructVrt(benchmark::State& state) {
  for (auto _ : state) {
    vrt::Vrt<std::string, true, thread_num> vrt;
    auto func = [&](int start, int end) {
      for (int i = start; i < end; i++) {
        vrt.Upsert(keys[i], "123");
      }
    };
    std::vector<std::thread> ts(thread_num);
    auto batch = kKeySize / thread_num;
    for (int i = 0; i < thread_num; i++) {
      ts[i] = std::thread(func, i * batch, (i + 1) * batch);
    }
    for (int i = 0; i < thread_num; i++) {
      ts[i].join();
    }
  }
}

static void RunFindPhmapByMutex(benchmark::State& state) {
  phmap::parallel_flat_hash_map<std::string, std::string, phmap::priv::hash_default_hash<std::string>,
                                phmap::priv::hash_default_eq<std::string>,
                                std::allocator<std::pair<const std::string, std::string>>, 8, std::mutex>
      map;
  for (int i = 0; i < kKeySize; i++) {
    map.try_emplace(keys[i], "123");
  }
  std::vector<std::string> values(kKeySize);
  for (auto _ : state) {
    auto func = [&](int start, int end) {
      for (int i = start; i < end; i++) {
        map.if_contains(keys[i],
                        [&values, i](const std::pair<std::string, std::string>& val) { values[i] = val.second; });
      }
    };
    std::vector<std::thread> ts(thread_num);
    auto batch = kKeySize / thread_num;
    for (int i = 0; i < thread_num; i++) {
      ts[i] = std::thread(func, i * batch, (i + 1) * batch);
    }
    for (int i = 0; i < thread_num; i++) {
      ts[i].join();
    }
  }
}

static void RunFindVrt(benchmark::State& state) {
  vrt::Vrt<std::string, true, thread_num> vrt;
  for (int i = 0; i < kKeySize; i++) {
    vrt.Upsert(keys[i], "123");
  }
  std::vector<std::string> values(kKeySize);
  for (auto _ : state) {
    auto func = [&](int start, int end) {
      for (int i = start; i < end; i++) {
        vrt.Find(keys[i], &values[i]);
      }
    };
    std::vector<std::thread> ts(thread_num);
    auto batch = kKeySize / thread_num;
    for (int i = 0; i < thread_num; i++) {
      ts[i] = std::thread(func, i * batch, (i + 1) * batch);
    }
    for (int i = 0; i < thread_num; i++) {
      ts[i].join();
    }
  }
}

static void RunDeletePhmapByMutex(benchmark::State& state) {
  for (auto _ : state) {
    phmap::parallel_flat_hash_map<std::string, std::string, phmap::priv::hash_default_hash<std::string>,
                                  phmap::priv::hash_default_eq<std::string>,
                                  std::allocator<std::pair<const std::string, std::string>>, 8, std::mutex>
        map;
    auto func = [&](int start, int end) {
      for (int i = start; i < end; i++) {
        map.try_emplace(keys[i], "123");
      }
      for (int i = start; i < end; i++) {
        map.erase(keys[i]);
      }
    };
    std::vector<std::thread> ts(thread_num);
    auto batch = kKeySize / thread_num;
    for (int i = 0; i < thread_num; i++) {
      ts[i] = std::thread(func, i * batch, (i + 1) * batch);
    }
    for (int i = 0; i < thread_num; i++) {
      ts[i].join();
    }
  }
}

static void RunDeleteVrt(benchmark::State& state) {
  for (auto _ : state) {
    vrt::Vrt<std::string, true, thread_num> vrt;
    auto func = [&](int start, int end) {
      for (int i = start; i < end; i++) {
        vrt.Upsert(keys[i], "123");
      }
      for (int i = start; i < end; i++) {
        vrt.Delete(keys[i]);
      }
    };
    std::vector<std::thread> ts(thread_num);
    auto batch = kKeySize / thread_num;
    for (int i = 0; i < thread_num; i++) {
      ts[i] = std::thread(func, i * batch, (i + 1) * batch);
    }
    for (int i = 0; i < thread_num; i++) {
      ts[i].join();
    }
  }
}

BENCHMARK(RunInsertDeConstructPhmapByMutex);
BENCHMARK(RunInsertDeConstructVrt);
BENCHMARK(RunFindPhmapByMutex);
BENCHMARK(RunFindVrt);
BENCHMARK(RunDeletePhmapByMutex);
BENCHMARK(RunDeleteVrt);

int main(int argc, char** argv) {
  GenKeys();
  ::benchmark ::Initialize(&argc, argv);
  if (::benchmark ::ReportUnrecognizedArguments(argc, argv)) return 1;
  ::benchmark ::RunSpecifiedBenchmarks();
  ::benchmark ::Shutdown();
#ifdef MEM_DEBUG
  std::cout << vrt::VrtNodeHelper<true>::GetCreateNodeCnt() << std::endl;
  std::cout << vrt::VrtNodeHelper<true>::GetDestroyNodeCnt() << std::endl;
  std::cout << vrt::VrtNodeHelper<false>::GetCreateNodeCnt() << std::endl;
  std::cout << vrt::VrtNodeHelper<false>::GetDestroyNodeCnt() << std::endl;
#endif
  return 0;
}