cmake_minimum_required(VERSION 3.10)

get_filename_component(folder_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" folder_name ${folder_name})

project(${folder_name})

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
