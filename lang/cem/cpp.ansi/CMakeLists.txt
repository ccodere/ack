cmake_minimum_required (VERSION 3.0)
project(cpp.ansi)

##################################
# Location of built scripts.
##################################
set(PROJECT_SCRIPTS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../util/scripts)

set(SRC_C
	ch3bin.c
	ch3bin.h
	ch3mon.c
	ch3mon.h
	class.h
	domacro.c
	domacro.h
	error.c
	error.h
	expr.c
	expr.h
	idf.c
	idf.h
	init.c
	init.h
	input.c
	input.h
	LLlex.c
	LLlex.h
	LLmessage.c
	main.c
	options.c
	options.h
	parameters.h
	preprocess.c
	replace.c
	skip.c
	tokenname.c
)

set(INCLUDE_DIRS
 .
 ${CMAKE_CURRENT_BINARY_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/../include
 ${CMAKE_CURRENT_SOURCE_DIR}/../../../h
 ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/src/idf
 ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/src/input
)

############################################
# LLgen inputs
############################################

set(GEN_G
	${CMAKE_CURRENT_BINARY_DIR}/tokenfile.g
)



set(SRC_G
 ${CMAKE_CURRENT_SOURCE_DIR}/expression.g
)


############################################
# LLgen outputs
############################################

set(GEN_SRC 
 ${CMAKE_CURRENT_BINARY_DIR}/expression.c 
 ${CMAKE_CURRENT_BINARY_DIR}/tokenfile.c
 ${CMAKE_CURRENT_BINARY_DIR}/Lpars.c
 ${CMAKE_CURRENT_BINARY_DIR}/Lpars.h
)


set(GFILES
 ${GEN_G}
 ${SRC_G}
)



############################################
# Configuration parameters generated files
############################################

set(GEN_OTHERS
	${CMAKE_CURRENT_BINARY_DIR}/replace.h
	${CMAKE_CURRENT_BINARY_DIR}/macro.h
	${CMAKE_CURRENT_BINARY_DIR}/next.c
)


set(IN_OTHERS
	${CMAKE_CURRENT_SOURCE_DIR}/replace.str
	${CMAKE_CURRENT_SOURCE_DIR}/macro.str
)



set(CFILES
	${SRC_C}
	${GEN_SRC}
	${GEN_OTHERS}
	symbol2str.c
	char.c
)

set(LLGENOPTIONS
 -v
)


    
###############################################
# Generate other source files
###############################################     
add_custom_command(
    OUTPUT ${GEN_OTHERS} mkalloc1.sed mkalloc2.sed mknext.sed
    COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SCRIPTS_SOURCE_DIR}/mkalloc1.sed
                ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed
    COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SCRIPTS_SOURCE_DIR}/mkalloc2.sed
                ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed
    COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SCRIPTS_SOURCE_DIR}/mknext.sed
                ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed   
                
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/macro.str>macro.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/replace.str>replace.h
     # Create next file from above files
    COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/next.str
                ${CMAKE_CURRENT_BINARY_DIR}/next.c
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/macro.str>>next.c
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/replace.str>>next.c
    DEPENDS ${IN_OTHERS}   
)



add_custom_command(
	OUTPUT char.c
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../../../util/cmisc/tabgen -f${CMAKE_CURRENT_SOURCE_DIR}/char.tab >char.c
)

###############################################
# Generate the LLGen source files 
# NOTE: Order of input file is important
############################################### 

add_custom_command(
    OUTPUT tokenfile.g mktkfile.sed
    COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SCRIPTS_SOURCE_DIR}/mktkfile.sed
                ${CMAKE_CURRENT_BINARY_DIR}/mktkfile.sed    
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mktkfile.sed ${CMAKE_CURRENT_SOURCE_DIR}/tokenname.c>tokenfile.g
)



add_custom_command(
    OUTPUT ${GEN_SRC}
    COMMAND  ${CMAKE_COMMAND} -E env LLGEN_LIB_DIR=${CMAKE_CURRENT_SOURCE_DIR}/../../../util/LLgen/lib ${CMAKE_CURRENT_BINARY_DIR}/../../../util/LLgen/LLgen ${LLGENOPTIONS} ${GEN_G} ${SRC_G}
    DEPENDS LLgen tokenfile.g)
    
find_program (CAT NAMES cat)

if(NOT CAT)
message(SEND_ERROR "cat tool not found to build \"${PROJECT_NAME}\"")
endif(NOT CAT)
    
add_custom_command(
   OUTPUT symbol2str.c
   COMMAND ${CAT} ${CMAKE_CURRENT_SOURCE_DIR}/tokcaseh.in >symbol2str.c 
   COMMAND sed -f ${CMAKE_CURRENT_SOURCE_DIR}/tokcase.sed ${CMAKE_CURRENT_SOURCE_DIR}/tokenname.c >>symbol2str.c 
   COMMAND ${CAT} ${CMAKE_CURRENT_SOURCE_DIR}/tokcasee.in >>symbol2str.c 
)     
 
add_executable(${PROJECT_NAME} ${CFILES})
target_compile_definitions(${PROJECT_NAME} PUBLIC NORCSID=1)
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} emheaders emk alloc print string system)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION lib/ack)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ncpp.6 DESTINATION share/man/man6 RENAME cpp.ansi.6 OPTIONAL)

