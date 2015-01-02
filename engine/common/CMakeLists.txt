#cmake_minimum_required(VERSION 2.8)

#project(common)

IF(MSVC)	 
	SET(COMP_FLAGS "-W3")
ELSE()
	SET(COMP_FLAGS "-ggdb -Wall -g  ")#-I/usr/include/ -lpthread
ENDIF()

file(GLOB_RECURSE project_headers *.h)
file(GLOB_RECURSE project_cpps *.cpp)
set(all_files ${project_headers} ${project_cpps})
MESSAGE(STATUS "all_files")
FOREACH(cpp ${all_files} ) 
	MESSAGE(STATUS "${cpp}")
ENDFOREACH()
source_group_by_dir(all_files)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/common/thread/") 
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/common/lock/")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/common/exception/")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/common/")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/common/util/")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/common/net/")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/common/io/")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/common/log/")
IF(UNIX)
	SET(unix_include "/usr/include/" "/usr/local/include/")
	SET(unix_link_dir "/usr/lib/" "/usr/local/lib/")
ENDIF()
#INCLUDE_DIRECTORIES( ${unix_include})



FILE(GLOB LOCK_LIST_CPP "./lock/*.cpp")
FILE(GLOB THREAD_LIST_CPP "./thread/*.cpp")
FILE(GLOB EXCEPTION_LIST_CPP "./exception/*.cpp")
FILE(GLOB UTIL_LIST_CPP "./util/*.cpp")
FILE(GLOB NET_LIST_CPP "./net/*.cpp")
FILE(GLOB IO_LIST_CPP "./io/*.cpp")
FILE(GLOB LOG_LIST_CPP "./log/*.cpp")

FILE(GLOB LOCK_LIST_H "./lock/*.h")
FILE(GLOB THREAD_LIST_H "./thread/*.h")
FILE(GLOB EXCEPTION_LIST_H "./exception/*.h")
FILE(GLOB CUR_LIST_H "./*.h")
FILE(GLOB UTIL_LIST_H "./util/*.h")
FILE(GLOB NET_LIST_H "./net/*.h")
FILE(GLOB IO_LIST_H "./io/*.h")
FILE(GLOB LOG_LIST_H "./log/*.h")

FOREACH(cpp ${NET_LIST_H} ) 
	MESSAGE(STATUS "${cpp}")
ENDFOREACH()

MESSAGE(STATUS "aux_source_directory")
aux_source_directory(./net/driver DRIVER)
FOREACH(cpp ${DRIVER} ) 
	MESSAGE(STATUS "${cpp}")
ENDFOREACH()

#source_group(lock FILES ${LOCK_LIST_CPP} ${LOCK_LIST_H})
#source_group(exception FILES ${EXCEPTION_LIST_CPP} ${EXCEPTION_LIST_H})
#source_group(thread FILES ${THREAD_LIST_CPP} ${THREAD_LIST_H})
#source_group(util FILES ${UTIL_LIST_CPP} ${UTIL_LIST_H})
source_group("net\\driver" FILES ${DRIVER})
#source_group(net FILES ${NET_LIST_CPP} ${NET_LIST_H})
#source_group(io FILES ${IO_LIST_CPP} ${IO_LIST_H})
#source_group(log FILES ${LOG_LIST_CPP} ${LOG_LIST_H})



SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)
#LINK_DIRECTORIES( ${unix_link_dir} )

ADD_LIBRARY( common STATIC  ${CUR_LIST_H} ${LOCK_LIST_CPP} ${EXCEPTION_LIST_CPP} ${THREAD_LIST_CPP} ${LOCK_LIST_H} ${EXCEPTION_LIST_H}
                  ${THREAD_LIST_H}  ${UTIL_LIST_CPP} ${UTIL_LIST_H} ${NET_LIST_H} ${NET_LIST_CPP} ${IO_LIST_H} ${IO_LIST_CPP} 
				  ${LOG_LIST_H} ${LOG_LIST_CPP})


SET_TARGET_PROPERTIES(common
		PROPERTIES 
		COMPILE_FLAGS ${COMP_FLAGS}
)

IF(MSVC)
	#TARGET_LINK_LIBRARIES(thread_test common.lib)
	MESSAGE(STATUS "LINK_DIRECTORIES =  ${PROJECT_SOURCE_DIR}/bin/Debug/")
ELSE()
#	TARGET_LINK_LIBRARIES(common lpthread.a)
ENDIF()
