cmake_minimum_required(VERSION 3.10)

project(ugltf VERSION 0.1)


#################################################################################################
# Build a coverage target
#
# Simply Copy-Paste this section into your CMakeLists.txt file somewhere after your
# call to project(name)
#################################################################################################

#----------------------------------------------------------------
# Always build the coverage target
#----------------------------------------------------------------
string( TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
add_library(${PROJECT_NAME}_coverage_lib INTERFACE)
add_library(${PROJECT_NAME}::coverage    ALIAS    ${PROJECT_NAME}_coverage_lib)
#----------------------------------------------------------------

if(CMAKE_COMPILER_IS_GNUCC)

    option(${PROJECT_NAME_UPPER}_BUILD_COVERAGE "Build with Coverage" ON)

    if( ${PROJECT_NAME_UPPER}_BUILD_COVERAGE )

        target_compile_options( ${PROJECT_NAME}_coverage_lib  INTERFACE --coverage -g -O0 -fprofile-arcs -ftest-coverage)
        target_link_libraries(  ${PROJECT_NAME}_coverage_lib  INTERFACE --coverage -g -O0 -fprofile-arcs -ftest-coverage)

        add_custom_target( ${PROJECT_NAME}_coverage
            COMMAND rm -rf coverage
            COMMAND mkdir -p coverage
            COMMAND ${CMAKE_MAKE_PROGRAM} test
            COMMAND gcovr . -r ${CMAKE_SOURCE_DIR} --html-details --html -o coverage/index.html -e ${CMAKE_SOURCE_DIR}/test -e ${CMAKE_BINARY_DIR};
            COMMAND gcovr . -r ${CMAKE_SOURCE_DIR} --xml                 -o coverage/report.xml -e ${CMAKE_SOURCE_DIR}/test -e ${CMAKE_BINARY_DIR};
            COMMAND gcovr . -r ${CMAKE_SOURCE_DIR}                       -o coverage/report.txt -e ${CMAKE_SOURCE_DIR}/test -e ${CMAKE_BINARY_DIR};
            COMMAND cat coverage/report.txt
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}  # Need separate command for this line
        )

        message("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" )
        message("COVERAGE TARGET BUILT                                                               " )
        message("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" )
        message("    Coverage Interface Library: ${PROJECT_NAME}::coverage" )
        message("    Coverage Build Target     : ${PROJECT_NAME}_coverage"  )
        message("    "  )
        message("    Any libraries/executables linked to ${PROJECT_NAME}::coverage will be built with"  )
        message("    coverage flags"  )
        message("    ")
        message("    eg: "  )
        message("     "  )
        message("    mkdir build && cd build && cmake .. "  )
        message("    make"  )
        message("    make ${PROJECT_NAME}_coverage"  )
        message("    xdg-open coverage/index.html"  )
        message("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" )

    endif()

endif()
#################################################################################################




#################################################################################################
# Build the actual interface library
#################################################################################################
add_library( ${PROJECT_NAME} INTERFACE )

add_library( ${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME} )

target_include_directories( ${PROJECT_NAME}
                            INTERFACE
                               "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                               "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
)


target_compile_features( ${PROJECT_NAME}
                          INTERFACE
                              cxx_std_17
                              )

set_target_properties( ${PROJECT_NAME}
                      PROPERTIES
                          PUBLIC_HEADER
                            ${CMAKE_CURRENT_SOURCE_DIR}/include/ugltf/ugltf.h )

target_compile_options( ${PROJECT_NAME} INTERFACE -Wextra -Wall)

target_compile_definitions( ${PROJECT_NAME}
                                INTERFACE
                                TEST_DEFINE)

# If you create any targets, add them to the following
# variable so that the unit tests link to them.
LIST(APPEND UNIT_TEST_LINK_TARGETS
              ${PROJECT_NAME}::${PROJECT_NAME}
              ${PROJECT_NAME}::coverage)

#################################################################################################





#################################################################################################
# Download the single header file for nlohmann/json. This is only for the unit tests
# and examples. It is not included with uGLTF
#################################################################################################
if( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/nlohmann/json.hpp" )
    file(DOWNLOAD "https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp" "${CMAKE_CURRENT_BINARY_DIR}/include/nlohmann/json.hpp" )
endif()

# Also download stb_image
if( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/stb_image.h" )
    file(DOWNLOAD "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h" "${CMAKE_CURRENT_BINARY_DIR}/include/stb_image.h" )
endif()
#################################################################################################






if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test")
    enable_testing()
    add_subdirectory(test)
endif()

add_executable(
    scene
    examples/scene.cpp
)

target_link_libraries(scene ${PROJECT_NAME}::${PROJECT_NAME} )

add_executable(
    gltfInfo
    examples/gltfInfo.cpp
)
target_link_libraries(gltfInfo ${PROJECT_NAME}::${PROJECT_NAME} )






#################################################################################################
#
#################################################################################################
include(GNUInstallDirs)

INSTALL(TARGETS
              ${PROJECT_NAME}
        LIBRARY
              DESTINATION some/libpath
        PUBLIC_HEADER
              DESTINATION ugltf
)

#################################################################################################


#https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
