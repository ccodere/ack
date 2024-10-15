#cmake_minimum_required (VERSION 3.0)
#project (mcg)

set(LOCAL_NAME mcg-${ARCH})

if(DEFINED MACH)
elseif(DEFINED MACH)
message(FATAL_ERROR "'MACH' variable must be defined")
endif(DEFINED MACH)

set(SRCS_GENERIC
				${ACK_SRC_HOME}/mach/proto/mcg/basicblock.c
				${ACK_SRC_HOME}/mach/proto/mcg/data.c
				${ACK_SRC_HOME}/mach/proto/mcg/graph.c
				${ACK_SRC_HOME}/mach/proto/mcg/hop.c
				${ACK_SRC_HOME}/mach/proto/mcg/ir.c
				${ACK_SRC_HOME}/mach/proto/mcg/main.c
				${ACK_SRC_HOME}/mach/proto/mcg/parse_em.c
				${ACK_SRC_HOME}/mach/proto/mcg/pass_convertstackops.c
				${ACK_SRC_HOME}/mach/proto/mcg/pass_eliminatetrivialblocks.c
				${ACK_SRC_HOME}/mach/proto/mcg/pass_groupirs.c
				${ACK_SRC_HOME}/mach/proto/mcg/pass_instructionselection.c
				${ACK_SRC_HOME}/mach/proto/mcg/pass_livevreganalysis.c
				${ACK_SRC_HOME}/mach/proto/mcg/pass_lowerpushes.c
				${ACK_SRC_HOME}/mach/proto/mcg/pass_phigroups.c
				${ACK_SRC_HOME}/mach/proto/mcg/pass_prologueepilogue.c
				${ACK_SRC_HOME}/mach/proto/mcg/pass_registerallocator.c
				${ACK_SRC_HOME}/mach/proto/mcg/pass_removedeadblocks.c
				${ACK_SRC_HOME}/mach/proto/mcg/pass_removedeadphis.c
				${ACK_SRC_HOME}/mach/proto/mcg/pass_returnvalues.c
				${ACK_SRC_HOME}/mach/proto/mcg/pass_splitcriticaledges.c
				${ACK_SRC_HOME}/mach/proto/mcg/pass_ssa.c
				${ACK_SRC_HOME}/mach/proto/mcg/pass_typeinference.c
				${ACK_SRC_HOME}/mach/proto/mcg/predicates.c
				${ACK_SRC_HOME}/mach/proto/mcg/procedure.c
				${ACK_SRC_HOME}/mach/proto/mcg/reg.c
				${ACK_SRC_HOME}/mach/proto/mcg/symbol.c
				${ACK_SRC_HOME}/mach/proto/mcg/treebuilder.c
				${ACK_SRC_HOME}/mach/${ARCH}/mcg/platform.c
				${ACK_BINARY_DIR}/mach/${ARCH}/mcg/ircodes.c
)

FILE(MAKE_DIRECTORY ${ACK_BINARY_DIR}/mach/${ARCH}/mcg)

set(GEN_H ${ACK_BINARY_DIR}/mach/${ARCH}/mcg/tables.h)
set(GEN_SRC ${ACK_BINARY_DIR}/mach/${ARCH}/mcg/tables.c)

add_executable(${LOCAL_NAME} ${SRCS_GENERIC} ${GEN_H} ${GEN_SRC})
target_include_directories(${LOCAL_NAME} PRIVATE ${ACK_SRC_HOME}/mach/proto/mcg ${ACK_SRC_HOME}/util/mcgg ${ACK_BINARY_DIR}/util/mcgg ${ACK_SRC_HOME}/mach/${ARCH}/mcg ${ACK_BINARY_DIR}/mach/${ARCH}/mcg)
target_link_libraries(${LOCAL_NAME} PRIVATE emheaders flt alloc em_data emk idf object read_emk string system)



add_custom_command(
    OUTPUT ${GEN_H} ${GEN_SRC}
    COMMAND cpp -P ${ACK_SRC_HOME}/mach/${ARCH}/mcg/table>${ACK_BINARY_DIR}/mach/${ARCH}/mcg/table
    COMMAND mcgg -o${GEN_SRC} -h${GEN_H} -i${ACK_BINARY_DIR}/mach/${ARCH}/mcg/table
    DEPENDS mcgg
)


add_custom_command(
  OUTPUT ${ACK_BINARY_DIR}/mach/${ARCH}/mcg/ircodes-dyn.h ${ACK_BINARY_DIR}/mach/${ARCH}/mcg/ircodes.c
  COMMAND ${AWK} -f ${ACK_SRC_HOME}/util/mcgg/ircodesh.awk ${ACK_SRC_HOME}/util/mcgg/ir.dat>${ACK_BINARY_DIR}/mach/${ARCH}/mcg/ircodes-dyn.h
  COMMAND ${AWK} -f ${ACK_SRC_HOME}/util/mcgg/ircodesc.awk ${ACK_SRC_HOME}/util/mcgg/ir.dat>${ACK_BINARY_DIR}/mach/${ARCH}/mcg/ircodes.c
  DEPENDS ${ACK_SRC_HOME}/util/mcgg/ir.dat
  COMMENT "Generating IR code files"
)


set_property(TARGET ${LOCAL_NAME} PROPERTY OUTPUT_NAME mcg)
set_property(TARGET ${LOCAL_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${ACK_BINARY_DIR}/mach/${ARCH}/mcg)
install(TARGETS ${LOCAL_NAME} RUNTIME DESTINATION lib/ack/mach/${ARCH})





