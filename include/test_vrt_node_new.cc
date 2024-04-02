#include "gtest/gtest.h"
#include "vrt_node_new.h"

using namespace vrt;

TEST(NodeCreateTest, Node4CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node4, std::string>(key, "456");
  EXPECT_EQ(std::string("456"), VrtNodeHelper<true>::GetValue<std::string>(node));
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
  EXPECT_EQ(std::string("456"), VrtNodeHelper<true>::GetValue<std::string>(node));
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
  EXPECT_EQ(std::string("456"), VrtNodeHelper<true>::GetValue<std::string>(node));
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
  EXPECT_EQ(std::string("456"), VrtNodeHelper<true>::GetValue<std::string>(node));
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
  EXPECT_EQ(std::string("456"), VrtNodeHelper<true>::GetValue<std::string>(node));
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
  EXPECT_EQ(std::string("456"), VrtNodeHelper<true>::GetValue<std::string>(after_remove_node));
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(node->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node4>(key);
  after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(node->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
}

TEST(NodeCreateRemovePrefixTest, Node16CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node16, std::string>(key, "456");
  auto *after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ(std::string("456"), VrtNodeHelper<true>::GetValue<std::string>(after_remove_node));
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(node->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node16>(key);
  after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(node->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
}

TEST(NodeCreateRemovePrefixTest, Node48CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node48, std::string>(key, "456");
  auto *after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ(std::string("456"), VrtNodeHelper<true>::GetValue<std::string>(after_remove_node));
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(node->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node48>(key);
  after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(node->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
}

TEST(NodeCreateRemovePrefixTest, Node256CreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node256, std::string>(key, "456");
  auto *after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ(std::string("456"), VrtNodeHelper<true>::GetValue<std::string>(after_remove_node));
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(node->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<Node256>(key);
  after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(node->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
}

TEST(NodeCreateRemovePrefixTest, LeafNodeCreateTest) {
  std::string key("123");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<LeafNode, std::string>(key, "456");
  auto *after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ(std::string("456"), VrtNodeHelper<true>::GetValue<std::string>(after_remove_node));
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(node->has_value, 1);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);

  node = VrtNodeHelper<true>::CreateVrtNodeWithoutValue<LeafNode>(key);
  after_remove_node = VrtNodeHelper<true>::CreateVrtNodeByRemovePrefix<std::string>(node, 2);
  EXPECT_EQ("3", VrtNodeHelper<true>::GetKey(after_remove_node));
  EXPECT_EQ(node->has_value, 0);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
}

GTEST_API_ int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
