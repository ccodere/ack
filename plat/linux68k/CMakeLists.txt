# 3.6 is required for Toolchain file support, CMAKE_TRY_COMPILE_TARGET
cmake_minimum_required (VERSION 3.6)
set(SYSTEM_NAME linux)
set(SYSTEM_PROCESSOR 68k)
set(ACK_HOME $ENV{ACK_HOME})
SET(CMAKE_INSTALL_PREFIX ${ACK_HOME}/share/ack CACHE PATH "Installation prefix" )
project(m68k-linux LANGUAGES C ASM)



set(MACHINE ${SYSTEM_NAME}${SYSTEM_PROCESSOR})

set(ACK_SRC_HOME ${CMAKE_CURRENT_SOURCE_DIR}/../..)
include(${ACK_SRC_HOME}/first/search.cmake)

set(CMAKE_INSTALL_PREFIX )


find_program(ACK ack PATH ${ACK_HOME}/bin)

message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_AR: ${CMAKE_AR}")

# DESCRIPTOR file
set(DESCR_FILE ${CMAKE_CURRENT_SOURCE_DIR}/descr) 
# Generic processor agnostic platform directory
set(SYS_GEN_ROOT ${ACK_SRC_HOME}/plat/${SYSTEM_NAME})
# Processor specfic platform directory
set(SYS_ARCH_ROOT ${ACK_SRC_HOME}/plat/${SYSTEM_NAME}${SYSTEM_PROCESSOR})
# Generic platform agnostic processor directory
set(SYS_MACHINE_ROOT ${ACK_SRC_HOME}/mach/m68020)

###############################################################
# libsys: This is platform specific, hence a specific project
###############################################################
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libsys)
###############################################################
# libc: This is mostly generic for library contents
# can be overriden by platform by creating a subdirectory
# under this directory with name libc to get the
# platform specific specificities - see proto.cmake
# for list of files to use
#############################################################
include(${ACK_SRC_HOME}/plat/proto/libc/proto.cmake) 
#############################################################
# libend : Section symbols, processor specific (alignments)
# platform agnostic
#############################################################
include(${SYS_MACHINE_ROOT}/libend/proto.cmake) 
#############################################################
# libem : EM instruction emulation, processor specific,
# platform agnostic
#############################################################
include(${SYS_MACHINE_ROOT}/libem/proto.cmake) 
#############################################################
# libbasic : processor and platform agnostic
#############################################################
add_subdirectory(${ACK_SRC_HOME}/lang/basic/lib ${CMAKE_CURRENT_BINARY_DIR}/libbasic)
#############################################################
# libpascal : processor and platform agnostic
#############################################################
add_subdirectory(${ACK_SRC_HOME}/lang/pc/libpc ${CMAKE_CURRENT_BINARY_DIR}/libpc)

# Install descriptor files
INSTALL(FILES ${DESCR_FILE}
	DESTINATION ${ACK_DATA_PATH}/descr
	RENAME ${MACHINE})

INSTALL(DIRECTORY 
	${SYS_ARCH_ROOT}/include
	${SYS_GEN_ROOT}/include
	DESTINATION ${ACK_DATA_PATH}/${MACHINE})
	