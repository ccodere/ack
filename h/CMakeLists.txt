cmake_minimum_required (VERSION 3.0)
project(emheaders)

# Create an include only library to be used
# for easier dependency management.

if(NOT ACK_SRC_HOME)
message(SEND_ERROR "ACK_SRC_HOME variable not defined")
endif(NOT ACK_SRC_HOME)

if(NOT ACK_DATA_PATH)
message(SEND_ERROR "ACK_DATA_PATH variable not defined")
endif(NOT ACK_DATA_PATH)

if(NOT DEFAULT_PLATFORM)
message(SEND_ERROR "DEFAULT_PLATFORM variable not defined")
endif(NOT DEFAULT_PLATFORM)


configure_file(em_path.h.in ${CMAKE_CURRENT_BINARY_DIR}/em_path.h)

# Created when generating the CMake file.
file(WRITE  ${CMAKE_CURRENT_BINARY_DIR}/local.h "#define VERSION 3\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/local.h "#define ACKM \"${DEFAULT_PLATFORM}\"\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/local.h "#define BIGMACHINE 1\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/local.h "#define SYS_5\n")


add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION h
)

set(HEADERS
	arch.h
	as_spec.h
	cg_pattern.h
	cgg_cg.h
	em_abs.h
	em_ego.h
	em_flag.h
	em_mes.h
	em_reg.h
	ip_spec.h
	out.h
	ranlib.h
	stb.h
)

#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/em_path.h DESTINATION config OPTIONAL)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/local.h DESTINATION config OPTIONAL)

