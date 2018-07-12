find_package(Sphinx)
find_package(LATEX)

if(SPHINX_FOUND)
    set(HTML_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/html")
    file(MAKE_DIRECTORY ${HTML_OUTPUT_DIR})
    add_custom_target(sphinx-html
                      COMMAND ${SPHINX_EXECUTABLE} -q -E -b html
                      -w sphinx-html.log
                      "${CMAKE_CURRENT_SOURCE_DIR}"
                      "${HTML_OUTPUT_DIR}"
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Building HTML documentation with Sphinx"
                      VERBATIM)

    set(LATEX_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/latex")
    file(MAKE_DIRECTORY ${LATEX_OUTPUT_DIR})
    add_custom_target(sphinx-latex
                      COMMAND ${SPHINX_EXECUTABLE} -q -E -b latex
                      -w sphinx-latex.log
                      "${CMAKE_CURRENT_SOURCE_DIR}"
                      "${LATEX_OUTPUT_DIR}"
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Building HTML documentation with Sphinx"
                      VERBATIM)

    if(LATEX_FOUND)
        set(PDF_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/pdf")
        file(MAKE_DIRECTORY ${PDF_OUTPUT_DIR})
        # 
        # Find the latex source name by parsing conf.py.
        file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/conf.py _tmp_string REGEX "^project.*")
        # Extract the name inside project = u'<NAME>'
        string(REGEX REPLACE "project = u'\(.*\)'" "\\1" SPHINX_PROJECT_NAME ${_tmp_string}) 
        # Execute latex command twice to get references correct
        add_custom_target(sphinx-pdf
                          COMMAND ${PDFLATEX_COMPILER} -output-directory ${PDF_OUTPUT_DIR} ${SPHINX_PROJECT_NAME}.tex
                          COMMAND ${PDFLATEX_COMPILER} -output-directory ${PDF_OUTPUT_DIR} ${SPHINX_PROJECT_NAME}.tex
                          DEPENDS sphinx-latex
                          WORKING_DIRECTORY ${LATEX_OUTPUT_DIR}
                          COMMENT "Building PDF documentation from Spinx Latex"
                          VERBATIM)
    endif(LATEX_FOUND)

endif(SPHINX_FOUND)


add_subdirectory(doxygen)
