cmake_minimum_required(VERSION 3.16)
project(ROSInspector VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt6 Modules (WebEngineRemoved -> OpenGL Added)
find_package(Qt6 REQUIRED COMPONENTS Quick Concurrent OpenGL)
find_package(PkgConfig REQUIRED)
pkg_check_modules(TINYXML2 REQUIRED tinyxml2)
pkg_check_modules(YAMLCPP REQUIRED yaml-cpp)

# C++ Sources
set(PROJECT_SOURCES
    src/main.cpp
    src/facade/InspectorFacade.h
    src/facade/InspectorFacade.cpp
    src/parsers/XmlParser.h
    src/parsers/XmlParser.cpp
    src/parsers/CmakeParser.h
    src/parsers/CmakeParser.cpp
    src/parsers/YamlParser.h
    src/parsers/YamlParser.cpp
    src/parsers/UrdfParser.h
    src/parsers/UrdfParser.cpp
    src/parsers/LaunchParser.h
    src/parsers/LaunchParser.cpp
    src/engines/CrossFileAnalyzer.h
    src/engines/CrossFileAnalyzer.cpp
    src/engines/HealthCalculator.h
    src/engines/HealthCalculator.cpp
    src/engines/ForceGraphPhysics.h
    src/engines/ForceGraphPhysics.cpp
    src/renderers/Native3DGraphItem.h
    src/renderers/Native3DGraphItem.cpp
    src/models/IssueReport.h
)

qt_add_executable(ROSInspector ${PROJECT_SOURCES})
set_target_properties(ROSInspector PROPERTIES OUTPUT_NAME "ROSInspectorApp")

target_include_directories(ROSInspector PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/facade
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engines
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parsers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/renderers
    ${TINYXML2_INCLUDE_DIRS}
    ${YAMLCPP_INCLUDE_DIRS}
)

target_link_libraries(ROSInspector PRIVATE 
    Qt6::Quick 
    Qt6::Concurrent
    Qt6::OpenGL
    ${TINYXML2_LIBRARIES}
    ${YAMLCPP_LIBRARIES}
)

qt_add_qml_module(ROSInspector
    URI ROSInspector
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    QML_FILES 
        ui/main.qml
        ui/HeaderScore.qml
        ui/DropZone.qml
        ui/FileListView.qml
        ui/IssueDetailView.qml
        ui/DiffViewer.qml
        ui/Graph3DView.qml
)
