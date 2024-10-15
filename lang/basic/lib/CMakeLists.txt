project(libbasic LANGUAGES C ASM)

# Define the target
string(REPLACE "lib" "" TARGET_NAME ${PROJECT_NAME})

if(NOT MACHINE)
message(SEND_ERROR "MACHINE is not set to indicate target install directory")
endif(NOT MACHINE)

set(SRC
 abs.c
 asc.c
 asrt.c
 atn.c
 bc_io.h
 chr.c
 conversion.c
 error.c
 exp.c
 fef.e
 fif.e
 file.c
 hlt.c
 io.c
 lib.h
 log.c
 mki.c
 oct.c
 peek.c
 power.c
 print.c
 random.c
 read.c
 return.c
 salloc.c
 setline.e
 sgn.c
 sin.c
 sqt.c
 stop.c
 string.c
 swap.c
 trace.c
 trap.c
 write.c
)

add_library(${TARGET_NAME} ${SRC})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(IS_ACK)
if(NOT DESCR_FILE)
message(SEND_ERROR "Descriptor file is not set")
endif(NOT DESCR_FILE)
target_compile_options(${TARGET_NAME} PRIVATE -m${DESCR_FILE} -Rbe=${CG} -Ras=${AS})
target_link_libraries(${TARGET_NAME} c)
endif(IS_ACK)

install(TARGETS ${TARGET_NAME} 
        LIBRARY DESTINATION ${MACHINE}
        ARCHIVE DESTINATION ${MACHINE}
        PUBLIC_HEADER DESTINATION include
		COMPONENT Development)        
