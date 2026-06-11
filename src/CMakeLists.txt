qt_add_executable(info_overlay_bin
    main.cpp
    OverlayAdaptor.h
)
qt_add_qml_module(info_overlay_bin
    URI info_overlay
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)
target_link_libraries(info_overlay_bin PRIVATE
    Qt6::Quick
    Qt6::WebEngineQuick
    Qt6::DBus
    LayerShellQt::Interface
)
install(TARGETS info_overlay_bin
        DESTINATION bin
        RENAME info_overlay)
