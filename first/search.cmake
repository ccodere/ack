######################################################
# This function is used to search for specified source
# files in different directories, and if the source
# is not found in any of the directories, outputs
# an error. It returns a list of source files
# with the resolved paths as well as a list of 
# include directories
# 
# The syntax is as follows:
#  search_src(SOURCES source... 
#    PATHS path1 path2... 
#    TARGET <var> 
#    INCLUDES <var>
#
#  The SOURCES parameters indicates the list of 
#  source files to search for in PATHS.
#
# OUT_INCLUDES_LIST = Additional include directories
# OUT_SRC_LIST = Additional source list
#####################################################
function(search_src)


set(multiValueArgs SOURCES PATHS TARGET INCLUDES)

cmake_parse_arguments(arg "" "" "${multiValueArgs}" ${ARGN})

#message(STATUS "SOURCES: ${arg_SOURCES}")
#message(STATUS "PATHS: ${arg_PATHS}")

foreach(SRC_FILE ${arg_SOURCES})



# For each path, search if the source file
# exists in that directory
unset(FOUND_FILE)
foreach(SRC_PATH ${arg_PATHS})

if(EXISTS ${SRC_PATH}/${SRC_FILE})

   get_filename_component(SRC_FULL_PATH ${SRC_PATH}/${SRC_FILE}
                        ABSOLUTE)
   get_filename_component(SRC_DIRECTORY_PATH ${SRC_FULL_PATH}
                        DIRECTORY)
   # Add this directory as include directory	                        
   list(APPEND arg_INCLUDES ${SRC_DIRECTORY_PATH})
   # Add this source file
   list(APPEND arg_TARGET ${SRC_FULL_PATH})
   message(STATUS "${SRC_FILE} found in ${SRC_FULL_PATH}")
   set(FOUND_FILE 1)
   break()
endif()


endforeach(SRC_PATH)

if(NOT DEFINED FOUND_FILE)
    message(SEND_ERROR "file : ${SRC_FILE} not found.")
endif()

endforeach(SRC_FILE)

list(REMOVE_DUPLICATES arg_INCLUDES)

set(OUT_SRC_LIST ${arg_TARGET} PARENT_SCOPE)
set(OUT_INCLUDES_LIST ${arg_INCLUDES} PARENT_SCOPE)

endfunction(search_src)
