cmake_minimum_required (VERSION 2.9)
project (mcgg)

set(SRC
	iburg.c
)

set(HEADERS
	iburg.h
)

set(GEN_SRC
	${CMAKE_CURRENT_BINARY_DIR}/y.tab.c
	${CMAKE_CURRENT_BINARY_DIR}/scan.c
	${CMAKE_CURRENT_BINARY_DIR}/ircodes.c
)


find_program (YACC NAMES yacc byacc bison)
find_program (LEX NAMES flex lex)

if(NOT YACC)
message(SEND_ERROR "yacc tool not found to build \"${PROJECT_NAME}\"")
endif(NOT YACC)

if(NOT LEX)
message(SEND_ERROR "lex tool not found to build \"${PROJECT_NAME}\"")
endif(NOT LEX)

find_program (AWK NAMES awk gawk)

if(NOT AWK)
message(SEND_ERROR "awk tool not found to build \"${PROJECT_NAME}\"")
endif(NOT AWK)



add_custom_command(
    OUTPUT y.tab.c y.tab.h
    COMMAND ${YACC} -b y -d ${CMAKE_CURRENT_SOURCE_DIR}/gram.y
    DEPENDS gram.y
)

add_custom_command(
    OUTPUT scan.c
    COMMAND ${LEX} -st ${CMAKE_CURRENT_SOURCE_DIR}/scan.l >scan.c
    DEPENDS scan.l
)

add_custom_command(
  OUTPUT ircodes-dyn.h ircodes.c
  COMMAND ${AWK} -f ${CMAKE_CURRENT_SOURCE_DIR}/ircodesh.awk ${CMAKE_CURRENT_SOURCE_DIR}/ir.dat>ircodes-dyn.h
  COMMAND ${AWK} -f ${CMAKE_CURRENT_SOURCE_DIR}/ircodesc.awk ${CMAKE_CURRENT_SOURCE_DIR}/ir.dat>ircodes.c
  DEPENDS ir.dat
  COMMENT "Generating IR code files"
)


add_executable(${PROJECT_NAME} ${SRC} ${HEADERS} ${GEN_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} data em_data emheaders)


install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin
	COMPONENT Development)

