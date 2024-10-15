cmake_minimum_required (VERSION 3.0)
project(object)

set(SRC_C
 obj.h
 object.h
 rd_arhdr.c
 rd_bytes.c
 rd_int2.c
 rd_int4.c
 rd_ranlib.c
 rd_unsig2.c
 rd.c
 wr_arhdr.c
 wr_bytes.c
 wr_int2.c
 wr_int4.c
 wr_putc.c
 wr_ranlib.c
 wr.c	
)

set(INCLUDE_DIRS
  .
)


add_library(${PROJECT_NAME} ${SRC_C})
target_compile_definitions(${PROJECT_NAME} PUBLIC NORCSID=1)
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "object.h")
target_link_libraries(${PROJECT_NAME} emheaders)

install(TARGETS ${PROJECT_NAME} 
        LIBRARY DESTINATION modules/lib
        ARCHIVE DESTINATION modules/lib
        PUBLIC_HEADER DESTINATION modules/h
		COMPONENT Development
		OPTIONAL)
		        
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/object.3 DESTINATION share/man/man3
		COMPONENT Development
		OPTIONAL)
