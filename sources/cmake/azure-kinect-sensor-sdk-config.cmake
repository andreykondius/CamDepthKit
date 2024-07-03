# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

if (azure-kinect-sensor-sdk_CONFIG_INCLUDED)
  return()
endif()
set(azure-kinect-sensor-sdk_CONFIG_INCLUDED TRUE)

set(K4A_INSTALL_PATH "${CMAKE_SOURCE_DIR}/sources/3rdparty/Azure-Kinect-Sensor-SDK/release")
set(K4A_BIN_PATH "${K4A_INSTALL_PATH}/bin")
set(K4A_LIB_PATH "${K4A_INSTALL_PATH}/src/sdk")
set(K4ARECORD_LIB_PATH "${K4A_INSTALL_PATH}/src/record/sdk")

function(addlib a_lib a_path)

foreach(lib ${a_lib})

  set(onelib "${lib}-NOTFOUND")
  find_library(onelib ${lib}
      PATHS "${a_path}"
      NO_DEFAULT_PATH)

  if((NOT onelib) OR (onelib EQUAL "${lib}-NOTFOUND"))
    message(WARNING "Library '${lib}' not found in ${a_path}")
    message(FATAL_ERROR "Library '${lib}' in package azure-kinect-sensor-sdk is not installed properly")
  else()
    message(STATUS "Library '${lib}' FOUND")
  endif()

  set(onebin "${lib}-NOTFOUND")
  find_file(onebin "${lib}.dll"
      PATHS "${K4A_BIN_PATH}"
      NO_DEFAULT_PATH)

  if((NOT onebin) OR (onebin EQUAL "${lib}-NOTFOUND"))
    message(WARNING "Binary '${lib}' not found in ${K4A_BIN_PATH}")
    message(FATAL_ERROR "Binary '${lib}.dll' in package azure-kinect-sensor-sdk is not installed properly")
  endif()

  set(K4A_LIBS ${K4A_LIBS} ${a_path}/${lib}.lib PARENT_SCOPE)

  add_library(azure-kinect-sensor-sdk-${lib} SHARED IMPORTED GLOBAL)
  set_property(TARGET azure-kinect-sensor-sdk-${lib} PROPERTY IMPORTED_LOCATION ${onebin})
  set_property(TARGET azure-kinect-sensor-sdk-${lib} PROPERTY IMPORTED_IMPLIB ${onelib})

  list(APPEND azure-kinect-sensor-sdk_LIBRARIES azure-kinect-sensor-sdk-${lib})

endforeach()

endfunction()

addlib(k4a ${K4A_LIB_PATH})
addlib(k4arecord ${K4ARECORD_LIB_PATH})

set(depthengine_2_0_BIN_PATH "C:/Program Files/Azure Kinect SDK v1.4.1/sdk/windows-desktop/amd64/release/bin")

foreach(bin depthengine_2_0)

  set(onebin "${bin}-NOTFOUND")
  find_file(onebin "${bin}.dll"
      PATHS "${depthengine_2_0_BIN_PATH}"
      NO_DEFAULT_PATH)

  if((NOT onebin) OR (onebin EQUAL "${bin}-NOTFOUND"))
    message(WARNING "Binary '${bin}' not found in ${depthengine_2_0_BIN_PATH}")
    message(FATAL_ERROR "Binary '${bin}.dll' in package azure-kinect-sensor-sdk is not installed properly")
  else()
    message(STATUS "Binary '${bin}' FOUND !")
  endif()

  add_library(azure-kinect-sensor-sdk-${bin} SHARED IMPORTED GLOBAL)
  set_property(TARGET azure-kinect-sensor-sdk-${bin} PROPERTY IMPORTED_LOCATION ${onebin})

  list(APPEND azure-kinect-sensor-sdk_BINARIES azure-kinect-sensor-sdk-${bin})

endforeach()
