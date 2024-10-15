cmake_minimum_required (VERSION 3.0)
project(read_em)


set(ESRCFILES
 read_em.c
 mkcalls.c
 EM_vars.c
)

set(KSRCFILES
 read_em.c
 mkcalls.c
 EM_vars.c
)

set(SRCFILES 
 em_comp.h
 read_em.c
 reade.c
 readk.c
 mkcalls.c
 EM_vars.c
)


set(INCLUDE_DIRS 
 ${CMAKE_CURRENT_SOURCE_DIR}/../../../../h
 ${CMAKE_CURRENT_SOURCE_DIR}/../../h
 ${CMAKE_CURRENT_SOURCE_DIR}/../../../h
)

#include_directories(${INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(genheader DEPENDS C_mnem_narg.h C_mnem.h) 

add_library(read_emk read_em.c em_comp.h EM_vars.c mkcalls.c)
add_dependencies(read_emk genheader) 
target_compile_definitions(read_emk PRIVATE COMPACT=1 PRIVATE=static EXPORT=)
target_include_directories(read_emk PUBLIC .)
target_include_directories(read_emk PRIVATE ${INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
# Module dependencies
target_link_libraries(read_emk PRIVATE emk alloc)



add_library(read_emeV read_em.c em_comp.h EM_vars.c mkcalls.c)
add_dependencies(read_emeV genheader) 
target_compile_definitions(read_emeV PRIVATE CHECKING=1 PRIVATE=static EXPORT=)
target_include_directories(read_emeV PUBLIC .)
target_include_directories(read_emeV PRIVATE ${INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
# Module dependencies
target_link_libraries(read_emeV PRIVATE eme alloc)

add_library(read_emkV read_em.c em_comp.h EM_vars.c mkcalls.c)
add_dependencies(read_emkV genheader) 
target_compile_definitions(read_emkV PRIVATE COMPACT=1 CHECKING=1 PRIVATE=static EXPORT=)
target_include_directories(read_emkV PUBLIC .)
target_include_directories(read_emkV PRIVATE ${INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
# Module dependencies
target_link_libraries(read_emkV PRIVATE emk alloc)

find_program (AWK NAMES awk gawk)

if(NOT AWK)
message(SEND_ERROR "awk tool not found to build \"${PROJECT_NAME}\"")
endif(NOT AWK)



add_custom_command(
  OUTPUT C_mnem.h
  COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/C_nmem.awk
                ${CMAKE_CURRENT_BINARY_DIR}/C_nmem.awk
  COMMAND ${AWK} -f ${CMAKE_CURRENT_BINARY_DIR}/C_nmem.awk ${CMAKE_CURRENT_SOURCE_DIR}/../../../h/em_table>C_mnem.h
  COMMENT "Generating mnemonic switch"
)

add_custom_command(
  OUTPUT  C_mnem_narg.h
  COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/C_nmem_na.awk
                ${CMAKE_CURRENT_BINARY_DIR}/C_nmem_na.awk
  COMMAND ${AWK} -f ${CMAKE_CURRENT_BINARY_DIR}/C_nmem_na.awk ${CMAKE_CURRENT_SOURCE_DIR}/../../../h/em_table>C_mnem_narg.h
  COMMENT "Generating mnemonic no argument switch"
)


set_target_properties(read_emk PROPERTIES PUBLIC_HEADER "em_comp.h")


install(TARGETS read_emk read_emeV read_emkV
        LIBRARY DESTINATION modules/lib
        ARCHIVE DESTINATION modules/lib
        PUBLIC_HEADER DESTINATION modules/h
		COMPONENT Development
)


install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/read_em.3 DESTINATION share/man/man3
		COMPONENT Development
		OPTIONAL)
