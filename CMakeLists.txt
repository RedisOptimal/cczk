cmake_minimum_required(VERSION 2.8)

project(xcs-cczk)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "DEBUG")
ENDIF()

SET(CXX_FLAGS
 -g
 -DVALGRIND
 -Wall
 -Wextra
 # -Werror
 -Woverloaded-virtual
 -Wpointer-arith
 -Wwrite-strings
 -Wno-unused-value
 -Wno-unused-variable
 -Wno-reorder
 -Wno-unused-parameter
 -Wno-unused-function
 -fPIC
 -D_FILE_OFFSET_BITS=64
 -DHAVE_STDINT_H
 -DHAVE_NETINET_IN_H
 -D__STDC_LIMIT_MACROS
 )

# 平台相关设定
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Mac OS 使用clang，获取更好的错误提示信息；不使用 clang 的运行库，避免程序行为的不一致
  LIST(APPEND CXX_FLAGS "-stdlib=libstdc++")
  SET(CMAKE_CXX_COMPILER "clang++")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux 打开全量符号表
  LIST(APPEND CXX_FLAGS "-rdynamic -fpermissive")
  SET(CMAKE_CXX_COMPILER "g++")
ENDIF()

IF(CMAKE_BUILD_BITS EQUAL 32)
  LIST(APPEND CXX_FLAGS "-m32")
ENDIF()

