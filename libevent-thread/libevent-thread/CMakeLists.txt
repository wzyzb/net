CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(libeventThreadSample)

IF(MSVC)
	SET(COMP_FLAGS "-W3")
ELSE()
	SET(COMP_FLAGS "-ggdb -Wall")
ENDIF()

message(STATUS "PROJECT_NAME = ${PROJECT_NAME}")
message(STATUS "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
message(STATUS "PROJECT_BINARY_DIR" = ${PROJECT_BINARY_DIR} )
message(STATUS "${PROJECT_NAME}_SOURCE_DIR = ${common_SOURCE_DIR}")
message(STATUS "${PROJECT_NAME}_BINARY_DIR" = ${common_BINARY_DIR} )
message(STATUS "CMAKE_BUILD_TYPE" = ${CMAKE_BUILD_TYPE} )

message(STATUS "CMAKE_COMMAND" = ${CMAKE_COMMAND} )
message(STATUS "CMAKE_DEBUG_POSTFIX" = ${CMAKE_DEBUG_POSTFIX} )
message(STATUS "CMAKE_GENERATOR" = ${CMAKE_GENERATOR} )

message(STATUS "MSVC" = ${MSVC} )
message(STATUS "MSVC" = ${MSVC_VERSION} )

LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../../myProject/libevent-2.0.21-stable/lib/)
add_executable(${PROJECT_NAME} echoserver_threaded.c workqueue.c workqueue.h)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} libevent.lib libevent_extra.lib libevent_core.lib)