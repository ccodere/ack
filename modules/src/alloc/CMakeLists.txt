cmake_minimum_required (VERSION 3.0)
project (alloc)

set(SRC 
 botch.c 
 clear.c 
 Malloc.c 
 No_Mem.c 
 Realloc.c 
 Salloc.c
 Srealloc.c
 std_alloc.c
 st_alloc.c
 alloc.h
)


add_library(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "alloc.h")

install(TARGETS ${PROJECT_NAME} 
        LIBRARY DESTINATION modules/lib
        ARCHIVE DESTINATION modules/lib
        PUBLIC_HEADER DESTINATION modules/h
        COMPONENT Development
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/alloc.3 DESTINATION share/man/man3
       COMPONENT Development)

