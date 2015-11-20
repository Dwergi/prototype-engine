# Install script for directory: W:/testing/DD/include/magnum/corrade/src/Corrade/Containers

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/Corrade")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files/Corrade/include/Corrade/Containers/Array.h;C:/Program Files/Corrade/include/Corrade/Containers/ArrayView.h;C:/Program Files/Corrade/include/Corrade/Containers/Containers.h;C:/Program Files/Corrade/include/Corrade/Containers/EnumSet.h;C:/Program Files/Corrade/include/Corrade/Containers/LinkedList.h;C:/Program Files/Corrade/include/Corrade/Containers/Tags.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Program Files/Corrade/include/Corrade/Containers" TYPE FILE FILES
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Containers/Array.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Containers/ArrayView.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Containers/Containers.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Containers/EnumSet.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Containers/LinkedList.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Containers/Tags.h"
    )
endif()

