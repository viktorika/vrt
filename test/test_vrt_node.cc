/*
 * @Author: viktorika 
 * @Date: 2024-04-05 19:02:05 
 * @Last Modified by:   viktorika 
 * @Last Modified time: 2024-04-05 19:02:05 
 */
#include "gtest/gtest.h"
#include "vrt_node.h"

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

TEST(AddChildTest, Node4Test) {
  std::string key("1234567");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node4, std::string>(key, "456");
  auto *old_node = node;

  auto child_cnt = 5;
  VrtNode<true> *childs[child_cnt];
  for (int i = 0; i < child_cnt / 2; i++) {
    childs[i] = VrtNodeHelper<true>::CreateVrtNode<Node4, std::string>(key, "456");
    node = VrtNodeHelper<true>::AddChild<std::string>(node, i, childs[i]);
  }
  for (int i = 0; i < child_cnt / 2; i++) {
    EXPECT_EQ(childs[i], VrtNodeHelper<true>::FindChild(node, i));
  }
  EXPECT_EQ(nullptr, VrtNodeHelper<true>::FindChild(node, child_cnt / 2));
  for (int i = child_cnt / 2; i < child_cnt; i++) {
    childs[i] = VrtNodeHelper<true>::CreateVrtNode<Node4, std::string>(key, "456");
    node = VrtNodeHelper<true>::AddChild<std::string>(node, i, childs[i]);
  }
  for (int i = child_cnt / 2; i < child_cnt; i++) {
    EXPECT_EQ(childs[i], VrtNodeHelper<true>::FindChild(node, i));
  }
  EXPECT_EQ(nullptr, VrtNodeHelper<true>::FindChild(node, child_cnt));
  EXPECT_NE(node, old_node);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(old_node);
  for (auto *child : childs) {
    VrtNodeHelper<true>::DestroyNode<std::string>(child);
  }
}

TEST(AddChildTest, Node16Test) {
  std::string key("1234567");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node16, std::string>(key, "456");
  auto *old_node = node;

  auto child_cnt = 17;
  VrtNode<true> *childs[child_cnt];
  for (int i = 0; i < child_cnt / 2; i++) {
    childs[i] = VrtNodeHelper<true>::CreateVrtNode<Node4, std::string>(key, "456");
    node = VrtNodeHelper<true>::AddChild<std::string>(node, i, childs[i]);
  }
  for (int i = 0; i < child_cnt / 2; i++) {
    EXPECT_EQ(childs[i], VrtNodeHelper<true>::FindChild(node, i));
  }
  EXPECT_EQ(nullptr, VrtNodeHelper<true>::FindChild(node, child_cnt / 2));
  for (int i = child_cnt / 2; i < child_cnt; i++) {
    childs[i] = VrtNodeHelper<true>::CreateVrtNode<Node4, std::string>(key, "456");
    node = VrtNodeHelper<true>::AddChild<std::string>(node, i, childs[i]);
  }
  for (int i = child_cnt / 2; i < child_cnt; i++) {
    EXPECT_EQ(childs[i], VrtNodeHelper<true>::FindChild(node, i));
  }
  EXPECT_EQ(nullptr, VrtNodeHelper<true>::FindChild(node, child_cnt));
  EXPECT_NE(node, old_node);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(old_node);
  for (auto *child : childs) {
    VrtNodeHelper<true>::DestroyNode<std::string>(child);
  }
}

TEST(AddChildTest, Node48Test) {
  std::string key("1234567");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node48, std::string>(key, "456");
  auto *old_node = node;

  auto child_cnt = 49;
  VrtNode<true> *childs[child_cnt];
  for (int i = 0; i < child_cnt / 2; i++) {
    childs[i] = VrtNodeHelper<true>::CreateVrtNode<Node4, std::string>(key, "456");
    node = VrtNodeHelper<true>::AddChild<std::string>(node, i, childs[i]);
  }
  for (int i = 0; i < child_cnt / 2; i++) {
    EXPECT_EQ(childs[i], VrtNodeHelper<true>::FindChild(node, i));
  }
  EXPECT_EQ(nullptr, VrtNodeHelper<true>::FindChild(node, child_cnt / 2));
  for (int i = child_cnt / 2; i < child_cnt; i++) {
    childs[i] = VrtNodeHelper<true>::CreateVrtNode<Node4, std::string>(key, "456");
    node = VrtNodeHelper<true>::AddChild<std::string>(node, i, childs[i]);
  }
  for (int i = child_cnt / 2; i < child_cnt; i++) {
    EXPECT_EQ(childs[i], VrtNodeHelper<true>::FindChild(node, i));
  }
  EXPECT_EQ(nullptr, VrtNodeHelper<true>::FindChild(node, child_cnt));
  EXPECT_NE(node, old_node);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(old_node);
  for (auto *child : childs) {
    VrtNodeHelper<true>::DestroyNode<std::string>(child);
  }
}

TEST(AddChildTest, Node256Test) {
  std::string key("1234567");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<Node256, std::string>(key, "456");
  auto *old_node = node;

  auto child_cnt = 256;
  VrtNode<true> *childs[child_cnt];
  for (int i = 0; i < child_cnt / 2; i++) {
    childs[i] = VrtNodeHelper<true>::CreateVrtNode<Node4, std::string>(key, "456");
    node = VrtNodeHelper<true>::AddChild<std::string>(node, i, childs[i]);
  }
  for (int i = 0; i < child_cnt / 2; i++) {
    EXPECT_EQ(childs[i], VrtNodeHelper<true>::FindChild(node, i));
  }
  EXPECT_EQ(nullptr, VrtNodeHelper<true>::FindChild(node, child_cnt / 2));
  EXPECT_EQ(node, old_node);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  for (int i = 0; i < child_cnt / 2; i++) {
    VrtNodeHelper<true>::DestroyNode<std::string>(childs[i]);
  }
}

TEST(AddChildTest, LeafNodeTest) {
  std::string key("1234567");
  auto *node = VrtNodeHelper<true>::CreateVrtNode<LeafNode, std::string>(key, "456");
  auto *old_node = node;
  auto *child = VrtNodeHelper<true>::CreateVrtNode<Node4, std::string>(key, "456");
  node = VrtNodeHelper<true>::AddChild<std::string>(node, 0, child);
  EXPECT_EQ(child, VrtNodeHelper<true>::FindChild(node, 0));
  EXPECT_EQ(nullptr, VrtNodeHelper<true>::FindChild(node, 1));
  EXPECT_NE(node, old_node);
  VrtNodeHelper<true>::DestroyNode<std::string>(node);
  VrtNodeHelper<true>::DestroyNode<std::string>(old_node);
  VrtNodeHelper<true>::DestroyNode<std::string>(child);
}

GTEST_API_ int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
