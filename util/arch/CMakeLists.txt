cmake_minimum_required (VERSION 3.0)
project(aal)

add_executable(${PROJECT_NAME} archiver.c)
target_compile_definitions(${PROJECT_NAME} PUBLIC NORCSID=1 PUBLIC AAL=1)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} emheaders object print string system)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/aal.1 DESTINATION share/man/man1 OPTIONAL)



add_executable(arch archiver.c)
target_compile_definitions(arch PUBLIC NORCSID=1)
target_include_directories(arch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(arch emheaders object print string system)

install(TARGETS arch RUNTIME DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/arch.1 DESTINATION share/man/man1 OPTIONAL)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/arch.5 DESTINATION share/man/man5 OPTIONAL)

###############################################
# Generate and install documentation
###############################################     

if(${GENERATE_DOCS})
include(${ACK_SRC_HOME}/first/gendoc.cmake)
gendoc(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/aal.1 man aal1)
gendoc(arch ${CMAKE_CURRENT_SOURCE_DIR}/arch.1 man arch1)
gendoc(arch ${CMAKE_CURRENT_SOURCE_DIR}/arch.5 man arch_format)
endif(${GENERATE_DOCS})
