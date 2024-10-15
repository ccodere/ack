#######################################################
# This loops through the 'mach' subdirectories and 
# builds the different elements if they exist based
# on the rules defined in the proto/<eleement>/proto.cmake
# file. The ${ARCH} will be set to the machine name,
# and ${MACHINES} variable is used to set the machines
# that will be visited for building.
#######################################################
cmake_minimum_required (VERSION 3.0)
project(mach)


SET(MACHINES
   6500
   6800
   6805
   6809
   arm
   em22
   em24
   em44
   i80
   i86
   i386
   m68k2
   m68k4
   m68020
#   mips
   ns
   pdp
   powerpc
   s2650
   sparc
   vax4
   vc4
   z80
   z8000
)


message(STATUS "Machine dependent files...")

foreach(ARCH ${MACHINES})

## target optimizer 'top' ##
if(EXISTS ${ACK_SRC_HOME}/mach/${ARCH}/top)
message(STATUS "Generating 'top' CMakeLists.txt for ${ARCH}")

include(${ACK_SRC_HOME}/mach/proto/top/proto.cmake)

endif(EXISTS ${ACK_SRC_HOME}/mach/${ARCH}/top)
############################

## assembler 'as' ##
if(EXISTS ${ACK_SRC_HOME}/mach/${ARCH}/as)
message(STATUS "Generating 'as' CMakeLists.txt for ${ARCH}")

include(${ACK_SRC_HOME}/mach/proto/as/proto.cmake)

endif(EXISTS ${ACK_SRC_HOME}/mach/${ARCH}/as)
############################


## code generaror 'cg' ##
if(EXISTS ${ACK_SRC_HOME}/mach/${ARCH}/cg/table)
message(STATUS "Generating 'cg' CMakeLists.txt for ${ARCH}")

include(${ACK_SRC_HOME}/mach/proto/cg/proto.cmake)

endif(EXISTS ${ACK_SRC_HOME}/mach/${ARCH}/cg/table)
############################


## code generaror 'ncg' ##
if(EXISTS ${ACK_SRC_HOME}/mach/${ARCH}/ncg/table)
message(STATUS "Generating 'ncg' CMakeLists.txt for ${ARCH}")

include(${ACK_SRC_HOME}/mach/proto/ncg/proto.cmake)

endif(EXISTS ${ACK_SRC_HOME}/mach/${ARCH}/ncg/table)
############################


## code generaror 'mcg' ##
if(EXISTS ${ACK_SRC_HOME}/mach/${ARCH}/mcg)
message(STATUS "Generating 'mcg' CMakeLists.txt for ${ARCH}")

include(${ACK_SRC_HOME}/mach/proto/mcg/proto.cmake)

endif(EXISTS ${ACK_SRC_HOME}/mach/${ARCH}/mcg)
############################


endforeach(ARCH)

