cmake_minimum_required(VERSION 3.8)

project(ModernGL)

set(OUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIB_PATH ${CMAKE_SOURCE_DIR}/lib)
set(INC_PATH ${CMAKE_SOURCE_DIR}/External/GLExtension/include)

add_subdirectory(./External/GLExtension)
add_subdirectory(./Graphics)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${INC_PATH})
target_link_directories(${PROJECT_NAME} PRIVATE ${LIB_PATH})
target_link_libraries(${PROJECT_NAME} PRIVATE GLExtension.lib)

add_dependencies(${PROJECT_NAME} lib::GLExtension)
add_dependencies(${PROJECT_NAME} lib::Graphics)

set_target_properties(${PROJECT_NAME} 
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUT_PATH}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUT_PATH}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUT_PATH}
    )
