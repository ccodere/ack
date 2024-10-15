project(libpascal LANGUAGES C ASM)

# Define the target
string(REPLACE "lib" "" TARGET_NAME ${PROJECT_NAME})

if(NOT MACHINE)
message(SEND_ERROR "MACHINE is not set to indicate target install directory")
endif(NOT MACHINE)

if(NOT ACK_SRC_HOME)
message(SEND_ERROR "ACK_SRC_HOME is not set")
endif(NOT ACK_SRC_HOME)


set(SRC
 abi.c
 abl.c
 abr.c
 arg.c
 ass.c
 asz.c
 atn.c
 bcp.c
 bts.e
 buff.c
 catch.c
 cls.c
 cvt.c
 diag.c
 efl.c
 eln.c
 encaps.e
 exp.c
 fef.e
 fif.e
 get.c
 gto.e
 hlt.c
 hol0.e
 incpt.c
 ini.c
 log.c
 mdi.c
 mdl.c
 new.c
 nfa.c
 nobuff.c
 notext.c
 opn.c
 outcpt.c
 pac.c
 pclose.c
 pcreat.c
 pentry.c
 perrno.c
 pexit.c
 popen.c
 put.c
 rcka.c
 rdc.c
 rdi.c
 rdl.c
 rdr.c
 rf.c
 rln.c
 rnd.c
 sig.e
 sin.c
 sqt.c
 string.c
 trap.e
 trp.e
 unp.c
 uread.c
 uwrite.c
 wdw.c
 wf.c
 wrc.c
 wrf.c
 wri.c
 wrl.c
 wrr.c
 wrs.c
 wrz.c
)

set(EM_INC ${ACK_SRC_HOME}/h)

add_library(${TARGET_NAME} ${SRC})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../include ${EM_INC})

if(IS_ACK)
if(NOT DESCR_FILE)
message(SEND_ERROR "Descriptor file is not set")
endif(NOT DESCR_FILE)
target_compile_options(${TARGET_NAME} PRIVATE -m${DESCR_FILE} -Rbe=${CG} -Ras=${AS})
target_link_libraries(${TARGET_NAME} c)

# Startup routine
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
  COMMAND ${CMAKE_C_COMPILER} -m${DESCR_FILE} -Rbe=${CG} -Ras=${AS} -I${EM_INC} -c ${ACK_SRC_HOME}/lang/pc/libpc/head_pc.e -o pascal.o 
  BYPRODUCTS pascal.o
)


INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/pascal.o
	DESTINATION ${MACHINE})

endif(IS_ACK)




install(TARGETS ${TARGET_NAME} 
        LIBRARY DESTINATION ${MACHINE}
        ARCHIVE DESTINATION ${MACHINE}
        PUBLIC_HEADER DESTINATION include
		COMPONENT Development)        
