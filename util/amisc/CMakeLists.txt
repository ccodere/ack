cmake_minimum_required (VERSION 2.9)
project (amisc)

set(SRC_MODULES abmodules.c)
set(SRC_ELFLOD aelflod.c)
set(SRC_ANM anm.c)
set(SRC_ASHOW ashow.c)
set(SRC_ASIZE asize.c)
set(SRC_ASLOD aslod.c)
set(SRC_ASTRIP astrip.c)




add_executable(anm ${SRC_ANM})
add_executable(ashow ${SRC_ASHOW})
add_executable(asize ${SRC_ASIZE})
add_executable(astrip ${SRC_ASTRIP})
add_executable(aslod ${SRC_ASLOD})


include_directories(${INCLUDE_DIRS})

# Set linking options
#target_link_libraries(ack-modules emheaders object)
target_link_libraries(anm emheaders object)
target_link_libraries(ashow emheaders object)
target_link_libraries(asize emheaders object)
target_link_libraries(astrip emheaders object system)
target_link_libraries(aslod emheaders object system)

install(TARGETS anm RUNTIME DESTINATION bin)
install(TARGETS ashow RUNTIME DESTINATION bin)
install(TARGETS asize RUNTIME DESTINATION bin)
install(TARGETS astrip RUNTIME DESTINATION bin)
install(TARGETS aslod RUNTIME DESTINATION bin)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/anm.1 DESTINATION share/man/man1 OPTIONAL)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ashow.1 DESTINATION share/man/man1 OPTIONAL)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/asize.1 DESTINATION share/man/man1 OPTIONAL)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/astrip.1 DESTINATION share/man/man1 OPTIONAL)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/aslod.1 DESTINATION share/man/man1 OPTIONAL)

###############################################
# Generate and install documentation
###############################################     

if(${GENERATE_DOCS})
include(${ACK_SRC_HOME}/first/gendoc.cmake)
gendoc(anm ${CMAKE_CURRENT_SOURCE_DIR}/anm.1 man anm1)
gendoc(ashow ${CMAKE_CURRENT_SOURCE_DIR}/ashow.1 man ashow1)
gendoc(asize ${CMAKE_CURRENT_SOURCE_DIR}/asize.1 man asize1)
gendoc(astrip ${CMAKE_CURRENT_SOURCE_DIR}/astrip.1 man astrip1)
gendoc(aslod ${CMAKE_CURRENT_SOURCE_DIR}/aslod.1 man aslod1)
endif(${GENERATE_DOCS})
