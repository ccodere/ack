cmake_minimum_required (VERSION 3.0)
project(input)

# Create an include only library to be used
# for easier dependency management.

SET(PUBLIC_H
	inp_pkg.body
	inp_pkg.spec
)
	


add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
#set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${PUBLIC_H}")


install(TARGETS ${PROJECT_NAME} 
        LIBRARY DESTINATION modules/lib
        ARCHIVE DESTINATION modules/lib
        PUBLIC_HEADER DESTINATION modules/input
		COMPONENT Development)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/input.3 DESTINATION share/man/man3
		COMPONENT Development
		OPTIONAL)
