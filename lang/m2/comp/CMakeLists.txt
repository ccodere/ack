cmake_minimum_required (VERSION 3.0)
project(em_m2)

##################################
# Compiler parameter configuration
# By default MinSizeRel will define
# the small parser.
##################################
if(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
MESSAGE(STATUS "${PROJECT_NAME}: Setting small parser configuration")
set(CONFIG_FILE SmallPars)
else()
MESSAGE(STATUS "${PROJECT_NAME}: Setting big parser configuration")
set(CONFIG_FILE BigPars)
endif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")


##################################
# Location of built scripts.
##################################
set(PROJECT_SCRIPTS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../util/scripts)


set(SRC_C
	chk_expr.c
	chk_expr.h
	class.h
	code.c
	code.h
	cstoper.c
	cstoper.h
	debug.h
	defmodule.c
	def.c
	desig.c
	desig.h
	enter.c
	enter.h
	error.c
	error.h
	f_info.h
	idf.c
	idf.h
	input.c
	input.h
	LLlex.c
	LLlex.h
	LLmessage.c
	lookup.c
	lookup.h
	main.c
	main.h
	misc.c
	misc.h
	node.c
	node.h
	options.c
	options.h
	real.h
	scope.c
	scope.h
	stab.c
	standards.h
	SYSTEMM2.h
	tokenname.c
	tokenname.h
	type.c
	type.h
	tmpvar.c
	tmpvar.h
	typequiv.c
	typequiv.h
	walk.c
	walk.h
	warning.h
	scope.c
	casestat.c
)

set(INCLUDE_DIRS
 .
 ${CMAKE_CURRENT_BINARY_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/../include
)


############################################
# LLgen inputs
############################################

set(GEN_G
	${CMAKE_CURRENT_BINARY_DIR}/tokenfile.g
)

set(SRC_G
 ${CMAKE_CURRENT_SOURCE_DIR}/program.g
 ${CMAKE_CURRENT_SOURCE_DIR}/declar.g
 ${CMAKE_CURRENT_SOURCE_DIR}/expression.g
 ${CMAKE_CURRENT_SOURCE_DIR}/statement.g
)

############################################
# LLgen outputs
############################################

set(GEN_SRC 
 ${CMAKE_CURRENT_BINARY_DIR}/program.c 
 ${CMAKE_CURRENT_BINARY_DIR}/declar.c 
 ${CMAKE_CURRENT_BINARY_DIR}/expression.c 
 ${CMAKE_CURRENT_BINARY_DIR}/statement.c
 ${CMAKE_CURRENT_BINARY_DIR}/tokenfile.c
 ${CMAKE_CURRENT_BINARY_DIR}/Lpars.c
 ${CMAKE_CURRENT_BINARY_DIR}/Lpars.h
)





set(GFILES
 ${SRC_G}
 ${GEN_G}
)



############################################
# Configuration parameters generated files
############################################

set(GEN_H_CONFIG
	${CMAKE_CURRENT_BINARY_DIR}/parameters.h
)

set(GEN_OTHERS
	${CMAKE_CURRENT_BINARY_DIR}/next.c
)

set(IN_OTHERS
	${CMAKE_CURRENT_SOURCE_DIR}/def.h
	${CMAKE_CURRENT_SOURCE_DIR}/type.h
	${CMAKE_CURRENT_SOURCE_DIR}/node.h	
	${CMAKE_CURRENT_SOURCE_DIR}/real.h
	${CMAKE_CURRENT_SOURCE_DIR}/scope.c
	${CMAKE_CURRENT_SOURCE_DIR}/tmpvar.c
	${CMAKE_CURRENT_SOURCE_DIR}/casestat.c
)                


set(CFILES
	${SRC_C}
	${GEN_SRC}
	${GEN_H_CONFIG}
	${GEN_OTHERS}
	symbol2str.c
	char.c
)

set(LLGENOPTIONS
)

###############################################
# Generate the compile time configuration file 
###############################################
add_custom_command(
    OUTPUT ${GEN_H_CONFIG} mkparams.sed
    COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SCRIPTS_SOURCE_DIR}/mkparams.sed
                ${CMAKE_CURRENT_BINARY_DIR}/mkparams.sed
    COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG_FILE}
                ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE}                      
    COMMAND sed -f ${PROJECT_SCRIPTS_SOURCE_DIR}/mkparams.sed ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE}>parameters.h
    DEPENDS ${CONFIG_FILE})    


    
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
                
                
#    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/def.xh>def.h
#    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/type.xh>type.h
#    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/node.xh>node.h   
#    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/real.xh>real.h
#    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/scope.xc>scope.c      
#    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/tmpvar.xc>tmpvar.c
#    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc1.sed -f ${CMAKE_CURRENT_BINARY_DIR}/mkalloc2.sed ${CMAKE_CURRENT_SOURCE_DIR}/casestat.xc>casestat.c
    # Create next file from above files
    COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/next.in
                ${CMAKE_CURRENT_BINARY_DIR}/next.c
     COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/def.h>>next.c
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/type.h>>next.c
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/node.h>>next.c
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/real.h>>next.c  
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/scope.c>>next.c
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/tmpvar.c>>next.c
    COMMAND sed -n -f ${CMAKE_CURRENT_BINARY_DIR}/mknext.sed ${CMAKE_CURRENT_SOURCE_DIR}/casestat.c>>next.c 
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
    DEPENDS LLgen ${SRC_G} ${GEN_G} )
    
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
target_link_libraries(${PROJECT_NAME} emheaders emh alloc print string system em_data emk em_mes flt idf input)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION lib/ack)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/em_m2.6 DESTINATION share/man/man6 OPTIONAL)

