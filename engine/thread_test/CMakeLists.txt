#CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#PROJECT(myThreadTest)

#SET(CMAKE_BUILE_TYPE Debug)
#SET(COMP_FLAGS "-ggdb -Wall")

FIND_LIBRARY(is_exit pthread )

message(STATUS "is_exit = ${is_exit}")

message(STATUS "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
message(STATUS "PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}")
message(STATUS "COM_FLAGS=${COM_FLAGS}")
message(STATUS "MY=${MY}")
IF(MSVC)	 
	SET(COMP_FLAGS "-W3")
ELSE()
	SET(COMP_FLAGS "-ggdb -Wall -g")
ENDIF()


INCLUDE_DIRECTORIES("./")
INCLUDE_DIRECTORIES("./../common/")

IF(UNIX)
	SET(unix_include "/usr/include/" "/usr/local/include/")
	SET(unix_link_dir "/usr/lib/" "/usr/local/lib/" )
ENDIF()

INCLUDE_DIRECTORIES(${unix_include})

FILE(GLOB SRC_CPP "./*.cpp")
FILE(GLOB SRC_H "./*.h")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)

LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/bin/)
LINK_DIRECTORIES( ${unix_link_dir} )

 
ADD_EXECUTABLE(thread_test ${SRC_CPP} ${SRC_H} )


IF(MSVC)
	TARGET_LINK_LIBRARIES(thread_test common.lib)
	MESSAGE(STATUS "LINK_DIRECTORIES =  ${PROJECT_SOURCE_DIR}/bin/Debug/")
ELSE()
	TARGET_LINK_LIBRARIES(thread_test common.a pthread.so)
ENDIF()