cmake_minimum_required(VERSION 3.16)
project(regul_installator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g0")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")
endif()

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

file(GLOB_RECURSE ALL_PACKAGE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/packages/*")

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/InstallerEngine.cpp
)

set(HEADERS
    src/MainWindow.h
    src/InstallerEngine.h
)

set(QRC_CONTENT "<RCC>\n  <qresource prefix=\"/\">\n")

foreach(PACKAGE_FILE ${ALL_PACKAGE_FILES})
    file(RELATIVE_PATH REL_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${PACKAGE_FILE})

    get_filename_component(FILENAME ${PACKAGE_FILE} NAME)
    get_filename_component(FILE_DIR ${PACKAGE_FILE} DIRECTORY)
    get_filename_component(PARENT_DIR ${FILE_DIR} NAME)

    if(PARENT_DIR STREQUAL "packages")
        set(ALIAS_PATH "packages/${FILENAME}")
    else()
        get_filename_component(SUB_DIR ${FILE_DIR} NAME)
        set(ALIAS_PATH "packages/${SUB_DIR}/${FILENAME}")
    endif()

    set(QRC_CONTENT "${QRC_CONTENT}    <file alias=\"${ALIAS_PATH}\">${PACKAGE_FILE}</file>\n")
endforeach()

set(QRC_CONTENT "${QRC_CONTENT}  </qresource>\n</RCC>")

file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/src/resources.qrc ${QRC_CONTENT})

qt5_add_resources(QRC_FILES src/resources.qrc)

add_executable(regul_installator ${SOURCES} ${HEADERS} ${QRC_FILES}
    packages/wine/wine.list
    README.md
)

target_link_libraries(regul_installator Qt5::Widgets Qt5::Core)

target_include_directories(regul_installator PRIVATE src)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/packages/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/packages)
