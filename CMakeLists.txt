cmake_minimum_required(VERSION 3.25)
project(WebSocketServer)

file(GLOB INCLUDE_FILES
    *.h
    **/*.h)

file(GLOB SOURCE_FILES
    *.cpp
    **/*.cpp)

add_executable(${PROJECT_NAME} ${INCLUDE_FILES} ${SOURCE_FILES})

if (NOT MSVC AND NOT MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    _WEBSOCKETPP_CPP11_STL_
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)