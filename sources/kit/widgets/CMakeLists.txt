set(WIDGETS_SRC
    src/mainwindow.cpp
)

qt6_wrap_ui(CDK_UI_FORM
    ui/mainwindow.ui
)

add_library(${CDK_UI_LIB}
    ${CDK_UI_FORM}
    ${WIDGETS_SRC}
)

target_link_libraries(${CDK_UI_LIB}
    PUBLIC
    Qt::OpenGLWidgets
    ${3RDPARTY_LIB}
)

target_include_directories(${CDK_UI_LIB}
    PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    src
    ui
    ../depth
    ../glbridge
)
