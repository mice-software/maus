#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "G4analysis" for configuration "Release"
set_property(TARGET G4analysis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4analysis PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4global;G4intercoms"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4analysis.so"
  IMPORTED_SONAME_RELEASE "libG4analysis.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4analysis )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4analysis "${_IMPORT_PREFIX}/lib64/libG4analysis.so" )

# Import target "G4digits_hits" for configuration "Release"
set_property(TARGET G4digits_hits APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4digits_hits PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4geometry;G4global;G4intercoms;G4materials;G4particles;G4track;G4graphics_reps"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4digits_hits.so"
  IMPORTED_SONAME_RELEASE "libG4digits_hits.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4digits_hits )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4digits_hits "${_IMPORT_PREFIX}/lib64/libG4digits_hits.so" )

# Import target "G4error_propagation" for configuration "Release"
set_property(TARGET G4error_propagation APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4error_propagation PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4digits_hits;G4event;G4geometry;G4global;G4intercoms;G4materials;G4particles;G4processes;G4run;G4track;G4tracking"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4error_propagation.so"
  IMPORTED_SONAME_RELEASE "libG4error_propagation.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4error_propagation )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4error_propagation "${_IMPORT_PREFIX}/lib64/libG4error_propagation.so" )

# Import target "G4event" for configuration "Release"
set_property(TARGET G4event APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4event PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4digits_hits;G4geometry;G4global;G4graphics_reps;G4intercoms;G4materials;G4particles;G4processes;G4track;G4tracking"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4event.so"
  IMPORTED_SONAME_RELEASE "libG4event.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4event )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4event "${_IMPORT_PREFIX}/lib64/libG4event.so" )

# Import target "G4clhep" for configuration "Release"
set_property(TARGET G4clhep APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4clhep PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4clhep.so"
  IMPORTED_SONAME_RELEASE "libG4clhep.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4clhep )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4clhep "${_IMPORT_PREFIX}/lib64/libG4clhep.so" )

# Import target "G4geometry" for configuration "Release"
set_property(TARGET G4geometry APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4geometry PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4global;G4graphics_reps;G4intercoms;G4materials"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4geometry.so"
  IMPORTED_SONAME_RELEASE "libG4geometry.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4geometry )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4geometry "${_IMPORT_PREFIX}/lib64/libG4geometry.so" )

# Import target "G4global" for configuration "Release"
set_property(TARGET G4global APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4global PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4clhep"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4global.so"
  IMPORTED_SONAME_RELEASE "libG4global.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4global )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4global "${_IMPORT_PREFIX}/lib64/libG4global.so" )

# Import target "G4graphics_reps" for configuration "Release"
set_property(TARGET G4graphics_reps APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4graphics_reps PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4global;G4intercoms"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4graphics_reps.so"
  IMPORTED_SONAME_RELEASE "libG4graphics_reps.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4graphics_reps )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4graphics_reps "${_IMPORT_PREFIX}/lib64/libG4graphics_reps.so" )

# Import target "G4intercoms" for configuration "Release"
set_property(TARGET G4intercoms APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4intercoms PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4global"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4intercoms.so"
  IMPORTED_SONAME_RELEASE "libG4intercoms.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4intercoms )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4intercoms "${_IMPORT_PREFIX}/lib64/libG4intercoms.so" )

# Import target "G4interfaces" for configuration "Release"
set_property(TARGET G4interfaces APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4interfaces PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4global;G4intercoms"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4interfaces.so"
  IMPORTED_SONAME_RELEASE "libG4interfaces.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4interfaces )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4interfaces "${_IMPORT_PREFIX}/lib64/libG4interfaces.so" )

# Import target "G4materials" for configuration "Release"
set_property(TARGET G4materials APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4materials PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4global;G4intercoms"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4materials.so"
  IMPORTED_SONAME_RELEASE "libG4materials.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4materials )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4materials "${_IMPORT_PREFIX}/lib64/libG4materials.so" )

# Import target "G4parmodels" for configuration "Release"
set_property(TARGET G4parmodels APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4parmodels PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4digits_hits;G4geometry;G4global;G4graphics_reps;G4intercoms;G4materials;G4particles;G4processes;G4track"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4parmodels.so"
  IMPORTED_SONAME_RELEASE "libG4parmodels.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4parmodels )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4parmodels "${_IMPORT_PREFIX}/lib64/libG4parmodels.so" )

# Import target "G4particles" for configuration "Release"
set_property(TARGET G4particles APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4particles PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4global;G4materials;G4geometry;G4intercoms"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4particles.so"
  IMPORTED_SONAME_RELEASE "libG4particles.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4particles )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4particles "${_IMPORT_PREFIX}/lib64/libG4particles.so" )

# Import target "G4persistency" for configuration "Release"
set_property(TARGET G4persistency APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4persistency PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4geometry;G4global;G4graphics_reps;G4intercoms;G4materials;G4particles;G4digits_hits;G4event;G4processes;G4run;G4track;G4tracking;/nustorm/data2/lib/mausRecent/merge/third_party/install/lib/libxerces-c.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4persistency.so"
  IMPORTED_SONAME_RELEASE "libG4persistency.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4persistency )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4persistency "${_IMPORT_PREFIX}/lib64/libG4persistency.so" )

# Import target "G4physicslists" for configuration "Release"
set_property(TARGET G4physicslists APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4physicslists PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4geometry;G4global;G4intercoms;G4materials;G4particles;G4processes;G4run;G4track;G4digits_hits;G4event;G4tracking"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4physicslists.so"
  IMPORTED_SONAME_RELEASE "libG4physicslists.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4physicslists )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4physicslists "${_IMPORT_PREFIX}/lib64/libG4physicslists.so" )

# Import target "G4processes" for configuration "Release"
set_property(TARGET G4processes APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4processes PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4digits_hits;G4geometry;G4global;G4intercoms;G4materials;G4particles;G4track;/nustorm/data2/lib/mausRecent/merge/third_party/install/lib/libexpat.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4processes.so"
  IMPORTED_SONAME_RELEASE "libG4processes.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4processes )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4processes "${_IMPORT_PREFIX}/lib64/libG4processes.so" )

# Import target "G4readout" for configuration "Release"
set_property(TARGET G4readout APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4readout PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4digits_hits;G4event;G4geometry;G4global;G4intercoms;G4materials;G4particles;G4processes;G4run;G4track;G4tracking"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4readout.so"
  IMPORTED_SONAME_RELEASE "libG4readout.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4readout )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4readout "${_IMPORT_PREFIX}/lib64/libG4readout.so" )

# Import target "G4run" for configuration "Release"
set_property(TARGET G4run APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4run PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4digits_hits;G4event;G4geometry;G4global;G4graphics_reps;G4intercoms;G4materials;G4particles;G4processes;G4track;G4tracking"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4run.so"
  IMPORTED_SONAME_RELEASE "libG4run.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4run )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4run "${_IMPORT_PREFIX}/lib64/libG4run.so" )

# Import target "G4track" for configuration "Release"
set_property(TARGET G4track APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4track PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4geometry;G4global;G4intercoms;G4materials;G4particles"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4track.so"
  IMPORTED_SONAME_RELEASE "libG4track.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4track )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4track "${_IMPORT_PREFIX}/lib64/libG4track.so" )

# Import target "G4tracking" for configuration "Release"
set_property(TARGET G4tracking APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4tracking PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4digits_hits;G4geometry;G4global;G4graphics_reps;G4intercoms;G4materials;G4particles;G4processes;G4track"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4tracking.so"
  IMPORTED_SONAME_RELEASE "libG4tracking.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4tracking )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4tracking "${_IMPORT_PREFIX}/lib64/libG4tracking.so" )

# Import target "G4FR" for configuration "Release"
set_property(TARGET G4FR APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4FR PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4digits_hits;G4geometry;G4global;G4graphics_reps;G4intercoms;G4modeling;G4vis_management"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4FR.so"
  IMPORTED_SONAME_RELEASE "libG4FR.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4FR )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4FR "${_IMPORT_PREFIX}/lib64/libG4FR.so" )

# Import target "G4visHepRep" for configuration "Release"
set_property(TARGET G4visHepRep APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4visHepRep PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4digits_hits;G4geometry;G4global;G4graphics_reps;G4intercoms;G4materials;G4modeling;G4tracking;G4vis_management;G4zlib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4visHepRep.so"
  IMPORTED_SONAME_RELEASE "libG4visHepRep.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4visHepRep )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4visHepRep "${_IMPORT_PREFIX}/lib64/libG4visHepRep.so" )

# Import target "G4RayTracer" for configuration "Release"
set_property(TARGET G4RayTracer APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4RayTracer PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4digits_hits;G4event;G4geometry;G4global;G4graphics_reps;G4intercoms;G4materials;G4modeling;G4particles;G4processes;G4track;G4tracking;G4vis_management"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4RayTracer.so"
  IMPORTED_SONAME_RELEASE "libG4RayTracer.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4RayTracer )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4RayTracer "${_IMPORT_PREFIX}/lib64/libG4RayTracer.so" )

# Import target "G4Tree" for configuration "Release"
set_property(TARGET G4Tree APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4Tree PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4digits_hits;G4geometry;G4global;G4graphics_reps;G4intercoms;G4materials;G4modeling;G4particles;G4track;G4vis_management"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4Tree.so"
  IMPORTED_SONAME_RELEASE "libG4Tree.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4Tree )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4Tree "${_IMPORT_PREFIX}/lib64/libG4Tree.so" )

# Import target "G4VRML" for configuration "Release"
set_property(TARGET G4VRML APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4VRML PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4digits_hits;G4geometry;G4global;G4graphics_reps;G4intercoms;G4modeling;G4vis_management"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4VRML.so"
  IMPORTED_SONAME_RELEASE "libG4VRML.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4VRML )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4VRML "${_IMPORT_PREFIX}/lib64/libG4VRML.so" )

# Import target "G4visXXX" for configuration "Release"
set_property(TARGET G4visXXX APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4visXXX PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4digits_hits;G4geometry;G4global;G4graphics_reps;G4intercoms;G4modeling;G4tracking;G4vis_management"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4visXXX.so"
  IMPORTED_SONAME_RELEASE "libG4visXXX.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4visXXX )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4visXXX "${_IMPORT_PREFIX}/lib64/libG4visXXX.so" )

# Import target "G4zlib" for configuration "Release"
set_property(TARGET G4zlib APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4zlib PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4zlib.so"
  IMPORTED_SONAME_RELEASE "libG4zlib.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4zlib )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4zlib "${_IMPORT_PREFIX}/lib64/libG4zlib.so" )

# Import target "G4GMocren" for configuration "Release"
set_property(TARGET G4GMocren APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4GMocren PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4FR;G4digits_hits;G4event;G4geometry;G4global;G4graphics_reps;G4intercoms;G4materials;G4modeling;G4particles;G4tracking;G4vis_management"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4GMocren.so"
  IMPORTED_SONAME_RELEASE "libG4GMocren.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4GMocren )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4GMocren "${_IMPORT_PREFIX}/lib64/libG4GMocren.so" )

# Import target "G4vis_management" for configuration "Release"
set_property(TARGET G4vis_management APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4vis_management PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4digits_hits;G4event;G4geometry;G4global;G4graphics_reps;G4intercoms;G4materials;G4modeling;G4particles;G4processes;G4run;G4track;G4tracking"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4vis_management.so"
  IMPORTED_SONAME_RELEASE "libG4vis_management.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4vis_management )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4vis_management "${_IMPORT_PREFIX}/lib64/libG4vis_management.so" )

# Import target "G4modeling" for configuration "Release"
set_property(TARGET G4modeling APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(G4modeling PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4digits_hits;G4event;G4geometry;G4global;G4graphics_reps;G4intercoms;G4materials;G4particles;G4processes;G4track;G4tracking"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libG4modeling.so"
  IMPORTED_SONAME_RELEASE "libG4modeling.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS G4modeling )
list(APPEND _IMPORT_CHECK_FILES_FOR_G4modeling "${_IMPORT_PREFIX}/lib64/libG4modeling.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
