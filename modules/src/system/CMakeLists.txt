cmake_minimum_required (VERSION 2.9)
project(system)

set(SRC
 access.c
 basename.c
 break.c
 chmode.c
 close.c
 create.c
 filesize.c
 lock.c
 maketempfile.c
 modtime.c
 open.c
 read.c
 remove.c
 rename.c
 seek.c
 setbinarymode.c
 stop.c
 strndup.c
 syssystem.c
 system.c
 write.c
 tmpdir.c
 system.h
)

add_library(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "system.h")
target_link_libraries(${PROJECT_NAME} data)

install(TARGETS ${PROJECT_NAME} 
        LIBRARY DESTINATION modules/lib
        ARCHIVE DESTINATION modules/lib
        PUBLIC_HEADER DESTINATION modules/h
		COMPONENT Development)        

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/system.3 DESTINATION share/man/man3
		COMPONENT Development
		OPTIONAL)
		
		
