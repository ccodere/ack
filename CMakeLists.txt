cmake_minimum_required (VERSION 3.1)

enable_testing()

# Watch out on the order of the building, some modules and libraries 
# must be built before others.

# if (MSVC)
#	add_compile_options(/W3 /WX)
# else()
# 	add_compile_options(-W -Wall -Werror)
# endif()


option(USE_C90_LIBS_ONLY "ANSI C90 library only" ON)
option(GENERATE_DOCS "Generate documentation" ON)
set(DEFAULT_PLATFORM em22 CACHE string "Set default target")
set(ACK_SRC_HOME ${CMAKE_CURRENT_SOURCE_DIR})
set(ACK_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(ACK_HOME ${CMAKE_INSTALL_PREFIX})
set(ACK_DATA_PATH ${ACK_HOME}/share/ack)


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/h)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/h)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/alloc)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/string)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/print)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/flt_arith)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/data)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/idf)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/input)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/system)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/em_data)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/em_code)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/em_mes)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/object)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/src/read_em)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/make)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/arch)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/amisc)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/cmisc)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/LLgen)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/misc)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/topgen)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/ass)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/int)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/led)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/ego/share)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/ego/ra)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/ego)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/opt)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/cgg)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/ncgg)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/mcgg)




# Compilers and runtime libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lang/basic/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lang/pc/comp)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lang/m2/comp)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lang/cem/cemcom.ansi)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lang/cem/cpp.ansi)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lang/b/compiler)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/fast/driver)


find_program (YACC yacc)

if(NOT YACC)
message(SEND_ERROR "yacc tool not found to build machine assembly program")
endif(NOT YACC)

# Compilers / assemblers / linkers
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/6500/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/6800/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/6805/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/6809/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/arm/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/i386/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/i80/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/i86/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/m68020/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/m68k2/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/mips/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/ns/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/pdp/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/powerpc/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/s2650/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/vax4/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/vc4/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/z80/as)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/z8000/as)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach)

# ACK front-end system
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util/ack)


# CPU Specific code generators
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mach/6500/cg)

####################################################################
# All platform specific tools and libraries are built here.
# All build tools and modules should have been built before here.
####################################################################
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lang/cem/libcc.ansi)


#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/plat)


