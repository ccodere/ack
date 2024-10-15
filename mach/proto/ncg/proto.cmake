#cmake_minimum_required (VERSION 3.0)
#project (ncg)



if(DEFINED MACH)
elseif(DEFINED MACH)
message(FATAL_ERROR "'MACH' variable must be defined")
endif(DEFINED MACH)

set(LOCAL_NAME ncg-${ARCH})


set(SRCS_GENERIC
	${ACK_SRC_HOME}/mach/proto/ncg/codegen.c
	${ACK_SRC_HOME}/mach/proto/ncg/compute.c
	${ACK_SRC_HOME}/mach/proto/ncg/equiv.c
	${ACK_SRC_HOME}/mach/proto/ncg/fillem.c
	${ACK_SRC_HOME}/mach/proto/ncg/gencode.c
	${ACK_SRC_HOME}/mach/proto/ncg/glosym.c
	${ACK_SRC_HOME}/mach/proto/ncg/label.c
	${ACK_SRC_HOME}/mach/proto/ncg/main.c
	${ACK_SRC_HOME}/mach/proto/ncg/move.c
	${ACK_SRC_HOME}/mach/proto/ncg/nextem.c
	${ACK_SRC_HOME}/mach/proto/ncg/reg.c
	${ACK_SRC_HOME}/mach/proto/ncg/regvar.c
	${ACK_SRC_HOME}/mach/proto/ncg/salloc.c
	${ACK_SRC_HOME}/mach/proto/ncg/state.c
	${ACK_SRC_HOME}/mach/proto/ncg/subr.c
	${ACK_SRC_HOME}/mach/proto/ncg/var.c
)


set(HEADERS_GENERIC
				${ACK_SRC_HOME}/mach/proto/ncg/data.h
				${ACK_SRC_HOME}/mach/proto/ncg/equiv.h
				${ACK_SRC_HOME}/mach/proto/ncg/glosym.h
				${ACK_SRC_HOME}/mach/proto/ncg/label.h
				${ACK_SRC_HOME}/mach/proto/ncg/param.h
				${ACK_SRC_HOME}/mach/proto/ncg/regvar.h
				${ACK_SRC_HOME}/mach/proto/ncg/result.h
				${ACK_SRC_HOME}/mach/proto/ncg/state.h
				${ACK_SRC_HOME}/mach/proto/ncg/types.h
				${ACK_SRC_HOME}/mach/proto/ncg/xmach.h
)

FILE(MAKE_DIRECTORY ${ACK_BINARY_DIR}/mach/${ARCH}/ncg)

set(GEN_H ${ACK_BINARY_DIR}/mach/${ARCH}/ncg/tables.h)
set(GEN_SRC ${ACK_BINARY_DIR}/mach/${ARCH}/ncg/tables.c)

add_executable(${LOCAL_NAME} ${SRCS_GENERIC} ${HEADERS_GENERIC} ${GEN_SRC})
target_include_directories(${LOCAL_NAME} PRIVATE ${ACK_SRC_HOME}/mach/proto/ncg ${ACK_SRC_HOME}/mach/${ARCH}/ncg ${ACK_BINARY_DIR}/mach/${ARCH}/ncg)
target_link_libraries(${LOCAL_NAME} PRIVATE emheaders flt em_data object)

add_custom_command(
    OUTPUT ${GEN_H} ${GEN_SRC}
    WORKING_DIRECTORY ${ACK_BINARY_DIR}/mach/${ARCH}/ncg
    COMMAND cpp ${ACK_SRC_HOME}/mach/${ARCH}/ncg/table>${ACK_BINARY_DIR}/mach/${ARCH}/ncg/table
    COMMAND ncgg ${ACK_BINARY_DIR}/mach/${ARCH}/ncg/table
    DEPENDS ncgg
)

set_property(TARGET ${LOCAL_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${ACK_BINARY_DIR}/mach/${ARCH}/ncg)
set_property(TARGET ${LOCAL_NAME} PROPERTY OUTPUT_NAME ncg)
install(TARGETS ${LOCAL_NAME} RUNTIME DESTINATION lib/ack/mach/${ARCH})



