###################################################################
# Function to generate and install documentation in different
# printable and readable formats. 
#
# This function searches for troff, groff if not found will
# cause an error. Also searches for ps2pdf to convert to PDF.
#
# It takes the following arguments: 
#  TARGET The target that will be associated with this documentation
#  INPUTNAME which is the name of the troff file to convert (full path).
#  MACRO which is the name of the troff macro package to use (man or ms).
#  OUTNAME The name of the output file (no full path)
# 
##################################################################     
function(gendoc TARGET INPUTNAME MACRO OUTNAME)

set(MAN_PAGE ${INPUTNAME})
set(DOC_PS ${CMAKE_CURRENT_BINARY_DIR}/${OUTNAME}.ps)
set(DOC_PDF ${CMAKE_CURRENT_BINARY_DIR}/${OUTNAME}.pdf)
set(DOC_HTML ${CMAKE_CURRENT_BINARY_DIR}/${OUTNAME}.xhtml)


find_program (GROFF NAMES groff troff)
find_program (PS2PDF ps2pdf)

if(GROFF)
message(STATUS "Documents for \"${PROJECT_NAME}\" will be generated.")
else(GROFF)
message(SEND_ERROR "groff tool not found to build \"${PROJECT_NAME}\"")
endif(GROFF)   


add_custom_target(${TARGET}_GENDOC_${OUTNAME} DEPENDS ${DOC_PS} ${DOC_HTML} ${DOC_PDF})
add_dependencies(${TARGET} ${TARGET}_GENDOC_${OUTNAME})

add_custom_command(OUTPUT ${DOC_PS}
        COMMAND ${GROFF} -m ${MACRO} -I${CMAKE_CURRENT_SOURCE_DIR} -R -p -t -Tps ${MAN_PAGE} >${CMAKE_CURRENT_BINARY_DIR}/${OUTNAME}.ps
#        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${MAN_PAGE}
        COMMENT "Generating postscript with groff"
        )

add_custom_command(OUTPUT ${DOC_HTML}
        COMMAND ${GROFF} -m ${MACRO} -I${CMAKE_CURRENT_SOURCE_DIR} -R -p -t -Txhtml ${MAN_PAGE} >${CMAKE_CURRENT_BINARY_DIR}/${OUTNAME}.xhtml
#        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${MAN_PAGE}
        COMMENT "Generating xhtml with groff"
        )
        
        
install(FILES ${DOC_HTML} DESTINATION doc/${PROJECT_NAME}/xhtml
          COMPONENT Documentation
         OPTIONAL)        
install(FILES ${DOC_PS} DESTINATION doc/${PROJECT_NAME}/ps
          COMPONENT Documentation
         OPTIONAL)        

if(PS2PDF)

add_custom_command(OUTPUT ${DOC_PDF}
        COMMAND ${PS2PDF} ${DOC_PS} ${DOC_PDF}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${DOC_PS}
        COMMENT "Generating PDF with ps2pdf"
        )


install(FILES ${DOC_PDF} DESTINATION doc/${PROJECT_NAME}/pdf
          COMPONENT Documentation
         OPTIONAL)        
endif(PS2PDF)         


endfunction(gendoc)