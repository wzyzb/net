BUILD=library
INCLUDES=-I../../../inc/common -I../../../../../thirdparty/fastdb/inc -I../../../../../thirdparty/stlport/inc -I../../../../../thirdparty/mysql/inc -I../../../inc -I../../../inc/net/impl

#just for debug compile flags
DEBUG_COMPILE_FLAGS=
DEBUG_LINK_FLAGS=

#just for release compile flags
RELEASE_COMPILE_FLAGS=
RELEASE_LINK_FLAGS=

CXXFLAGS=CXX_FLAGS -fPIC
CFLAGS=CXX_FLAGS -fPIC
MYLDFLAGS=LD_FLAGS

