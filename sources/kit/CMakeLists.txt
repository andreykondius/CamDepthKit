project(CamDepthKit VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC OFF)

ADD_DEFINITIONS(-DPROJECT_MAJOR_VERSION=1)
ADD_DEFINITIONS(-DPROJECT_MINOR_VERSION=0)
ADD_DEFINITIONS(-DPROJECT_PATCH_VERSION=0)

find_package(Qt6 REQUIRED COMPONENTS Gui Core Widgets OpenGL OpenGLWidgets)

set(CDK_UI_LIB cdk_ui)
set(GL_UI_LIB gl_context)
add_subdirectory(widgets)
add_subdirectory(glbridge)

qt6_add_resources(RESOURCES resources/buttons/res.qrc)

file(GLOB DEPTH_SOURCES "depth/*.*")

set(SRC
    main.cpp
    depth/type.h
    ${DEPTH_SOURCES}
)

install(TARGETS ${CamDepthKit}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

add_executable(CamDepthKit
    ${SRC}
    ${RESOURCES}
)

target_include_directories(CamDepthKit
    PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    widgets/src
    widgets/ui
    glbridge
    depth
)

target_link_libraries(CamDepthKit
    PUBLIC
    Qt6::Widgets
    Qt::OpenGL
    Qt::OpenGLWidgets
    ${CDK_UI_LIB}
    ${GL_UI_LIB}
    ${3RDPARTY_LIB}
)

set(bin_folder ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(EXECUTABLE_OUTPUT_PATH ${bin_folder})

macro (set_target_deploy_qt arg1 arg2)
    if(NOT EXISTS "${CMAKE_PREFIX_PATH}/bin/windeployqt.exe")
        message(FATAL_ERROR "windeployqt.exe not found in '${CMAKE_PREFIX_PATH}/bin'. Please check that your Qt installation includes the deployment tool.")
    endif()
    add_custom_command(TARGET ${arg1}
        POST_BUILD COMMAND "${CMAKE_PREFIX_PATH}/bin/windeployqt.exe"
        \"$<TARGET_FILE:${arg1}>\"
        COMMENT "Deploying Qt libraries using windeployqt.exe for compilation target '${arg1}' ..."
    )
    add_custom_command(TARGET ${arg1} POST_BUILD COMMAND
        ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}/bin/Qt6OpenGLWidgets${DLL_POSTFIX}.dll ${EXECUTABLE_OUTPUT_PATH}
    )
    add_custom_command(TARGET ${arg1} POST_BUILD COMMAND
        ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}/bin/Qt6OpenGL${DLL_POSTFIX}.dll ${EXECUTABLE_OUTPUT_PATH}
    )
    file(GLOB DLL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/opencv/builds/${arg2}/bin/*.dll)
    foreach(dll_file IN LISTS DLL_FILES)
        add_custom_command(TARGET ${arg1} POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy ${dll_file} ${EXECUTABLE_OUTPUT_PATH})
    endforeach()
    file(GLOB DLL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/Azure-Kinect-Sensor-SDK/${arg2}/bin/*.dll)
    foreach(dll_file IN LISTS DLL_FILES)
        add_custom_command(TARGET ${arg1} POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy ${dll_file} ${EXECUTABLE_OUTPUT_PATH})
    endforeach()
    file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/files)
    file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/files/mkv)
    add_custom_command(TARGET ${arg1} POST_BUILD COMMAND
        ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources/mkv ${EXECUTABLE_OUTPUT_PATH}/files/mkv
    )
    file(MAKE_DIRECTORY ${bin_folder}/files/json)
    add_custom_command(TARGET ${arg1} POST_BUILD COMMAND
        ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources/json ${EXECUTABLE_OUTPUT_PATH}/files/json
    )
endmacro (set_target_deploy_qt)
file(MAKE_DIRECTORY ${bin_folder}/files/matricies)
file(MAKE_DIRECTORY ${bin_folder}/files/points_cloud)
set_target_deploy_qt(CamDepthKit ${CMAKE_BUILD_TYPE})


