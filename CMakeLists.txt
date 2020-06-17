cmake_minimum_required(VERSION 3.10)


################################################################################
# This Top-level project is for a single Top-level library
#
# A single library
# Unit tests for the library
# A set of executables which use this library
################################################################################



################################################################################
# Project name
#
#  many targets will be created:
#   myProject::warnings
#   myProject:
################################################################################
project( ugltf
        VERSION 1.0.0.0
        LANGUAGES CXX)

################################################################################



################################################################################
# If the conan file exists, include it
################################################################################
if( EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake )
    include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(NO_OUTPUT_DIRS TARGETS)
endif()
################################################################################



# Extra targets.  adds coverage::coverage, warning::all, warning:error
SET(PROJECT_TARGETS_PREFIX ${PROJECT_NAME})

include(.cmake/extras.cmake)

option( ${PROJECT_NAME}_ENABLE_COVERAGE "Enable Coverage. After build, execute make coverage"        TRUE)
option( ${PROJECT_NAME}_ENABLE_WARNINGS "Enable Strict Warnings" TRUE)



################################################################################
# Build the Library.
#  By default, the library name will be the ${PROJECT_NAME}
#
#  If you wish to add more libraries, make a copy of this section and
# change the values below.
################################################################################
set(outName              ${PROJECT_NAME})   # name of the library
set(srcFiles             "src/* include/ugltf/*")          # all the source files for this library
set(PublicLinkedTargets  CONAN_PKG::nlohmann_json )
set(PrivateLinkedTargets "" )
#-------------------------------------------------------------------------------
file(GLOB_RECURSE srcFiles ${srcFiles} )
add_library( ${outName} INTERFACE )
add_library( ${PROJECT_NAME}::${outName}  ALIAS ${outName} )

target_include_directories( ${outName}
                            INTERFACE
                               "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
 )

target_compile_features( ${outName}
                          INTERFACE
                              cxx_std_17)

target_compile_definitions( ${outName}
                                INTERFACE
                                TEST_DEFINE)

target_link_libraries( ${outName}  INTERFACE  ${PublicLinkedTargets}  )
target_link_libraries( ${outName}  INTERFACE ${PrivateLinkedTargets} )

if( ${PROJECT_NAME}_ENABLE_COVERAGE )
    target_link_libraries( ${outName}  INTERFACE   ${PROJECT_NAME}::coverage  )
endif()
if( ${PROJECT_NAME}_ENABLE_WARNINGS )
    target_link_libraries( ${outName}  INTERFACE   ${PROJECT_NAME}::warnings )
endif()

include(GNUInstallDirs)
install(
   TARGETS
       ${outName}
   LIBRARY  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
   ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
   RUNTIME  DESTINATION "${CMAKE_INSTALL_BINDIR}"
   INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  )

install(
    DIRECTORY   "include/ugltf"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "*"
)


  message("*****************************************************")
  message("LIBRARIES:")
  message("*****************************************************")
  message("   ${outName} ")
  message("*****************************************************")
################################################################################


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test")
    enable_testing()
    add_subdirectory(test)
endif()







#####################################################
# Macro get get all subdirectories
#####################################################
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()
#####################################################


# Loop through all the subdirectories in the /bin folder and
# compile each of the folders into an executable
SUBDIRLIST(BIN_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR}/bin)

message("*****************************************************")
message("EXECUTABLES:")
message("*****************************************************")
FOREACH(subdir ${BIN_FOLDERS})
    set(bin_name ${subdir} )
    file(GLOB_RECURSE srcFiles ${CMAKE_CURRENT_SOURCE_DIR}/bin/${subdir}/* )

    add_executable( ${bin_name} ${srcFiles} )
    target_link_libraries(${bin_name} PRIVATE ${PROJECT_NAME}::${PROJECT_NAME}
CONAN_PKG::lyra CONAN_PKG::stb )

    install(
       TARGETS
           ${bin_name}
       LIBRARY  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
       ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
       RUNTIME  DESTINATION "${CMAKE_INSTALL_BINDIR}"
       INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
      )

      message("    ${bin_name}")

ENDFOREACH()
message("*****************************************************")


#####################################################
## PACKAGE
#####################################################
SET(CPACK_PACKAGE_NAME          ${PROJECT_NAME})
SET(CPACK_PACKAGE_VENDOR        ${PROJECT_NAME})
SET(CPACK_PACKAGE_EXECUTABLES   ${PROJECT_NAME};${PROJECT_NAME})
set(CPACK_PACKAGING_INSTALL_PREFIX     ${CMAKE_INSTALL_PREFIX})
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${PROJECT_NAME}")
SET(CPACK_PACKAGE_VERSION       "${PROJECT_VERSION}")
SET(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
SET(CPACK_PACKAGE_CONTACT       "me")

INCLUDE(InstallRequiredSystemLibraries)

IF(WIN32 AND NOT UNIX)
  SET(CPACK_NSIS_INSTALLED_ICON_NAME   "bin\\\\${PROJECT_NAME}.exe")
  SET(CPACK_NSIS_DISPLAY_NAME          "${CPACK_PACKAGE_INSTALL_DIRECTORY}${PROJECT_NAME}")
  SET(CPACK_NSIS_MODIFY_PATH           ON)
ENDIF(WIN32 AND NOT UNIX)

INCLUDE(CPack)


return()
