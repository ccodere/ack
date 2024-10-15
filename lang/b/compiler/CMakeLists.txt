cmake_minimum_required (VERSION 3.0)
project(em_b)


set(INCLUDE_DIRS
 .
)

add_executable(${PROJECT_NAME} b.h b0.c b1.c)
target_compile_definitions(${PROJECT_NAME} PUBLIC NORCSID=1)
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} emheaders emh data alloc print string system em_data emk em_mes)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION lib/ack)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/em_b.6 DESTINATION share/man/man6 OPTIONAL)