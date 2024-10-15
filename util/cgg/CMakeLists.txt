cmake_minimum_required (VERSION 2.9)
project (cgg)

set(SRC
 ${CMAKE_CURRENT_BINARY_DIR}/y.tab.c
 ${CMAKE_CURRENT_BINARY_DIR}/y.tab.h
 ${CMAKE_CURRENT_BINARY_DIR}/bootlex.c
 booth.h
 main.c
)

set(DIR_COMMON ${CMAKE_CURRENT_SOURCE_DIR}/../../h)


find_program (YACC NAMES yacc byacc bison)
find_program (LEX NAMES flex lex)

if(NOT YACC)
message(SEND_ERROR "yacc tool not found to build \"${PROJECT_NAME}\"")
endif(NOT YACC)

if(NOT LEX)
message(SEND_ERROR "lex tool not found to build \"${PROJECT_NAME}\"")
endif(NOT LEX)


add_custom_command(
    OUTPUT y.tab.c y.tab.h
    COMMAND ${YACC} -b y -d ${CMAKE_CURRENT_SOURCE_DIR}/bootgram.y
    DEPENDS bootgram.y
)

add_custom_command(
    OUTPUT bootlex.c
    COMMAND ${LEX} -st ${CMAKE_CURRENT_SOURCE_DIR}/bootlex.l >bootlex.c
    DEPENDS bootlex.l
)



add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${DIR_COMMON})
target_link_libraries(${PROJECT_NAME} em_data)


install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin
	COMPONENT Development)

