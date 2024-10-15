cmake_minimum_required (VERSION 2.9)
project(int)
include(CheckSymbolExists)
include(CheckIncludeFile)

enable_testing()

set(SRC_C
	alloc.c
	alloc.h
	core.c
	data.c
	data.h
	debug.h
	disassemble.c
	do_array.c
	do_branch.c
	do_comp.c
	do_conv.c
	do_fpar.c
	do_incdec.c
	do_intar.c
	do_load.c
	do_logic.c
	do_misc.c
	do_proc.c
	do_ptrar.c
	do_sets.c
	do_store.c
	do_unsar.c
	dump.c
	e.out.h
	fra.c
	fra.h
	global.c
	global.h
	init.c
	io.c
	io.h
	linfil.h
	log.c
	log.h
	logging.h
	m_ioctl.c
	m_sigtrp.c
	m_sigtrp.h
	main.c
	mem.h
	memdirect.h
	moncalls.c
	mondefs.h
	monstruct.c
	monstruct.h
	nofloat.h
	opcode.h
	proctab.c
	proctab.h
	read.c
	read.h
	rsb.c
	rsb.h
	segcheck.h
	segment.c
	segment.h
	shadow.h
	stack.c
	stack.h
	switch.c
	sysidf.h
	tally.c
	text.c
	text.h
	trap.c
	trap.h
	v7ioctl.h
	warn.c
)


############################################
# Configuration file based on libc functionality
############################################


check_include_file("unistd.h" HAS_UNISTD_H)
check_include_file("sys/wait.h" HAS_SYS_WAIT_H)
check_include_file("sys/stat.h" HAS_SYS_STAT_H)
check_include_file("sys/times.h" HAS_SYS_TIMES_H)
check_include_file("sys/timeb.h" HAS_SYS_TIMEB_H)
check_include_file("fcntl.h" HAS_FCNTL_H)

check_symbol_exists(alarm "unistd.h" HAS_ALARM)
check_symbol_exists(access "unistd.h" HAS_ACCESS)
check_symbol_exists(fork "unistd.h" HAS_FORK)
check_symbol_exists(ftime "sys/timeb.h" HAS_FTIME)
check_symbol_exists(kill "signal.h" HAS_KILL)
check_symbol_exists(pause "unistd.h" HAS_PAUSE)
check_symbol_exists(stat "sys/stat.h" HAS_STAT)
check_symbol_exists(sync "unistd.h" HAS_SYNC)
check_symbol_exists(times "sys/times.h" HAS_WAIT)
check_symbol_exists(wait "sys/wait.h" HAS_WAIT)

# Non mandatory API calls 
if(NOT HAS_ALARM)
message(WARNING "interpreter: alarm() interface is not available, some functionality removed.")
endif(NOT HAS_ALARM)
if(NOT HAS_SYNC)
message(WARNING "interpreter: sync() interface is not available, some functionality removed.")
endif(NOT HAS_SYNC)
if(NOT HAS_FORK)
message(WARNING "interpreter: fork() interface is not available, some functionality removed.")
endif(NOT HAS_FORK)
if(NOT HAS_WAIT)
message(WARNING "interpreter: wait() interface is not available, some functionality removed.")
endif(NOT HAS_WAIT)


# Emulated API calls
if(NOT HAS_KILL)
message(WARNING "interpreter: kill() interface is not available, functionaly limited through emulation.")
endif(NOT HAS_KILL)

# Mandatory API calls and include files

if(NOT HAS_STAT)
message(FATAL_ERROR "interpreter: stat() interface is not available but is required.")
endif(NOT HAS_STAT)

if(NOT HAS_ACCESS)
message(FATAL_ERROR "interpreter: access() interface is not available but is required.")
endif(NOT HAS_ACCESS)

if(NOT HAS_FCNTL_H)
message(FATAL_ERROR "interpreter: fcntl.h is not available but is required.")
endif(NOT HAS_FCNTL_H)


