# Create a static library for Catch2s main so that we can reduce
# compiling time. Each unit test will link to this
cmake_minimum_required(VERSION 3.13)

add_library(${PROJECT_NAME}-catchmain STATIC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_include_directories(${PROJECT_NAME}-catchmain PUBLIC third_party)
target_compile_features(${PROJECT_NAME}-catchmain PUBLIC cxx_std_11)



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
 ${file}
 )
 target_compile_features( ${UNIT_EXE_NAME}
 PUBLIC
 cxx_std_11)

 target_link_libraries( ${UNIT_EXE_NAME} PUBLIC ${PROJECT_NAME}-catchmain ${UNIT_TEST_LINK_TARGETS})
 add_test( NAME ${UNIT_TEST_NAME}
 COMMAND ${UNIT_EXE_NAME}
 )
endforeach()
