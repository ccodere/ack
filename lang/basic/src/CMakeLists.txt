cmake_minimum_required (VERSION 3.0)
project(em_bem)

set(SRC
 basic.c
 bem.c
 bem.h
 compile.c
 eval.c
 eval.h
 func.c
 func.h
 gencode.c
 gencode.h
 graph.c
 graph.h
 initialize.c
 llmess.h
 parsepar.c
 parsepar.h
 symbols.c
 symbols.h
 util.c
 util.h
 yylexp.h
 ${CMAKE_CURRENT_BINARY_DIR}/Lpars.c
 ${CMAKE_CURRENT_BINARY_DIR}/Lpars.h
 ${CMAKE_CURRENT_BINARY_DIR}/tokentab.h
)

set(INCLUDE_DIRS
 .
 ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(
    OUTPUT basic.c Lpars.c Lpars.h
    COMMAND  ${CMAKE_COMMAND} -E env LLGEN_LIB_DIR=${CMAKE_CURRENT_SOURCE_DIR}/../../../util/LLgen/lib ${CMAKE_CURRENT_BINARY_DIR}/../../../util/LLgen/LLgen "${CMAKE_CURRENT_SOURCE_DIR}/basic.g"
    MAIN_DEPENDENCY basic.g
    DEPENDS LLgen basic.g)
    
add_custom_command(
   OUTPUT tokentab.h
   COMMAND sed -f ${CMAKE_CURRENT_SOURCE_DIR}/mktktab.sed ${CMAKE_CURRENT_BINARY_DIR}/Lpars.h >tokentab.h 
   MAIN_DEPENDENCY Lpars.h
   DEPENDS Lpars.h
)     

add_executable(${PROJECT_NAME} ${SRC})
target_compile_definitions(${PROJECT_NAME} PUBLIC NORCSID=1)
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} emheaders emh alloc print string system em_data eme em_mes)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION lib/ack)

