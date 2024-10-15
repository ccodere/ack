cmake_minimum_required (VERSION 2.9)
project(em_data)

set(SRC
 em_spec.h
 em_pseu.h
 em_pseu.c
 em_mnem.h
 em_mnem.c
 em_flag.c
 em_ptyp.c
)

set(INCLUDE_DIRS 
 ${CMAKE_CURRENT_SOURCE_DIR}/../../h 
 ${CMAKE_CURRENT_SOURCE_DIR}/../../../h
)

find_program (AWK NAMES awk gawk)

if(NOT AWK)
message(SEND_ERROR "awk tool not found to build \"${PROJECT_NAME}\"")
endif(NOT AWK)


add_library(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} emheaders emh)

add_custom_command(
  OUTPUT em_spec.h em_pseu.h em_pseu.c em_mnem.h em_mnem.c em_flag.c
  COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/makeem.awk
                ${CMAKE_CURRENT_BINARY_DIR}/makeem.awk  
  COMMAND ${AWK} -f ${CMAKE_CURRENT_BINARY_DIR}/makeem.awk ${CMAKE_CURRENT_SOURCE_DIR}/../../../h/em_table
  COMMENT "Generating em_data files"
)

install(TARGETS ${PROJECT_NAME} 
        LIBRARY DESTINATION modules/lib
        ARCHIVE DESTINATION modules/lib
		COMPONENT Development)        





