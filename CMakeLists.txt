cmake_minimum_required(VERSION 3.16)

project(info_overlay VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find necessary system dependencies
find_package(Qt6 6.5 REQUIRED COMPONENTS Quick WaylandClient DBus Core)
find_package(LayerShellQt REQUIRED)

qt_standard_project_setup()

# Define the single executable including all split files
qt_add_executable(info_overlay_bin
    src/main.cpp
    src/PortalShortcuts.h
    src/PortalShortcuts.cpp
    src/OverlayMaskHandler.h
    src/OverlayMaskHandler.cpp
    src/AppLauncher.h
)

# Bundle QML assets into the binary
qt_add_qml_module(info_overlay_bin
    URI info_overlay
    VERSION 1.0
    QML_FILES
        src/qml/Main.qml
)

# Link everything to your target binary
target_link_libraries(info_overlay_bin PRIVATE
    Qt6::Quick
    Qt6::DBus
    LayerShellQt::Interface
)

# Installation targets matching your structure
install(TARGETS info_overlay_bin
        DESTINATION bin
        RENAME info_overlay)
