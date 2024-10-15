cmake_minimum_required (VERSION 3.0)
project(em_ass)

set(SRC_C
	ass00.c
	ass00.h
	ass30.c
	ass40.c
	ass50.c
	ass60.c
	ass70.c
	ass80.c
	assci.c
	assci.h
	asscm.c
	asscm.h
	assda.c
	assex.h
	assrl.c
	assrl.h
)


set(GEN_C
	${CMAKE_CURRENT_BINARY_DIR}/asstb.c
)


add_custom_command(
   OUTPUT asstb.c
   COMMAND ${CMAKE_CURRENT_BINARY_DIR}/maktab ${CMAKE_CURRENT_SOURCE_DIR}/../../etc/ip_spec.t>asstb.c
   DEPENDS maktab
)     

add_executable(maktab maktab.c)
target_link_libraries(maktab emh emheaders em_data)


add_executable(${PROJECT_NAME} ${SRC_C} ${GEN_C})
target_compile_definitions(${PROJECT_NAME} PUBLIC NORCSID=1)
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME} emheaders emh alloc system em_data)


install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION lib/ack)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/em_ass.6 DESTINATION share/man/man6 OPTIONAL)