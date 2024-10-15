cmake_minimum_required (VERSION 3.1)
project (data)

set(SRCS
 array.c
 array.h
 astring.c
 astring.h
 diagnostics.c
 diagnostics.h
 imap.c
 imap.h
 pmap.c
 pmap.h
 smap.c
 smap.h
 stringlist.c
 stringlist.h 
)



set(INCLUDE_DIRS
 ${CMAKE_CURRENT_SOURCE_DIR}
)



add_library(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "array.h;astring.h;diagnostics.h;imap.h;pmap.h;smap.h;stringlist.h")

install(TARGETS ${PROJECT_NAME} 
        LIBRARY DESTINATION modules/lib
        ARCHIVE DESTINATION modules/lib
        PUBLIC_HEADER DESTINATION modules/h
        COMPONENT Development
        OPTIONAL
)


