#cmake_minimum_required (VERSION 3.0)
#project (cg)

set(LOCAL_NAME cg-${ARCH})

if(DEFINED MACH)
elseif(DEFINED MACH)
message(FATAL_ERROR "'MACH' variable must be defined")
endif(DEFINED MACH)

set(SRCS_GENERIC
				${ACK_SRC_HOME}/mach/proto/cg/codegen.c
				${ACK_SRC_HOME}/mach/proto/cg/compute.c
				${ACK_SRC_HOME}/mach/proto/cg/equiv.c
				${ACK_SRC_HOME}/mach/proto/cg/fillem.c
				${ACK_SRC_HOME}/mach/proto/cg/gencode.c
				${ACK_SRC_HOME}/mach/proto/cg/glosym.c
				${ACK_SRC_HOME}/mach/proto/cg/main.c
				${ACK_SRC_HOME}/mach/proto/cg/move.c
				${ACK_SRC_HOME}/mach/proto/cg/nextem.c
				${ACK_SRC_HOME}/mach/proto/cg/reg.c
				${ACK_SRC_HOME}/mach/proto/cg/regvar.c
				${ACK_SRC_HOME}/mach/proto/cg/salloc.c
				${ACK_SRC_HOME}/mach/proto/cg/state.c
				${ACK_SRC_HOME}/mach/proto/cg/subr.c
				${ACK_SRC_HOME}/mach/proto/cg/var.c
#				${ACK_SRC_HOME}/mach/${ARCH}/cg/mach.c
				${ACK_SRC_HOME}/mach/${ARCH}/cg/mach.h
)

FILE(MAKE_DIRECTORY ${ACK_BINARY_DIR}/mach/${ARCH}/cg)

set(GEN_H ${ACK_BINARY_DIR}/mach/${ARCH}/cg/tables.h)
set(GEN_SRC ${ACK_BINARY_DIR}/mach/${ARCH}/cg/tables.c)

add_executable(${LOCAL_NAME} ${SRCS_GENERIC} ${GEN_SRC})
target_include_directories(${LOCAL_NAME} PRIVATE ${ACK_SRC_HOME}/mach/proto/cg ${ACK_SRC_HOME}/mach/${ARCH}/cg ${ACK_BINARY_DIR}/mach/${ARCH}/cg)
target_link_libraries(${LOCAL_NAME} PRIVATE emheaders flt em_data)

add_custom_command(
    OUTPUT ${GEN_H} ${GEN_SRC}
    COMMAND cpp ${ACK_SRC_HOME}/mach/${ARCH}/cg/table>${ACK_BINARY_DIR}/mach/${ARCH}/cg/table
    COMMAND cgg -c${GEN_SRC} -h${GEN_H} ${ACK_BINARY_DIR}/mach/${ARCH}/cg/table
    DEPENDS cgg
)

set_property(TARGET ${LOCAL_NAME} PROPERTY OUTPUT_NAME cg)
set_property(TARGET ${LOCAL_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${ACK_BINARY_DIR}/mach/${ARCH}/cg)
install(TARGETS ${LOCAL_NAME} RUNTIME DESTINATION lib/ack/mach/${ARCH})





