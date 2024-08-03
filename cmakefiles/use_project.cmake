include_guard()
include(def)
include(base)


add_executable(${PROJECT_NAME})


if(NOT COMMON_CPP_DIR)
    set(COMMON_CPP_DIR ${CMAKE_CURRENT_LIST_DIR}/../source)
endif()
set(ALL_MY_FILE ${COMMON_CPP_DIR})

collectHeadFiles(MY_INC ALL_MY_FILE)
collectSrcFiles(MY_SRC ALL_MY_FILE)
collectAllSrcFiles(MY_INC_SRC ALL_MY_FILE)

if(NOT MAIN_CPP_DIR)
    set(MAIN_CPP_DIR ${CMAKE_CURRENT_LIST_DIR}/../main)
endif()
set(ALL_MY_FILE_MAIN ${MAIN_CPP_DIR})


collectHeadFiles(MY_INC_MAIN ALL_MY_FILE_MAIN)
collectSrcFiles(MY_INC_SRC_MAIN ALL_MY_FILE_MAIN)
collectAllSrcFiles(MY_INC_SRC_MAIN ALL_MY_FILE_MAIN)


#message("+++++++++++++++++++++++++++++MY_INC_SRC++++++++++++++++++++++++++++++++")
#message(${MY_INC_SRC})
#message("++++++++++++++++++++++++++++MY_INC_SRC_MAIN+++++++++++++++++++++++++++++++++")
#message(${MY_INC_SRC_MAIN})
#
#
#
#
#message("++++++++++++++++++++++++++++++MY_INC+++++++++++++++++++++++++++++++")
#message(${MY_INC})
#message("++++++++++++++++++++++++++++MY_INC_MAIN+++++++++++++++++++++++++++++++++")
#message(${MY_INC_MAIN})

if(LINUX)
else()
    source_group(${PROJECT_NAME} TREE ${COMMON_CPP_DIR} FILES ${MY_INC_SRC})
    source_group(${PROJECT_NAME} TREE ${MAIN_CPP_DIR} FILES ${MY_INC_SRC_MAIN})
endif()


target_sources(${PROJECT_NAME} PRIVATE
    #${MY_INC}
    #${MY_SRC}
    #${MY_INC_MAIN}
    ${MY_INC_SRC}
    ${MY_INC_SRC_MAIN}
)

target_include_directories(${PROJECT_NAME} PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/../3rd
${CMAKE_CURRENT_SOURCE_DIR}/../3rd/lua
${CMAKE_CURRENT_SOURCE_DIR}/../3rd/zlib
${CMAKE_CURRENT_SOURCE_DIR}/../3rd/asio
${CMAKE_CURRENT_SOURCE_DIR}/../3rd/protobuf
${CMAKE_CURRENT_SOURCE_DIR}/../3rd/spdlog/include
${CMAKE_CURRENT_SOURCE_DIR}/../3rd/openssl/include
${CMAKE_CURRENT_SOURCE_DIR}/../3rd/mimalloc/include
${CMAKE_CURRENT_SOURCE_DIR}/protobuf/cpp
${CMAKE_CURRENT_LIST_DIR}/../source
${MY_INC}
${MY_INC_MAIN}
)


#if(LINUX)
#    set(MY_LIBS
#    ${LIB_DIR}/libmimalloc.a
#    ${LIB_DIR}/openssl/libssl.a
#    ${LIB_DIR}/openssl/libcrypto.a
#    ${LIB_DIR}/$<CONFIG>/libzlib.a
#    ${LIB_DIR}/$<CONFIG>/liblua.a 
#    ${LIB_DIR}/$<CONFIG>/libspdlog.a
#    ${LIB_DIR}/libprotobuf.a
#    ${LIB_DIR}/boost/libboost_system.a
#    ${LIB_DIR}/boost/libboost_container.a
#    ${LIB_DIR}/boost/libboost_context.a
#    ${LIB_DIR}/boost/libboost_coroutine.a
#    ${LIB_DIR}/boost/libboost_date_time.a
#    ${LIB_DIR}/boost/libboost_exception.a
#    ${LIB_DIR}/boost/libboost_thread.a
#    ${LIB_DIR}/boost/libboost_regex.a
#    )
#else()
#    set(MY_LIBS
#    ${LIB_DIR}/mimalloc-static.lib
#    ${LIB_DIR}/openssl/libssl.lib
#    ${LIB_DIR}/openssl/libcrypto.lib
#    ${LIB_DIR}/$<CONFIG>/zlib.lib 
#    ${LIB_DIR}/$<CONFIG>/lua.lib 
#    ${LIB_DIR}/$<CONFIG>/spdlog.lib 
#    ${LIB_DIR}/$<CONFIG>/libprotobuf.lib 
#    ${LIB_DIR}/boost/libboost_system.lib
#    ${LIB_DIR}/boost/libboost_container.lib
#    ${LIB_DIR}/boost/libboost_context.lib
#    ${LIB_DIR}/boost/libboost_coroutine.lib
#    ${LIB_DIR}/boost/libboost_date_time.lib
#    ${LIB_DIR}/boost/libboost_exception.lib
#    ${LIB_DIR}/boost/libboost_thread.lib
#    ${LIB_DIR}/boost/libboost_regex.lib
#
#    )
#endif()
#message(${MY_LIBS})


