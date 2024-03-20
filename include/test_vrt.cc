#include <array>
#include <random>
#include <string>

#include "vrt.h"
#include "gtest/gtest.h"

TEST(NormalInsertFindTest, NormalTest) {
  std::array<std::string, 6> keys = {
      "abcdefg", "ab", "abcght", "abqert", "abcghq", "abcgh",
  };
  vrt::Vrt<std::string> vrt_tree;
  for (int i = 0; i < 6; i++) {
    EXPECT_EQ(vrt_tree.Insert(keys[i], nullptr, std::to_string(i)), true);
  }
  for (int i = 0; i < 6; i++) {
    std::string value;
    EXPECT_EQ(vrt_tree.Find(keys[i], &value), true);
    EXPECT_EQ(value, std::to_string(i));
  }
  for (int i = 0; i < 6; i++) {
    std::string value;
    EXPECT_EQ(vrt_tree.Insert(keys[i], &value, std::to_string(i)), false);
    EXPECT_EQ(value, std::to_string(i));
  }
}

TEST(NormalDeleteTest, NormalTest) {
  std::array<std::string, 6> keys = {
      "abcdefg", "ab", "abcght", "abqert", "abcghq", "abcgh",
  };
  vrt::Vrt<std::string> vrt_tree;
  for (int i = 0; i < 6; i++) {
    EXPECT_EQ(vrt_tree.Insert(keys[i], nullptr, std::to_string(i)), true);
  }
  for (int i = 0; i < 3; i++) {
    EXPECT_EQ(vrt_tree.Delete(keys[i]), true);
  }
  for (int i = 0; i < 3; i++) {
    std::string value;
    EXPECT_EQ(vrt_tree.Find(keys[i], &value), false);
  }

  for (int i = 3; i < 6; i++) {
    std::string value;
    EXPECT_EQ(vrt_tree.Find(keys[i], &value), true);
    EXPECT_EQ(value, std::to_string(i));
  }
}

TEST(ErrTest, ErrUpdateTest) {
  std::array<std::string, 6> keys = {
      "abcdefg", "ab", "abcght", "abqert", "abcghq", "abcgh",
  };
  vrt::Vrt<std::string> vrt_tree;
  for (int i = 0; i < 6; i++) {
    EXPECT_EQ(vrt_tree.Update(keys[i], std::to_string(i)), false);
  }
}

TEST(ErrTest, ErrDeleteTest) {
  std::array<std::string, 6> keys = {
      "abcdefg", "ab", "abcght", "abqert", "abcghq", "abcgh",
  };
  vrt::Vrt<std::string> vrt_tree;
  for (int i = 0; i < 6; i++) {
    EXPECT_EQ(vrt_tree.Delete(keys[i]), false);
  }
}

TEST(RandomTest, RandomTest) {
  // TODO优化，没有解决key冲突的情况，但这个范围一般来说不会冲突
  constexpr uint32_t kMaxKeyLength = 20;
  constexpr uint32_t kMaxKey = 1000000;
  std::vector<std::string> keys(kMaxKey * 2);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> distrib(-128, 127);
  vrt::Vrt<std::string> vrt_tree;
  for (int i = 0; i < kMaxKey; i++) {
    keys[i].resize(kMaxKeyLength);
    for (int j = 0; j < kMaxKeyLength; j++) {
      keys[i][j] = distrib(gen);
    }
    EXPECT_EQ(vrt_tree.Insert(keys[i], nullptr, std::to_string(i)), true);
  }
  for (int i = 0; i < kMaxKey; i++) {
    std::string value;
    EXPECT_EQ(vrt_tree.Find(keys[i], &value), true);
    EXPECT_EQ(value, std::to_string(i));
  }
  for (int i = 0; i < kMaxKey / 10; i++) {
    EXPECT_EQ(vrt_tree.Update(keys[i], std::to_string(i + kMaxKey * 10)), true);
  }
  for (int i = 0; i < kMaxKey / 10; i++) {
    std::string value;
    EXPECT_EQ(vrt_tree.Find(keys[i], &value), true);
    EXPECT_EQ(value, std::to_string(i + kMaxKey * 10));
  }
  for (int i = kMaxKey / 10; i < kMaxKey; i++) {
    std::string value;
    EXPECT_EQ(vrt_tree.Find(keys[i], &value), true);
    EXPECT_EQ(value, std::to_string(i));
  }
  for (int i = 0; i < kMaxKey; i++) {
    EXPECT_EQ(vrt_tree.Upsert(keys[i], std::to_string(i)), true);
  }

  for (int i = kMaxKey; i < kMaxKey * 2; i++) {
    keys[i].resize(kMaxKeyLength);
    for (int j = 0; j < kMaxKeyLength; j++) {
      keys[i][j] = distrib(gen);
    }
    EXPECT_EQ(vrt_tree.Upsert(keys[i], std::to_string(i)), true);
  }
  for (int i = 0; i < kMaxKey * 2; i++) {
    std::string value;
    EXPECT_EQ(vrt_tree.Find(keys[i], &value), true);
    EXPECT_EQ(value, std::to_string(i));
  }
  for (int i = 0; i < kMaxKey; i++) {
    EXPECT_EQ(vrt_tree.Delete(keys[i]), true);
  }
  for (int i = 0; i < kMaxKey; i++) {
    std::string value;
    EXPECT_EQ(vrt_tree.Find(keys[i], &value), false);
  }
  for (int i = kMaxKey; i < kMaxKey * 2; i++) {
    std::string value;
    EXPECT_EQ(vrt_tree.Find(keys[i], &value), true);
    EXPECT_EQ(value, std::to_string(i));
  }
}

GTEST_API_ int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}