#cmake_minimum_required (VERSION 3.0)
#project (as)

set(LOCAL_NAME as-${ARCH})

set(SRCS
 ${ACK_BINARY_DIR}/mach/${ARCH}/as/${ARCH}-as.c
)

set(DIR_GENERIC ${ACK_SRC_HOME}/mach/proto/as)
set(DIR_COMMON ${ACK_SRC_HOME}/h)
set(DIR_FLOAT ${ACK_SRC_HOME}/modules/src/flt_arith)

set(SRCS_GENERIC
 ${ACK_SRC_HOME}/mach/proto/as/comm0.h
 ${ACK_SRC_HOME}/mach/proto/as/comm1.h
 ${ACK_SRC_HOME}/mach/proto/as/comm3.c
 ${ACK_SRC_HOME}/mach/proto/as/comm4.c
 ${ACK_SRC_HOME}/mach/proto/as/comm5.c
 ${ACK_SRC_HOME}/mach/proto/as/comm6.c
 ${ACK_SRC_HOME}/mach/proto/as/comm7.c
 ${ACK_SRC_HOME}/mach/proto/as/comm8.c 
)

FILE(MAKE_DIRECTORY ${ACK_BINARY_DIR}/mach/${ARCH}/as)

add_custom_command(
    OUTPUT ${ACK_BINARY_DIR}/mach/${ARCH}/as/${ARCH}-as.y
    COMMAND cpp -P -I${DIR_GENERIC} -I${ACK_SRC_HOME}/mach/${ARCH}/as ${DIR_GENERIC}/comm2.y > ${ACK_BINARY_DIR}/mach/${ARCH}/as/${ARCH}-as.y
)

add_custom_command(
    OUTPUT ${ACK_BINARY_DIR}/mach/${ARCH}/as/${ARCH}-as.c ${ACK_BINARY_DIR}/mach/${ARCH}/as/y.tab.h
    COMMAND ${CMAKE_COMMAND} -E chdir ${ACK_BINARY_DIR}/mach/${ARCH}/as yacc -b y -d ${ACK_BINARY_DIR}/mach/${ARCH}/as/${ARCH}-as.y
    COMMAND ${CMAKE_COMMAND} -E rename ${ACK_BINARY_DIR}/mach/${ARCH}/as/y.tab.c ${ACK_BINARY_DIR}/mach/${ARCH}/as/${ARCH}-as.c
    DEPENDS ${ACK_BINARY_DIR}/mach/${ARCH}/as/${ARCH}-as.y
)



add_executable(${LOCAL_NAME} ${SRCS_GENERIC} ${SRCS})

target_include_directories(${LOCAL_NAME} PRIVATE ${ACK_SRC_HOME}/mach/${ARCH}/as ${CMAKE_CURRENT_SOURCE_DIR} ${ACK_BINARY_DIR}/mach/${ARCH}/as ${DIR_FLOAT} ${DIR_GENERIC} ${DIR_COMMON})
target_link_libraries(${LOCAL_NAME} PRIVATE object flt)

set_property(TARGET ${LOCAL_NAME} PROPERTY OUTPUT_NAME as)
set_property(TARGET ${LOCAL_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${ACK_BINARY_DIR}/mach/${ARCH}/as)
install(TARGETS ${LOCAL_NAME} RUNTIME DESTINATION lib/ack/mach/${ARCH})