# 设置库名称
set(LIB_NAMES "libmimalloc" "libprotobuf" "zlib" "lua" "spdlog")
set(LIB_OPENSSL "libssl" "libcrypto")
set(LIB_BOOSTS "libboost_system" "libboost_container" "libboost_context" "libboost_coroutine" "libboost_date_time" "libboost_exception" "libboost_thread" "libboost_regex")
# 根据平台添加后缀
if(LINUX)
    appendSuffixInList(LIB_NAME_USE "${LIB_NAMES}" ".a")
	appendSuffixInList(LIB_OPENSSL_USE "${LIB_OPENSSL}" ".a")
	appendSuffixInList(LIB_BOOST_USE "${LIB_BOOSTS}" ".a")
else()
    appendSuffixInList(LIB_NAME_USE "${LIB_NAMES}" ".lib")
	appendSuffixInList(LIB_OPENSSL_USE "${LIB_OPENSSL}" ".lib")
	appendSuffixInList(LIB_BOOST_USE "${LIB_BOOSTS}" ".lib")
endif()

# 打印添加后缀后的库名称列表
foreach(item ${LIB_NAME_USE})
    message(STATUS "List item: ${item}")
endforeach()

# 查找多个库
find_multiple_libraries(BASE_LIB ${LIB_DIR} "${LIB_NAME_USE}")
find_multiple_libraries(OPENSSL_LIB ${LIB_DIR}/openssl "${LIB_OPENSSL_USE}")
find_multiple_libraries(BOOST_LIB ${LIB_DIR}/boost "${LIB_BOOST_USE}")

# 打印查找结果
message("++++++++++++++++++++++++++++BASE_LIB+++++++++++++++++++++++++++++++++")
foreach(lib_path ${BASE_LIB})
    message(STATUS "${lib_path}")
endforeach()


message("++++++++++++++++++++++++++++OPENSSL_LIB+++++++++++++++++++++++++++++++++")
foreach(lib_path ${OPENSSL_LIB})
    message(STATUS "${lib_path}")
endforeach()

message("++++++++++++++++++++++++++++BOOST_LIB+++++++++++++++++++++++++++++++++")
foreach(lib_path ${BOOST_LIB})
    message(STATUS "${lib_path}")
endforeach()


if(LINUX)
set_target_properties(${PROJECT_NAME}  PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${EXE_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${EXE_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${EXE_DIR}
    LINK_FLAGS "-Wl,--whole-archive ${LIB_DIR}/libmimalloc.a -Wl,--no-whole-archive"
)

else()
set_target_properties(${PROJECT_NAME}  PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${EXE_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${EXE_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${EXE_DIR}
    LINK_FLAGS "/WHOLEARCHIVE:${LIB_DIR}/mimalloc-static.lib"
)
endif()


target_link_libraries(${PROJECT_NAME} PRIVATE 
#${MY_LIBS}
${BASE_LIB}
${OPENSSL_LIB}
${BOOST_LIB}
)






#find_package(Boost REQUIRED COMPONENTS headers)
#if(Boost_FOUND)
#    target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
#    MESSAGE( STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}.")
#    MESSAGE( STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}.")
#    MESSAGE( STATUS "Boost_LIB_VERSION = ${Boost_LIB_VERSION}.")
#    target_link_libraries (${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})
#endif()
#
#find_package(Threads REQUIRED)
#target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

#target_link_directories(${PROJECT_NAME} PRIVATE ${LIB_DIR})


#target_compile_options(${PROJECT_NAME} PUBLIC -DASIO_STANDALONE)
#target_compile_options(${PROJECT_NAME} PRIVATE -DASIO_STANDALONE)