// Copyright 2018 Apex.AI, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <gtest/gtest.h>

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rcl_yaml_param_parser/parser.h"

#include "rcutils/allocator.h"
#include "rcutils/error_handling.h"
#include "rcutils/filesystem.h"

static char cur_dir[1024];

TEST(test_parser, correct_syntax) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "correct_config.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  ASSERT_TRUE(res) << rcutils_get_error_string().str;

  rcl_variant_t * param_value = rcl_yaml_node_struct_get("lidar_ns/lidar_1", "ports", params_hdl);
  ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
  ASSERT_TRUE(NULL != param_value->integer_array_value);
  ASSERT_EQ(3U, param_value->integer_array_value->size);
  EXPECT_EQ(2438, param_value->integer_array_value->values[0]);
  EXPECT_EQ(2439, param_value->integer_array_value->values[1]);
  EXPECT_EQ(2440, param_value->integer_array_value->values[2]);
  res = rcl_parse_yaml_value("lidar_ns/lidar_1", "ports", "[8080]", params_hdl);
  EXPECT_TRUE(res) << rcutils_get_error_string().str;
  ASSERT_TRUE(NULL != param_value->integer_array_value);
  ASSERT_EQ(1U, param_value->integer_array_value->size);
  EXPECT_EQ(8080, param_value->integer_array_value->values[0]);

  param_value = rcl_yaml_node_struct_get("lidar_ns/lidar_2", "is_back", params_hdl);
  ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
  ASSERT_TRUE(NULL != param_value->bool_value);
  EXPECT_FALSE(*param_value->bool_value);
  res = rcl_parse_yaml_value("lidar_ns/lidar_2", "is_back", "true", params_hdl);
  EXPECT_TRUE(res) << rcutils_get_error_string().str;
  ASSERT_TRUE(NULL != param_value->bool_value);
  EXPECT_TRUE(*param_value->bool_value);

  param_value = rcl_yaml_node_struct_get("camera", "cam_spec.angle", params_hdl);
  ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
  ASSERT_TRUE(NULL != param_value->double_value);
  EXPECT_DOUBLE_EQ(2.34, *param_value->double_value);
  res = rcl_parse_yaml_value("camera", "cam_spec.angle", "2.2", params_hdl);
  EXPECT_TRUE(res) << rcutils_get_error_string().str;
  ASSERT_TRUE(NULL != param_value->double_value);
  EXPECT_DOUBLE_EQ(2.2, *param_value->double_value);

  param_value = rcl_yaml_node_struct_get("intel", "arch", params_hdl);
  ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
  ASSERT_TRUE(NULL != param_value->string_value);
  EXPECT_STREQ("x86_64", param_value->string_value);
  res = rcl_parse_yaml_value("intel", "arch", "x86", params_hdl);
  EXPECT_TRUE(res) << rcutils_get_error_string().str;
  ASSERT_TRUE(NULL != param_value->string_value);
  EXPECT_STREQ("x86", param_value->string_value);

  rcl_yaml_node_struct_print(params_hdl);
}

TEST(test_file_parser, string_array_with_quoted_number) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "string_array_with_quoted_number.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  ASSERT_TRUE(res) << rcutils_get_error_string().str;
  rcl_variant_t * param_value = rcl_yaml_node_struct_get(
    "initial_params_node", "sa2", params_hdl);
  ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
  ASSERT_TRUE(NULL != param_value->string_array_value);
  ASSERT_EQ(2U, param_value->string_array_value->size);
  EXPECT_STREQ("and", param_value->string_array_value->data[0]);
  EXPECT_STREQ("7", param_value->string_array_value->data[1]);
  res = rcl_parse_yaml_value("initial_params_node", "category", "'0'", params_hdl);
  EXPECT_TRUE(res) << rcutils_get_error_string().str;
  param_value = rcl_yaml_node_struct_get("initial_params_node", "category", params_hdl);
  ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
  ASSERT_TRUE(NULL != param_value->string_value);
  EXPECT_STREQ("0", param_value->string_value);
  rcl_yaml_node_struct_print(params_hdl);
}

TEST(test_file_parser, multi_ns_correct_syntax) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "multi_ns_correct.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_TRUE(res) << rcutils_get_error_string().str;
  rcl_yaml_node_struct_print(params_hdl);
}

TEST(test_file_parser, root_ns) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "root_ns.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_TRUE(res) << rcutils_get_error_string().str;
  rcl_yaml_node_struct_print(params_hdl);
  // Check that there is only one forward slash in the node's FQN.
  // (Regression test for https://github.com/ros2/rcl/pull/299).
  EXPECT_EQ(1u, params_hdl->num_nodes);
  EXPECT_STREQ("/my_node", params_hdl->node_names[0]);
}

TEST(test_file_parser, seq_map1) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "seq_map1.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  bool res = rcl_parse_yaml_file(path, params_hdl);
  fprintf(stderr, "%s\n", rcutils_get_error_string().str);
  EXPECT_FALSE(res);
}

TEST(test_file_parser, seq_map2) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "seq_map2.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  bool res = rcl_parse_yaml_file(path, params_hdl);
  fprintf(stderr, "%s\n", rcutils_get_error_string().str);
  EXPECT_FALSE(res);
}

TEST(test_file_parser, params_with_no_node) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "params_with_no_node.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  bool res = rcl_parse_yaml_file(path, params_hdl);
  fprintf(stderr, "%s\n", rcutils_get_error_string().str);
  EXPECT_FALSE(res);
}

TEST(test_file_parser, no_alias_support) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "no_alias_support.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  bool res = rcl_parse_yaml_file(path, params_hdl);
  fprintf(stderr, "%s\n", rcutils_get_error_string().str);
  EXPECT_FALSE(res);
}

TEST(test_file_parser, max_string_sz) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "max_string_sz.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  bool res = rcl_parse_yaml_file(path, params_hdl);
  fprintf(stderr, "%s\n", rcutils_get_error_string().str);
  EXPECT_FALSE(res);
}

TEST(test_file_parser, empty_string) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "empty_string.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_TRUE(res) << rcutils_get_error_string().str;
  rcl_yaml_node_struct_print(params_hdl);
}

TEST(test_file_parser, no_value1) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "no_value1.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  bool res = rcl_parse_yaml_file(path, params_hdl);
  fprintf(stderr, "%s\n", rcutils_get_error_string().str);
  EXPECT_FALSE(res);
}

TEST(test_file_parser, indented_ns) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "indented_name_space.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  bool res = rcl_parse_yaml_file(path, params_hdl);
  fprintf(stderr, "%s\n", rcutils_get_error_string().str);
  EXPECT_FALSE(res);
}

// Regression test for https://github.com/ros2/rcl/issues/419
TEST(test_file_parser, maximum_number_parameters) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "max_num_params.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  bool res = rcl_parse_yaml_file(path, params_hdl);
  fprintf(stderr, "%s\n", rcutils_get_error_string().str);
  EXPECT_FALSE(res);
}

int32_t main(int32_t argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
