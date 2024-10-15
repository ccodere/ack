cmake_minimum_required (VERSION 3.0)
project(em_led)

set(SRC_C
	archive.c
	archive.h
	const.h
	debug.h
	defs.h
	error.c
	error.h
	extract.c
	extract.h
	finish.c
	finish.h
	mach.h
	main.c
	memory.c
	memory.h
	orig.h
	output.c
	output.h
	read.c
	relocate.c
	relocate.h
	save.c
	save.h
	scan.c
	scan.h
	sym.c
	sym.h
	write.c
	write.h
)


add_executable(${PROJECT_NAME} ${SRC_C})
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME} emheaders object string)


install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION lib/ack)


install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/led.6 DESTINATION share/man/man6 OPTIONAL RENAME em_led.6)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ack.out.5 DESTINATION share/man/man5 OPTIONAL)