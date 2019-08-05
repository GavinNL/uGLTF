cmake_minimum_required(VERSION 3.10)

project(ugltf)

add_library( ${PROJECT_NAME} src/myfile.cpp include/gltfpp/gltfpp.h )
add_library( ${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME} )
target_include_directories( ${PROJECT_NAME}
                            PUBLIC
                               "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
 )
target_compile_features( ${PROJECT_NAME}
                          PUBLIC
                              cxx_std_17)

target_compile_definitions( ${PROJECT_NAME}
                                PUBLIC
                                TEST_DEFINE)

# If you create any targets, add them to the following
# variable so that the unit tests link to them.
LIST(APPEND UNIT_TEST_LINK_TARGETS "${PROJECT_NAME}::${PROJECT_NAME}")


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test")
    enable_testing()
    add_subdirectory(test)
endif()


add_executable(
    scene
    examples/scene.cpp
)
target_link_libraries(scene ${PROJECT_NAME}::${PROJECT_NAME} )


if( NOT EXISTS "${CMAKE_SOURCE_DIR}/share/gltfpp/BoomBox.glb" )
    file(DOWNLOAD "https://github.com/KhronosGroup/glTF-Sample-Models/raw/master/2.0/BoomBox/glTF-Binary/BoomBox.glb" "${CMAKE_SOURCE_DIR}/share/gltfpp/BoomBox.glb" )
endif()

