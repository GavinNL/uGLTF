cmake_minimum_required(VERSION 3.10)

project(ugltf)

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

target_compile_options( ${PROJECT_NAME} INTERFACE -Wextra -Wall)

target_compile_definitions( ${PROJECT_NAME}
                                INTERFACE
                                TEST_DEFINE)

# If you create any targets, add them to the following
# variable so that the unit tests link to them.
LIST(APPEND UNIT_TEST_LINK_TARGETS "${PROJECT_NAME}::${PROJECT_NAME}")


#################################################################################################
# Download the single header file for nlohmann/json. This is only for the unit tests
# and examples. It is not included with uGLTF
#################################################################################################
if( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/nlohmann/json.hpp" )
    file(DOWNLOAD "https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp" "${CMAKE_CURRENT_BINARY_DIR}/include/nlohmann/json.hpp" )
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






#https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
