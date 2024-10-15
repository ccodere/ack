cmake_minimum_required (VERSION 2.9)
project (misc)

set(ENCODE_SRCS
 convert.c
)

set(DECODE_SRCS
 convert.c
)


set(INCLUDE_DIRS
 ${CMAKE_CURRENT_SOURCE_DIR}/../../h
 ${CMAKE_CURRENT_SOURCE_DIR}/../../modules/h
)




add_executable(em_encode ${ENCODE_SRCS})
target_compile_definitions(em_encode PUBLIC NORCSID=1)
target_link_libraries(em_encode PRIVATE em_data string read_emeV emk alloc print system)
target_include_directories(em_encode PRIVATE ${INCLUDE_DIRS})

add_executable(em_decode ${DECODE_SRCS})
target_compile_definitions(em_decode PUBLIC NORCSID=1)
target_link_libraries(em_decode PRIVATE em_data string read_emkV eme alloc print system)
target_include_directories(em_decode PRIVATE ${INCLUDE_DIRS})

add_executable(esize esize.c)
target_compile_definitions(esize PUBLIC NORCSID=1)
target_link_libraries(esize PRIVATE em_data string read_emkV eme alloc print system)
target_include_directories(esize PRIVATE ${INCLUDE_DIRS})



install(TARGETS em_encode RUNTIME DESTINATION lib/ack)
install(TARGETS em_decode RUNTIME DESTINATION lib/ack)

install(TARGETS esize RUNTIME DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/esize.1 DESTINATION share/man/man1 OPTIONAL)

###############################################
# Generate and install documentation
###############################################     

if(${GENERATE_DOCS})
include(${ACK_SRC_HOME}/first/gendoc.cmake)
gendoc(esize ${CMAKE_CURRENT_SOURCE_DIR}/esize.1 man esize1)
gendoc(em_decode ${CMAKE_CURRENT_SOURCE_DIR}/em_decode.6 man em_decode6)

endif(${GENERATE_DOCS})

