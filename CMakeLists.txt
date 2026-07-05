cmake_minimum_required(VERSION 3.16)

project(info_overlay VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find necessary system dependencies
find_package(Qt6 6.5 REQUIRED COMPONENTS Quick WaylandClient DBus Core ShaderTools)
find_package(LayerShellQt REQUIRED)
find_package(KF6WindowSystem REQUIRED)

qt_standard_project_setup()

# Define the single executable including all split files
qt_add_executable(info_overlay_bin
    src/main.cpp
    src/PortalShortcuts.h
    src/PortalShortcuts.cpp
    src/OverlayMaskHandler.h
    src/OverlayMaskHandler.cpp
    src/AppLauncher.h
    src/SystemGlobals.h
    src/SystemGlobals.cpp
)

# Bundle QML assets and textures into the binary
qt_add_qml_module(info_overlay_bin
    URI info_overlay
    VERSION 1.0
    QML_FILES
        src/qml/Main.qml
        src/qml/components/Crosshair.qml
        src/qml/components/Divider.qml
        src/qml/components/DividerVertical.qml
        src/qml/components/InteractiveSection.qml
        src/qml/components/OverlayCard.qml
        src/qml/components/RainOverlay.qml
        src/qml/components/StatsTimer.qml
        src/qml/components/ViewOnlySection.qml
        src/qml/components/TopBar.qml
)

qt_add_shaders(info_overlay_bin "info_overlay_shaders"
    PREFIX
        "/info_overlay"
    FILES
        "src/shaders/raindrops.frag"
)

# Link everything to your target binary
target_link_libraries(info_overlay_bin PRIVATE
    Qt6::Quick
    Qt6::DBus
    LayerShellQt::Interface
    KF6::WindowSystem
)

# Installation targets matching your structure
install(TARGETS info_overlay_bin
        DESTINATION bin
        RENAME info_overlay)
