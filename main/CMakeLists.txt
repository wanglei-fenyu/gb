cmake_minimum_required(VERSION 3.16)

project(Base CXX)
message("==================== ${PROJECT_NAME} config begin ====================")


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmakefiles")
include(def)
include(base)
include(use_project)






# 设置vs默认启动项
#set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT cmake_exec)
