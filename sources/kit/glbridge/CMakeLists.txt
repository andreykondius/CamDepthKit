add_library(${GL_UI_LIB}
    glwidget.cpp 
)

target_link_libraries(${GL_UI_LIB}
    PUBLIC
    Qt::OpenGLWidgets
)

target_include_directories(${GL_UI_LIB}
    PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../depth
)
