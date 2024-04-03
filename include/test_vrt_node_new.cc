#include "gtest/gtest.h"
#include "vrt_node_new.h"

using namespace vrt;

TEST(NodeCreateTest, Node4CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node4, std::string>(key, "456");
  EXPECT_EQ("456", VrtNodeHelper<true>::GetValue<std::string>(node));
  EXPECT_EQ(key, VrtNodeHelper<true>::GetKey(node));
  EXPECT_EQ(node->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node4>(key);
  EXPECT_EQ(key, VrtNodeHelper<true>::GetKey(node));
  EXPECT_EQ(node->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
}

TEST(NodeCreateTest, Node16CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node16, std::string>(key, "456");
  EXPECT_EQ("456", VrtNodeHelper<true>::GetValue<std::string>(node));
  EXPECT_EQ(key, VrtNodeHelper<true>::GetKey(node));
  EXPECT_EQ(node->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node16>(key);
  EXPECT_EQ(key, VrtNodeHelper<true>::GetKey(node));
  EXPECT_EQ(node->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
}

TEST(NodeCreateTest, Node48CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node48, std::string>(key, "456");
  EXPECT_EQ("456", VrtNodeHelper<true>::GetValue<std::string>(node));
  EXPECT_EQ(key, VrtNodeHelper<true>::GetKey(node));
  EXPECT_EQ(node->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node48>(key);
  EXPECT_EQ(key, VrtNodeHelper<true>::GetKey(node));
  EXPECT_EQ(node->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
}

TEST(NodeCreateTest, Node256CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node256, std::string>(key, "456");
  EXPECT_EQ("456", VrtNodeHelper<true>::GetValue<std::string>(node));
  EXPECT_EQ(key, VrtNodeHelper<true>::GetKey(node));
  EXPECT_EQ(node->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node256>(key);
  EXPECT_EQ(key, VrtNodeHelper<true>::GetKey(node));
  EXPECT_EQ(node->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
}

TEST(NodeCreateTest, LeafNodeCreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<LeafNode, std::string>(key, "456");
  EXPECT_EQ("456", VrtNodeHelper<true>::GetValue<std::string>(node));
  EXPECT_EQ(key, VrtNodeHelper<true>::GetKey(node));
  EXPECT_EQ(node->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<LeafNode>(key);
  EXPECT_EQ(key, VrtNodeHelper<true>::GetKey(node));
  EXPECT_EQ(node->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
}

TEST(NodeCreateRemovePrefixTest, Node4CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node4, std::string>(key, "456");
  auto *after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ("456", VrtNodeHelper<true>::GetValue<std::string>(after_remove_node));
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(after_remove_node->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_remove_node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node4>(key);
  after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(after_remove_node->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_remove_node);
}

TEST(NodeCreateRemovePrefixTest, Node16CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node16, std::string>(key, "456");
  auto *after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ("456", VrtNodeHelper<true>::GetValue<std::string>(after_remove_node));
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(after_remove_node->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_remove_node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node16>(key);
  after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(after_remove_node->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_remove_node);
}

TEST(NodeCreateRemovePrefixTest, Node48CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node48, std::string>(key, "456");
  auto *after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ("456", VrtNodeHelper<true>::GetValue<std::string>(after_remove_node));
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(after_remove_node->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_remove_node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node48>(key);
  after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(after_remove_node->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_remove_node);
}

TEST(NodeCreateRemovePrefixTest, Node256CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node256, std::string>(key, "456");
  auto *after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ("456", VrtNodeHelper<true>::GetValue<std::string>(after_remove_node));
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(after_remove_node->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_remove_node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node256>(key);
  after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(after_remove_node->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_remove_node);
}

TEST(NodeCreateRemovePrefixTest, LeafNodeCreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<LeafNode, std::string>(key, "456");
  auto *after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ("456", VrtNodeHelper<true>::GetValue<std::string>(after_remove_node));
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(after_remove_node->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_remove_node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<LeafNode>(key);
  after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(after_remove_node->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_remove_node);
}

TEST(NodeCreateAddValueTest, Node4CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node4>(key);
  auto *after_add_value = VrtNodeHelper<true>::CreateVrtNodeByAddValue<std::string>(node, "456");
  EXPECT_EQ("456", VrtNodeHelper<true>::GetValue<std::string>(after_add_value));
  EXPECT_EQ("123", VrtNodeHelper<true>::GetKey(after_add_value));
  EXPECT_EQ(after_add_value->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_add_value);
}

TEST(NodeCreateAddValueTest, Node16CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node16>(key);
  auto *after_add_value = VrtNodeHelper<true>::CreateVrtNodeByAddValue<std::string>(node, "456");
  EXPECT_EQ("456", VrtNodeHelper<true>::GetValue<std::string>(after_add_value));
  EXPECT_EQ("123", VrtNodeHelper<true>::GetKey(after_add_value));
  EXPECT_EQ(after_add_value->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_add_value);
}

TEST(NodeCreateAddValueTest, Node48CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node48>(key);
  auto *after_add_value = VrtNodeHelper<true>::CreateVrtNodeByAddValue<std::string>(node, "456");
  EXPECT_EQ("456", VrtNodeHelper<true>::GetValue<std::string>(after_add_value));
  EXPECT_EQ("123", VrtNodeHelper<true>::GetKey(after_add_value));
  EXPECT_EQ(after_add_value->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_add_value);
}

TEST(NodeCreateAddValueTest, Node256CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node48>(key);
  auto *after_add_value = VrtNodeHelper<true>::CreateVrtNodeByAddValue<std::string>(node, "456");
  EXPECT_EQ("456", VrtNodeHelper<true>::GetValue<std::string>(after_add_value));
  EXPECT_EQ("123", VrtNodeHelper<true>::GetKey(after_add_value));
  EXPECT_EQ(after_add_value->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_add_value);
}

TEST(NodeCreateAddValueTest, LeafNodeCreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<LeafNode>(key);
  auto *after_add_value = VrtNodeHelper<true>::CreateVrtNodeByAddValue<std::string>(node, "456");
  EXPECT_EQ("456", VrtNodeHelper<true>::GetValue<std::string>(after_add_value));
  EXPECT_EQ("123", VrtNodeHelper<true>::GetKey(after_add_value));
  EXPECT_EQ(after_add_value->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_add_value);
}

TEST(NodeCreateDeleteValueTest, Node4CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node4, std::string>(key, "456");
  auto *after_delete_value = VrtNodeHelper<true>::CreateVrtNodeByDeleteValue<std::string>(node);
  EXPECT_EQ("123", VrtNodeHelper<true>::GetKey(after_delete_value));
  EXPECT_EQ(after_delete_value->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_delete_value);
}

TEST(NodeCreateDeleteValueTest, Node16CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node16, std::string>(key, "456");
  auto *after_delete_value = VrtNodeHelper<true>::CreateVrtNodeByDeleteValue<std::string>(node);
  EXPECT_EQ("123", VrtNodeHelper<true>::GetKey(after_delete_value));
  EXPECT_EQ(after_delete_value->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_delete_value);
}

TEST(NodeCreateDeleteValueTest, Node48CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node48, std::string>(key, "456");
  auto *after_delete_value = VrtNodeHelper<true>::CreateVrtNodeByDeleteValue<std::string>(node);
  EXPECT_EQ("123", VrtNodeHelper<true>::GetKey(after_delete_value));
  EXPECT_EQ(after_delete_value->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_delete_value);
}

TEST(NodeCreateDeleteValueTest, Node256CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node256, std::string>(key, "456");
  auto *after_delete_value = VrtNodeHelper<true>::CreateVrtNodeByDeleteValue<std::string>(node);
  EXPECT_EQ("123", VrtNodeHelper<true>::GetKey(after_delete_value));
  EXPECT_EQ(after_delete_value->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_delete_value);
}

TEST(NodeCreateDeleteValueTest, LeafNodeCreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<LeafNode, std::string>(key, "456");
  auto *after_delete_value = VrtNodeHelper<true>::CreateVrtNodeByDeleteValue<std::string>(node);
  EXPECT_EQ("123", VrtNodeHelper<true>::GetKey(after_delete_value));
  EXPECT_EQ(after_delete_value->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(after_delete_value);
}

TEST(CheckSamePrefixLengthTest, Node4CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node4, std::string>(key, "456");
  EXPECT_EQ(2, VrtNodeHelper<true>::CheckSamePrefixLength(node, "12"));
  VrtNodeHelper<true>::DestroyNode<std::string>(node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node4>(key);
  EXPECT_EQ(3, VrtNodeHelper<true>::CheckSamePrefixLength(node, "1234"));
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
}

TEST(CheckSamePrefixLengthTest, Node16CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node16, std::string>(key, "456");
  EXPECT_EQ(2, VrtNodeHelper<true>::CheckSamePrefixLength(node, "12"));
  VrtNodeHelper<true>::DestroyNode<std::string>(node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node16>(key);
  EXPECT_EQ(3, VrtNodeHelper<true>::CheckSamePrefixLength(node, "1234"));
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
}

TEST(CheckSamePrefixLengthTest, Node48CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node48, std::string>(key, "456");
  EXPECT_EQ(2, VrtNodeHelper<true>::CheckSamePrefixLength(node, "12"));
  VrtNodeHelper<true>::DestroyNode<std::string>(node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node48>(key);
  EXPECT_EQ(3, VrtNodeHelper<true>::CheckSamePrefixLength(node, "1234"));
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
}

TEST(CheckSamePrefixLengthTest, Node256CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node256, std::string>(key, "456");
  EXPECT_EQ(2, VrtNodeHelper<true>::CheckSamePrefixLength(node, "12"));
  VrtNodeHelper<true>::DestroyNode<std::string>(node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node256>(key);
  EXPECT_EQ(3, VrtNodeHelper<true>::CheckSamePrefixLength(node, "1234"));
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
}

TEST(CheckSamePrefixLengthTest, LeafNodeCreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<LeafNode, std::string>(key, "456");
  EXPECT_EQ(2, VrtNodeHelper<true>::CheckSamePrefixLength(node, "12"));
  VrtNodeHelper<true>::DestroyNode<std::string>(node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<LeafNode>(key);
  EXPECT_EQ(3, VrtNodeHelper<true>::CheckSamePrefixLength(node, "1234"));
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
}

GTEST_API_ int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
