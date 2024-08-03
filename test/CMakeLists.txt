cmake_minimum_required(VERSION 3.16)
project(test CXX)

message("==================== ${PROJECT_NAME} config begin ====================")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmakefiles")
include(def)
include(base)
include(use_test)

