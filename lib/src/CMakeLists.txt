

ot_add_current_dir_to_include_dirs ()

ot_add_source_file ( AdaptiveDirectionalSampling.cxx )
ot_add_source_file ( QuadrantSampling.cxx )

ot_install_header_file ( AdaptiveDirectionalSampling.hxx )
ot_install_header_file ( QuadrantSampling.hxx )


include_directories ( ${INTERNAL_INCLUDE_DIRS} )

add_library ( otads ${SOURCEFILES} )
if ( BUILD_SHARED_LIBS )
  set_target_properties ( otads PROPERTIES COMPILE_DEFINITIONS "OTADS_DLL_EXPORTS" )
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties ( otads PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties ( otads PROPERTIES SOVERSION ${LIB_SOVERSION} )
target_link_libraries (otads ${OPENTURNS_LIBRARY})

# Add targets to the build-tree export set
export (TARGETS otads FILE ${PROJECT_BINARY_DIR}/OTADS-Targets.cmake)

# Install the export set for use with the install-tree
install(EXPORT OTADS-Targets
        DESTINATION "${OTADS_CONFIG_CMAKE_PATH}"
        COMPONENT Development)

install(TARGETS otads
        EXPORT  OTADS-Targets 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${OTADS_LIBRARY_PATH}
        ARCHIVE DESTINATION ${OTADS_LIBRARY_PATH}
)