configure_file(config.h.in config.h @ONLY)


############################################
# Generated data files from custom commands
############################################

set(GEN_WARN_H
	${CMAKE_CURRENT_BINARY_DIR}/warn.h
	${CMAKE_CURRENT_BINARY_DIR}/warn_msg
	)
	
set(GEN_DO_CASES
  ${CMAKE_CURRENT_BINARY_DIR}/DoCases
)


set(GEN_PR_CASES
  ${CMAKE_CURRENT_BINARY_DIR}/PrCases
)

set(GEN_TRAP_MSG
  ${CMAKE_CURRENT_BINARY_DIR}/trap_msg
)

add_executable(${PROJECT_NAME} ${SRC_C} ${GEN_WARN_H} ${GEN_DO_CASES} ${GEN_PR_CASES} ${GEN_TRAP_MSG})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} emh emheaders em_data)	

###################################################
# Utilities used to generates some data files.
###################################################
add_executable(mkiswitch switch/mkiswitch.c)  
target_include_directories(mkiswitch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(mkiswitch emh emheaders em_data)	

add_executable(mkswitch switch/mkswitch.c)  
target_include_directories(mkswitch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(mkswitch emh emheaders em_data)	

#################################################
# Generate the data files
#################################################
find_program (AWK NAMES awk gawk)

if(NOT AWK)
message(SEND_ERROR "awk tool not found to build \"${PROJECT_NAME}\"")
endif(NOT AWK)



add_custom_command(
   OUTPUT ${GEN_WARN_H}
   COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/mkwarnh.awk
                ${CMAKE_CURRENT_BINARY_DIR}/mkwarnh.awk  
   COMMAND ${AWK} -f ${CMAKE_CURRENT_BINARY_DIR}/mkwarnh.awk ${CMAKE_CURRENT_SOURCE_DIR}/../../doc/int/appA
   COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/mkwarnmsg.awk
                ${CMAKE_CURRENT_BINARY_DIR}/mkwarnmsg.awk  
   COMMAND ${AWK} -f ${CMAKE_CURRENT_BINARY_DIR}/mkwarnmsg.awk ${CMAKE_CURRENT_SOURCE_DIR}/../../doc/int/appA
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../doc/int/appA   
)

add_custom_command(
	OUTPUT ${GEN_DO_CASES}
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/mkiswitch Do ${CMAKE_CURRENT_SOURCE_DIR}/../../etc/ip_spec.t  DoCases
	DEPENDS mkiswitch ${CMAKE_CURRENT_SOURCE_DIR}/../../etc/ip_spec.t
)

add_custom_command(
	OUTPUT ${GEN_PR_CASES}
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/mkswitch Pr ${CMAKE_CURRENT_SOURCE_DIR}/../../etc/ip_spec.t  PrCases
	DEPENDS mkswitch ${CMAKE_CURRENT_SOURCE_DIR}/../../etc/ip_spec.t
)

add_custom_command(
	OUTPUT ${GEN_TRAP_MSG}
   COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/mktrapmsg.awk
                ${CMAKE_CURRENT_BINARY_DIR}/mktrapmsg.awk 
   COMMAND ${AWK} -f ${CMAKE_CURRENT_BINARY_DIR}/mktrapmsg.awk ${CMAKE_CURRENT_SOURCE_DIR}/../../etc/traps
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../etc/traps              	
)


####################################################
# Autodoc target
####################################################

set(doxyfile_in ../../first/Doxyfile.in)
set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
configure_file(${doxyfile_in} ${doxyfile} @ONLY)

add_custom_target(autodoc
        COMMAND doxygen ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

####################################################
# Install target
####################################################


install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)


###############################################
# Generate and install documentation
###############################################     

if(${GENERATE_DOCS})
include(${ACK_SRC_HOME}/first/gendoc.cmake)
gendoc(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/int.1 man int1)
endif(${GENERATE_DOCS})