STRING(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -fprofile-arcs -ftest-coverage -DDEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")

# Must use google code style to address the include files
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

# Boost
FIND_PACKAGE(Boost REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

# add common dependences
#zookeeper
FIND_LIBRARY(ZOOKEEPER_LIBRARY NAMES zookeeper_mt
             PATHS lib/ NO_DEFAULT_PATH)
IF (NOT ZOOKEEPER_LIBRARY)
  FIND_LIBRARY(ZOOKEEPER_LIBRARY NAMES zookeeper_mt)
  IF (NOT ZOOKEEPER_LIBRARY)
    MESSAGE(FATAL_ERROR "zookeeper not found.")
  ENDIF(NOT ZOOKEEPER_LIBRARY)
ENDIF(NOT ZOOKEEPER_LIBRARY)
#glog
FIND_LIBRARY(GLOG_LIBRARY NAMES glog
             PATHS lib/ NO_DEFAULT_PATH)
IF (NOT GLOG_LIBRARY)
  FIND_LIBRARY(GLOG_LIBRARY NAMES glog)
  IF (NOT GLOG_LIBRARY)
    MESSAGE(FATAL_ERROR "glog not found.")
  ENDIF(NOT GLOG_LIBRARY)
ENDIF(NOT GLOG_LIBRARY)
#glog
FIND_LIBRARY(GFLAGS_LIBRARY NAMES gflags
             PATHS lib/ NO_DEFAULT_PATH)
IF (NOT GFLAGS_LIBRARY)
  FIND_LIBRARY(GFLAGS_LIBRARY NAMES gflags)
  IF (NOT GFLAGS_LIBRARY)
    MESSAGE(FATAL_ERROR "gflags not found.")
  ENDIF(NOT GFLAGS_LIBRARY)
ENDIF(NOT GFLAGS_LIBRARY)

FILE(GLOB CCZK_SRC xcs/cczk/*.cc )

ADD_LIBRARY(cczk_static STATIC ${CCZK_SRC})
ADD_LIBRARY(cczk_dynamic SHARED ${CCZK_SRC})

TARGET_LINK_LIBRARIES(cczk_static ${ZOOKEEPER_LIBRARY} 
  ${GLOG_LIBRARY} ${GFLAGS_LIBRARY} pthread boost_thread boost_system)
TARGET_LINK_LIBRARIES(cczk_dynamic ${ZOOKEEPER_LIBRARY}
  ${GLOG_LIBRARY} ${GFLAGS_LIBRARY} pthread boost_thread boost_system)

SET_TARGET_PROPERTIES(cczk_dynamic PROPERTIES VERSION 1.0 SOVERSION 1)
SET_TARGET_PROPERTIES(cczk_dynamic PROPERTIES OUTPUT_NAME cczk)
SET_TARGET_PROPERTIES(cczk_static PROPERTIES OUTPUT_NAME cczk)

FILE(GLOB XCS_SRC xcs/*.cc)

ADD_LIBRARY(xcs_static STATIC ${XCS_SRC})
ADD_LIBRARY(xcs_dynamic SHARED ${XCS_SRC})

TARGET_LINK_LIBRARIES(xcs_static cczk_static ${ZOOKEEPER_LIBRARY} 
  ${GLOG_LIBRARY} ${GFLAGS_LIBRARY} pthread boost_thread boost_system)
TARGET_LINK_LIBRARIES(xcs_dynamic cczk_dynamic ${ZOOKEEPER_LIBRARY}
  ${GLOG_LIBRARY} ${GFLAGS_LIBRARY} pthread boost_thread boost_system)

SET_TARGET_PROPERTIES(xcs_dynamic PROPERTIES VERSION 1.0 SOVERSION 1)
SET_TARGET_PROPERTIES(xcs_dynamic PROPERTIES OUTPUT_NAME xcs)
SET_TARGET_PROPERTIES(xcs_static PROPERTIES OUTPUT_NAME xcs)

##############################For test below###############################
IF (${CMAKE_BUILD_TYPE} MATCHES "DEBUG")
  FIND_PACKAGE(GTest REQUIRED)
  FIND_LIBRARY(GTEST_LIBRARY NAMES gtest
              PATHS lib/ NO_DEFAULT_PATH)
  IF (NOT GTEST_LIBRARY)
    FIND_LIBRARY(GTEST_LIBRARY NAMES gtest)
    IF (NOT GTEST_LIBRARY)
      MESSAGE(FATAL_ERROR "gtest not found.")
    ENDIF(NOT GTEST_LIBRARY)
  ENDIF(NOT GTEST_LIBRARY)

  INCLUDE(CTest)
  INCLUDE(Dart)
  ENABLE_TESTING()
  FILE(GLOB TESTS test/*.cc)
  ## Unit test start here
  SET(TestArgs "--log_dir=." "--zookeeper_log_name=zookeeper.log")
  ADD_EXECUTABLE(cczk_test ${TESTS})
  TARGET_LINK_LIBRARIES(cczk_test xcs_static cczk_static ${GTEST_LIBRARY})
  
  GTEST_ADD_TESTS(cczk_test "" ${TESTS})
  SET(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --quiet --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=100 --verbose --demangle=yes")

#  FOREACH (test ${TESTS})
#    STRING(REGEX REPLACE ".*/(.+).cc$" "\\1" fileName ${test} )
#    ADD_EXECUTABLE(${fileName} ${test})  
#    INSTALL(TARGETS ${fileName} RUNTIME DESTINATION bin)
#    TARGET_LINK_LIBRARIES(${fileName} ${ZOOKEEPER_LIBRARY}
#    ${GLOG_LIRBARY} ${GFLAGS_LIBRARY} ${GTEST_LIBRARY} pthread cczk_static boost_thread boost_system)
#  ENDFOREACH (test)
ENDIF (${CMAKE_BUILD_TYPE} MATCHES "DEBUG")

# install related
FILE(GLOB XCS_HEAD_LIST "${PROJECT_SOURCE_DIR}/xcs/*.h")
MESSAGE("xcs header : ${XCS_HEAD_LIST}")
INSTALL(FILES ${XCS_HEAD_LIST} DESTINATION include/xcs)

FILE(GLOB CCZK_HEAD_LIST "${PROJECT_SOURCE_DIR}/xcs/cczk/*.h")
MESSAGE("cczk header : ${CCZK_HEAD_LIST}")
INSTALL(FILES ${CCZK_HEAD_LIST} DESTINATION include/xcs/cczk)

INSTALL(TARGETS xcs_dynamic cczk_dynamic LIBRARY DESTINATION lib/)
INSTALL(TARGETS xcs_static cczk_static ARCHIVE DESTINATION lib/)
