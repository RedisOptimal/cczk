cmake_minimum_required(VERSION 2.8)

project(cczk)

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
 -rdynamic
 -fPIC
 -D_FILE_OFFSET_BITS=64
 -DHAVE_STDINT_H
 -DHAVE_NETINET_IN_H
 -D__STDC_LIMIT_MACROS
 )

IF(CMAKE_BUILD_BITS EQUAL 32)
  LIST(APPEND CXX_FLAGS "-m32")
ENDIF()

STRING(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

SET(CMAKE_CXX_COMPILER "g++")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -fprofile-arcs -ftest-coverage")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")

INCLUDE_DIRECTORIES(include/)
INCLUDE_DIRECTORIES(src/)

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

FILE(GLOB SRC src/*.cc )

ADD_LIBRARY(cczk_static STATIC ${SRC})
ADD_LIBRARY(cczk_dynamic SHARED ${SRC})

TARGET_LINK_LIBRARIES(cczk_static ${ZOOKEEPER_LIBRARY} 
  ${GLOG_LIBRARY} ${GFLAGS_LIBRARY} pthread boost_thread boost_system)
TARGET_LINK_LIBRARIES(cczk_dynamic ${ZOOKEEPER_LIBRARY}
  ${GLOG_LIBRARY} ${GFLAGS_LIBRARY} pthread boost_thread boost_system)

SET_TARGET_PROPERTIES(cczk_dynamic PROPERTIES VERSION 1.0 SOVERSION 1)
SET_TARGET_PROPERTIES(cczk_dynamic PROPERTIES OUTPUT_NAME cczk)
SET_TARGET_PROPERTIES(cczk_static PROPERTIES OUTPUT_NAME cczk)


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
  TARGET_LINK_LIBRARIES(cczk_test cczk_static)
  
  GTEST_ADD_TESTS(cczk_test "${TestArgs}" ${TESTS})
  SET(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --quiet --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=100 --verbose --demangle=yes")

#  FOREACH (test ${TESTS})
#    STRING(REGEX REPLACE ".*/(.+).cc$" "\\1" fileName ${test} )
#    ADD_EXECUTABLE(${fileName} ${test})  
#    INSTALL(TARGETS ${fileName} RUNTIME DESTINATION bin)
#    TARGET_LINK_LIBRARIES(${fileName} ${ZOOKEEPER_LIBRARY}
#    ${GLOG_LIRBARY} ${GFLAGS_LIBRARY} ${GTEST_LIBRARY} pthread cczk_static boost_thread boost_system)
#  ENDFOREACH (test)
ENDIF (${CMAKE_BUILD_TYPE} MATCHES "DEBUG")
