find_package(Qt6 REQUIRED COMPONENTS Widgets)

find_package(azure-kinect-sensor-sdk QUIET PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
if (${azure-kinect-sensor-sdk_FOUND})
  message("K4A SDK found")

  list(APPEND K4A_BINS ${azure-kinect-sensor-sdk_BINARIES})
  set(K4A_INCLUDE_DIRS "${K4A_INSTALL_PATH}/src/sdk/include")
  list(APPEND K4A_INCLUDE_DIRS ${K4A_INSTALL_PATH}/../include)
  list(APPEND K4A_INCLUDE_DIRS ${K4A_INSTALL_PATH}/src/record/sdk/include)
  list(APPEND K4A_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/sources/3rdparty/Azure-Kinect-Sensor-SDK/include")

  # Create a list of all K4A DLLs
  list(APPEND K4A_DLLS ${azure-kinect-sensor-sdk_LIBRARIES})
  list(APPEND K4A_DLLS ${azure-kinect-sensor-sdk_BINARIES})

  foreach(DLL ${K4A_DLLS})
    set(prop "PROPERTY-NOTFOUND")
    get_property(prop TARGET ${DLL} PROPERTY IMPORTED_LOCATION)
    message(STATUS "IMPORTED_LOCATION: ${prop}")

    if((NOT prop) OR (prop EQUAL "PROPERTY-NOTFOUND"))
      message(FATAL_ERROR "Target '${DLL}' in package azure-kinect-sensor-sdk does not contain an IMPORTED_LOCATION property")
    endif()

    list(APPEND K4A_DLL_FILES ${prop})
  endforeach(DLL)
else()
    message("Error: K4A SDK not found ! Please install the Azure Kinect SDK.")
endif()

message("K4A Install Needed: ${K4A_INSTALL_PATH}")
message("K4A Include Dirs: ${K4A_INCLUDE_DIRS}")
message("K4A_BINS: ${K4A_BIN_PATH}")
message("K4A Libs: ${K4A_LIBS}")
message("K4A DLLs: ${K4A_DLL_FILES}")


find_package(OpenCV QUIET PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
if (${OpenCV_FOUND})
    message("OpenCV found of ver.${OpenCV_VERSION}.")
    message(STATUS "OpenCV_INSTALL_PATH = ${OpenCV_INSTALL_PATH}")
    message(STATUS "OpenCV Include Dirs = ${OpenCV_INCLUDE_DIRS}")
    message(STATUS "OpenCV_BIN_PATH = ${OpenCV_BIN_PATH}")
    message(STATUS "OpenCV_LIBRARIES = ${OpenCV_LIBRARIES}")
    message(STATUS "OpenCV_DLL_FILES = ${OpenCV_DLL_FILES}")
else()
    message("Error: OpenCV not found ! Please install the OpenCV.")
endif()

add_library(${3RDPARTY_LIB}
    sources/DepthPixelColorizer.h
    sources/Pixel.h
    sources/StaticImageProperties.cpp
    sources/colorlisteditor.cpp
    json11/json11.cpp
)
target_link_libraries(${3RDPARTY_LIB} PUBLIC
    ${K4A_LIBS}
    ${OpenCV_LIBRARIES}
    ${aruco}
    Qt6::Widgets
)

target_include_directories(${3RDPARTY_LIB}
    PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${K4A_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    sources
)
