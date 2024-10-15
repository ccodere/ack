#cmake_minimum_required (VERSION 3.0)
#project (top)

if(DEFINED MACH)
elseif(DEFINED MACH)
message(FATAL_ERROR "'MACH' variable must be defined")
endif(DEFINED MACH)

file(MAKE_DIRECTORY ${ACK_BINARY_DIR}/mach/${ARCH}/top)


set(SRCS_GENERIC
 ${ACK_SRC_HOME}/mach/proto/top/top.c
 ${ACK_SRC_HOME}/mach/proto/top/queue.c
)


set(LOCAL_NAME top-${ARCH})

FILE(MAKE_DIRECTORY ${ACK_BINARY_DIR}/mach/${ARCH}/top)

set(GEN_SRC ${ACK_BINARY_DIR}/mach/${ARCH}/top/gen.c)
set(GEN_H ${ACK_BINARY_DIR}/mach/${ARCH}/top/gen.h)

add_executable(${LOCAL_NAME} ${SRCS_GENERIC} ${GEN_H})
target_include_directories(${LOCAL_NAME} PRIVATE ${ACK_SRC_HOME}/mach/proto/top ${ACK_BINARY_DIR}/mach/${ARCH}/top ${DIR_GENERIC} ${DIR_COMMON})
target_link_libraries(${LOCAL_NAME} PRIVATE string)

add_custom_command(
    OUTPUT ${GEN_SRC} ${GEN_H}
    COMMAND topgen ${ACK_SRC_HOME}/mach/${ARCH}/top/table ${ACK_BINARY_DIR}/mach/${ARCH}/top
    DEPENDS topgen
)

set_property(TARGET ${LOCAL_NAME} PROPERTY OUTPUT_NAME top)
set_property(TARGET ${LOCAL_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${ACK_BINARY_DIR}/mach/${ARCH}/top)
install(TARGETS ${LOCAL_NAME} RUNTIME DESTINATION lib/ack/mach/${ARCH})




