cmake_minimum_required(VERSION 3.15)
project(MCHex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenMP REQUIRED)

file(GLOB SOURCE_FILES "src/*.cpp" "src/*.h")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    DEBUG_POSTFIX "_d"
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    CONFIGURATIONS Release
)

if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found, version: ${OpenMP_CXX_VERSION}")
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory 
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/inputBoundaries"
    COMMENT "Create inputBoundaries directory"

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/src/parameters.txt"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/parameters.txt"
    COMMENT "Copy parameters.txt"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Constants"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Constants"
    COMMENT "Copy Constants directory"
)