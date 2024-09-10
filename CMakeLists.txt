cmake_minimum_required(VERSION 3.16)

project(mic-indicator LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fno-exceptions -fno-rtti)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
pkg_search_module(PIPEWIRE REQUIRED libpipewire-0.3)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME} Qt6::Widgets)
target_include_directories(${PROJECT_NAME} PRIVATE ${PIPEWIRE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${PIPEWIRE_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
