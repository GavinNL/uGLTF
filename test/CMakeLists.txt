# Create a static library for Catch2s main so that we can reduce
# compiling time. Each unit test will link to this
cmake_minimum_required(VERSION 3.13)

add_library(               ${PROJECT_NAME}-catchmain STATIC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp )
target_include_directories(${PROJECT_NAME}-catchmain PUBLIC third_party)
target_compile_features(   ${PROJECT_NAME}-catchmain PUBLIC cxx_std_11)
target_compile_options(    ${PROJECT_NAME}-catchmain PUBLIC -Wextra -Wno-type-limits)




# Find all files named unit-*.cpp
file(GLOB files "unit-*.cpp")

enable_testing()

foreach(file ${files})

     get_filename_component(file_basename ${file} NAME_WE)
     string(REGEX REPLACE "unit-([^$]+)" "test-${folder_name}-\\1" testcase ${file_basename})

     string(REGEX REPLACE "unit-([^$]+)" "unit-\\1" exe_name ${file_basename})

     message("New File: ${file} Test case: ${testcase} Exe name: ${exe_name}")


set(UNIT_EXE_NAME ${PROJECT_NAME}-${exe_name} )
set(UNIT_TEST_NAME test-${PROJECT_NAME}-${exe_name} )

add_executable( ${UNIT_EXE_NAME}
                ${file} )

target_compile_features( ${UNIT_EXE_NAME}
                            PUBLIC
                            cxx_std_11)


target_link_libraries(      ${UNIT_EXE_NAME} PUBLIC ${PROJECT_NAME}-catchmain   ${UNIT_TEST_LINK_TARGETS}   )
target_include_directories( ${UNIT_EXE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

 add_test( NAME ${UNIT_TEST_NAME}
           COMMAND ${UNIT_EXE_NAME}
         )
endforeach()



#################################################################################################
# Download an example glb file from KhronosGroup
#################################################################################################

set(SAMPLE_MODEL_URI "https://github.com/KhronosGroup/glTF-Sample-Models/raw/0c1ffd876136b824a50e584b9158a3199fead935")


if( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/BoomBox.glb" )
    file(DOWNLOAD "${SAMPLE_MODEL_URI}/2.0/BoomBox/glTF-Binary/BoomBox.glb" "${CMAKE_CURRENT_BINARY_DIR}/BoomBox.glb" )
endif()

if( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/CesiumMan.glb" )
    file(DOWNLOAD "${SAMPLE_MODEL_URI}/2.0/CesiumMan/glTF-Binary/CesiumMan.glb" "${CMAKE_CURRENT_BINARY_DIR}/CesiumMan.glb" )
endif()

if( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/BrainStem.glb" )
    file(DOWNLOAD "${SAMPLE_MODEL_URI}/2.0/BrainStem/glTF-Binary/BrainStem.glb" "${CMAKE_CURRENT_BINARY_DIR}/BrainStem.glb" )
endif()

if( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/tiny_gltf.h" )
    file(DOWNLOAD "https://raw.githubusercontent.com/syoyo/tinygltf/master/tiny_gltf.h" "${CMAKE_CURRENT_BINARY_DIR}/tiny_gltf.h" )
endif()

if( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/BoxAnimated.glb" )
    file(DOWNLOAD "${SAMPLE_MODEL_URI}/2.0/BoxAnimated/glTF-Binary/BoxAnimated.glb" "${CMAKE_CURRENT_BINARY_DIR}/BoxAnimated.glb" )
endif()


if( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/BoxAnimated.gltf" )
    file(DOWNLOAD "${SAMPLE_MODEL_URI}/2.0/BoxAnimated/glTF-Embedded/BoxAnimated.gltf" "${CMAKE_CURRENT_BINARY_DIR}/BoxAnimated.gltf" )
endif()

if( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/BoxTextured.gltf" )
    file(DOWNLOAD "${SAMPLE_MODEL_URI}/2.0/BoxTextured/glTF-Embedded/BoxTextured.gltf" "${CMAKE_CURRENT_BINARY_DIR}/BoxTextured.gltf" )
endif()

if( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/BoxTextured.glb" )
    file(DOWNLOAD "${SAMPLE_MODEL_URI}/2.0/BoxTextured/glTF-Binary/BoxTextured.glb?raw=true" "${CMAKE_CURRENT_BINARY_DIR}/BoxTextured.glb" )
endif()




#################################################################################################
