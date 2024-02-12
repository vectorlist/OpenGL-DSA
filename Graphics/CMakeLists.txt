
file(GLOB SRCS CONFIGURE_DEPENDS "./*.h" "./*.cpp")

add_library(Graphics STATIC ${SRCS})
add_library(lib::Graphics ALIAS Graphics)

message(STATUS "Graphics can read : ${INC_PATH}")
target_include_directories(Graphics PUBLIC ${INC_PATH})

if(EXISTS ${LIB_PATH})
    message(STATUS "${Graphics} : found lib path : ${LIB_PATH}")
#else()
#    message(STATUS "${PROJECT_NAME} : failed found lib path replace lib folder")
endif()

set_target_properties(Graphics PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${LIB_PATH}
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${LIB_PATH}
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${LIB_PATH})





