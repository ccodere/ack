###########################################################
# m68k-generic cross-compiler CMake toolchain file
# Prerequisites: ACK_HOME environment variable set
# home directory of ACK installation tree.
###########################################################
if(CMAKE_HOST_WIN32)
set(EXEC_SUFFIX ".exe")
else()
set(EXEC_SUFFIX ${CMAKE_HOST_EXECUTABLE_SUFFIX})
endif()
set(IS_ACK TRUE)

set(ACK_HOME $ENV{ACK_HOME})
# If descriptor exists, then then try_compile() can be run.
#if(EXISTS ${ACK_HOME}/lib/ack/descr/)

## System information ##
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR m68k)

## C compiler information ##
#set(CMAKE_C_COMPILER_ID_RUN TRUE)
set(CMAKE_C_COMPILER ${ACK_HOME}/bin/ack${EXEC_SUFFIX})
set(CMAKE_C_BYTE_ORDER BIG_ENDIAN)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_ID_RUN FALSE)
set(CMAKE_C_COMPILER_ID "ack")
set(CMAKE_C_OUTPUT_EXTENSION ".o")
#set(CMAKE_C_SOURCE_FILE_EXTENSIONS "c;s")
#set(CMAKE_C_SOURCE_FILE_EXTENSIONS "o")
#set(CMAKE_C_COMPILER_ID ack)
# CMake 3.7+ set(CMAKE_C_FLAGS_INIT XXX)

## Assembler information ##
set(CMAKE_ASM_COMPILER ${ACK_HOME}/bin/ack${EXEC_SUFFIX})
set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> <DEFINES>  <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>")
#set(CMAKE_ASM_BYTE_ORDER BIG_ENDIAN)
#set(CMAKE_ASM_COMPILER_FORCED TRUE)
#set(CMAKE_ASM_OUTPUT_EXTENSION ".o")
set(CMAKE_ASM_COMPILER_ID "${CMAKE_C_COMPILER_ID}")
set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS "s")



set(CMAKE_AR ${ACK_HOME}/bin/aal${EXEC_SUFFIX})
set(CMAKE_RANLIB "")
set(AS ${ACK_HOME}/lib/ack/mach/m68020/as${EXEC_SUFFIX})
set(CG ${ACK_HOME}/lib/ack/mach/m68020/ncg${EXEC_SUFFIX})
#set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE NEVER)
