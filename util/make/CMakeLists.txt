cmake_minimum_required (VERSION 2.6)
project (amake)
include(CheckIncludeFile)


set(SRC 
 check.c
 input.c
 macro.c
 make.c
 main.c
 reader.c
 rules.c
 h.h
)

# The following should be added if utime.h and unistd.h are
# available.
check_include_file("utime.h" HAS_UTIME_H)
check_include_file("sys/stat.h" HAS_STAT_H)

if(NOT HAS_STAT_H)
message(FATAL_ERROR "Building make require access to stat() POSIX API")
endif(NOT HAS_STAT_H)

add_executable(${PROJECT_NAME} ${SRC})

if(HAS_UTIME_H)
target_compile_definitions(${PROJECT_NAME} PRIVATE unix)
endif(HAS_UTIME_H)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

