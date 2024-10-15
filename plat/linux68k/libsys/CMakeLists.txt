project(libsys LANGUAGES C ASM)

if(NOT ACK_SRC_HOME)
message(SEND_ERROR "ACK_SRC_HOME is not set")
endif(NOT ACK_SRC_HOME)

if(NOT DESCR_FILE)
message(SEND_ERROR "Descriptor file is not set")
endif(NOT DESCR_FILE)


# Define the target
string(REPLACE "lib" "" TARGET_NAME ${PROJECT_NAME})


# Processor agnostic operating system specific root directory
set(SYSTEM_ROOT ${ACK_SRC_HOME}/plat/${SYSTEM_NAME})
# Processor specfic operating system specific root directory
set(MACHINE_ROOT ${ACK_SRC_HOME}/plat/${SYSTEM_NAME}${SYSTEM_PROCESSOR})

# Generic include header file paths
set(GEN_C_INC ${ACK_SRC_HOME}/lang/cem/libcc.ansi/headers)
# System and platform specific header file paths
set(MACHINE_INC 
	${MACHINE_ROOT}/include
	${SYSTEM_ROOT}/include
	${MACHINE_ROOT}/${PROJECT_NAME}
	${SYSTEM_ROOT}/${PROJECT_NAME})

set(SRC_GEN
		${SYSTEM_ROOT}/${PROJECT_NAME}/close.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/creat.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/execve.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/_exit.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/fork.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/getpid.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/gettimeofday.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/_hol0.s
		${SYSTEM_ROOT}/${PROJECT_NAME}/ioctl.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/isatty.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/kill.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/lseek.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/open.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/read.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/sbrk.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/signal.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/sigprocmask.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/unlink.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/wait.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/waitpid.c
		${SYSTEM_ROOT}/${PROJECT_NAME}/write.c
)

set(SRC_SYS
		_syscall.s
)


add_library(${TARGET_NAME} ${SRC_GEN} ${SRC_SYS})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${MACHINE_INC} ${GEN_C_INC})
target_compile_options(${TARGET_NAME} PRIVATE -m${DESCR_FILE} -Rbe=${CG} -Ras=${AS})

install(TARGETS ${TARGET_NAME} 
        LIBRARY DESTINATION ${MACHINE}
        ARCHIVE DESTINATION ${MACHINE}
        PUBLIC_HEADER DESTINATION include
		COMPONENT Development)        

