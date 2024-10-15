cmake_minimum_required (VERSION 2.9)
project (ncgg)

set(SRC
 ${CMAKE_CURRENT_BINARY_DIR}/y.tab.c
 ${CMAKE_CURRENT_BINARY_DIR}/y.tab.h
 ${CMAKE_CURRENT_BINARY_DIR}/scan.c
 coerc.c
 emlookup.c 
 error.c
 expr.c
 hall.c
 instruct.c
 iocc.c 
 lookup.c
 main.c
 output.c
 set.c
 strlookup.c
 subr.c
 var.c
)

set(HEADERS
	cost.h
	expr.h
	extern.h
	instruct.h
	iocc.h
	lookup.h
	param.h
	property.h
	pseudo.h
	reg.h
	regvar.h
	set.h
    token.h
    varinfo.h
)

set(GEN_SRC
	${CMAKE_CURRENT_BINARY_DIR}/enterkeyw.c
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
    COMMAND ${YACC} -b y -d ${CMAKE_CURRENT_SOURCE_DIR}/cgg.y
    DEPENDS cgg.y
)

add_custom_command(
    OUTPUT scan.c
    COMMAND ${LEX} -st ${CMAKE_CURRENT_SOURCE_DIR}/scan.l >scan.c
    DEPENDS scan.l
)

add_custom_command(
  OUTPUT ${GEN_SRC}
  COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/cvtkeywds.awk
                ${CMAKE_CURRENT_BINARY_DIR}/cvtkeywds.awk
  COMMAND ${AWK} -f ${CMAKE_CURRENT_BINARY_DIR}/cvtkeywds.awk ${CMAKE_CURRENT_SOURCE_DIR}/keywords>enterkeyw.c
  COMMENT "Generating keyword files"
)


add_executable(${PROJECT_NAME} ${SRC} ${HEADERS} ${GEN_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} em_data emheaders)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin
	COMPONENT Development)
