cmake_minimum_required (VERSION 2.9)
project (cmisc)


add_executable(tabgen tabgen.c)
target_compile_definitions(tabgen PUBLIC NORCSID=1)

add_executable(cid cid.c GCIPM.c)
target_compile_definitions(cid PUBLIC NORCSID=1)

add_executable(cclash cclash.c GCIPM.c)
target_compile_definitions(cclash PUBLIC NORCSID=1)

add_executable(prid prid.c GCIPM.c)
target_compile_definitions(prid PUBLIC NORCSID=1)

add_executable(mkdep mkdep.c)
target_compile_definitions(prid PUBLIC NORCSID=1)



install(TARGETS tabgen cid cclash prid mkdep RUNTIME DESTINATION bin OPTIONAL)
install(FILES 
 ${CMAKE_CURRENT_SOURCE_DIR}/tabgen.1
 ${CMAKE_CURRENT_SOURCE_DIR}/cid.1
 ${CMAKE_CURRENT_SOURCE_DIR}/cclash.1
 ${CMAKE_CURRENT_SOURCE_DIR}/mkdep.1
 ${CMAKE_CURRENT_SOURCE_DIR}/prid.1
 DESTINATION share/man/man1 OPTIONAL)

###############################################
# Generate and install documentation
###############################################     

if(${GENERATE_DOCS})
include(${ACK_SRC_HOME}/first/gendoc.cmake)
gendoc(tabgen ${CMAKE_CURRENT_SOURCE_DIR}/tabgen.1 man tabgen)
gendoc(cid ${CMAKE_CURRENT_SOURCE_DIR}/cid.1 man cid)
gendoc(cclash ${CMAKE_CURRENT_SOURCE_DIR}/cclash.1 man cclash)
gendoc(mkdep ${CMAKE_CURRENT_SOURCE_DIR}/mkdep.1 man mkdep)
gendoc(prid ${CMAKE_CURRENT_SOURCE_DIR}/prid.1 man prid)
endif(${GENERATE_DOCS})
