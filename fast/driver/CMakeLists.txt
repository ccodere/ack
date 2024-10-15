cmake_minimum_required (VERSION 3.0)
project(fast)

add_executable(afcc driver.c)
target_compile_definitions(afcc PUBLIC NORCSID=1 PUBLIC FCC=1)
target_link_libraries(afcc emheaders system)

add_executable(afm2 driver.c)
target_compile_definitions(afm2 PUBLIC NORCSID=1 PUBLIC FM2=1)
target_link_libraries(afm2 emheaders system)

add_executable(afpc driver.c)
target_compile_definitions(afpc PUBLIC NORCSID=1 PUBLIC FPC=1)
target_link_libraries(afpc emheaders system)



install(TARGETS afcc afm2 afpc 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/afcc.1 ${CMAKE_CURRENT_SOURCE_DIR}/afm2.1 ${CMAKE_CURRENT_SOURCE_DIR}/afpc.1 DESTINATION share/man/man1 OPTIONAL)

###############################################
# Generate and install documentation
###############################################     

if(${GENERATE_DOCS})
include(${ACK_SRC_HOME}/first/gendoc.cmake)
gendoc(afcc ${CMAKE_CURRENT_SOURCE_DIR}/afcc.1 man afcc)
gendoc(afm2 ${CMAKE_CURRENT_SOURCE_DIR}/afm2.1 man afm2)
gendoc(afpc ${CMAKE_CURRENT_SOURCE_DIR}/afpc.1 man afpc)
endif(${GENERATE_DOCS})
