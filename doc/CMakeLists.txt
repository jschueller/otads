
find_package ( LATEX )
set ( IMAGEMAGICK_CONVERT convert )
set ( LATEX2HTML_CONVERTER latex2html )
include ( UseLATEX )

# latex part 
if ( PDFLATEX_COMPILER AND BIBTEX_COMPILER )

  if ( ${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR} )
    # in-source build
    set ( LATEX_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build )
#     execute_process (
#       COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/src/logoOpenTURNS.jpg ${CMAKE_CURRENT_SOURCE_DIR}/logoOpenTURNS.jpg
#     )  
  else ()
    # out-of-source build
    set ( LATEX_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
  endif ()
  add_latex_document ( OTADS_Documentation.tex
    INPUTS GenericInformation.tex.in
    OTADS_summary.tex
    OTADS_UserManual.tex
    Math_Notations.sty
    OTADS_ExamplesGuide.tex
    OTADS_title.tex
    OTADS_Documentation.sty
    OTADS_ReferenceGuide.tex
    OTADS_Validation.tex
    OTADS_UseCasesGuide.tex
    OTADS_Developer.tex
    IMAGES logoOTADS.png architecture.png
    DEFAULT_PDF
    MANGLE_TARGET_NAMES
  ) 

  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/GenericInformation.tex.in 
                  ${LATEX_OUTPUT_PATH}/GenericInformation.tex
                  @ONLY
  )
  
#   execute_process (
#     COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/latex2html/latex2html.sty ${LATEX_OUTPUT_PATH}/latex2html.sty
#     COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/src/Math_Notations.sty ${LATEX_OUTPUT_PATH}/Math_Notations.sty
#     COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/src/GenericInformation.tex ${LATEX_OUTPUT_PATH}/GenericInformation.tex
#   )     
  install ( FILES ${LATEX_OUTPUT_PATH}/OTADS_Documentation.pdf DESTINATION ${DOC_INSTALL_PATH}/pdf )

endif ()
