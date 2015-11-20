# Install script for directory: W:/testing/DD/include/magnum/corrade/src/Corrade/Utility

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
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/Corrade/lib/CorradeUtility-d.lib")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files/Corrade/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Debug/CorradeUtility-d.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/Corrade/lib/CorradeUtility.lib")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files/Corrade/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Release/CorradeUtility.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/Corrade/lib/CorradeUtility.lib")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files/Corrade/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/MinSizeRel/CorradeUtility.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/Corrade/lib/CorradeUtility.lib")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files/Corrade/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/RelWithDebInfo/CorradeUtility.lib")
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/Corrade/bin/CorradeUtility-d.dll")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files/Corrade/bin" TYPE SHARED_LIBRARY FILES "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Debug/CorradeUtility-d.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/Corrade/bin/CorradeUtility.dll")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files/Corrade/bin" TYPE SHARED_LIBRARY FILES "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Release/CorradeUtility.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/Corrade/bin/CorradeUtility.dll")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files/Corrade/bin" TYPE SHARED_LIBRARY FILES "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/MinSizeRel/CorradeUtility.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/Corrade/bin/CorradeUtility.dll")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files/Corrade/bin" TYPE SHARED_LIBRARY FILES "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/RelWithDebInfo/CorradeUtility.dll")
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files/Corrade/include/Corrade/Utility/Arguments.h;C:/Program Files/Corrade/include/Corrade/Utility/AbstractHash.h;C:/Program Files/Corrade/include/Corrade/Utility/Assert.h;C:/Program Files/Corrade/include/Corrade/Utility/Configuration.h;C:/Program Files/Corrade/include/Corrade/Utility/ConfigurationGroup.h;C:/Program Files/Corrade/include/Corrade/Utility/ConfigurationValue.h;C:/Program Files/Corrade/include/Corrade/Utility/Debug.h;C:/Program Files/Corrade/include/Corrade/Utility/Directory.h;C:/Program Files/Corrade/include/Corrade/Utility/Endianness.h;C:/Program Files/Corrade/include/Corrade/Utility/Macros.h;C:/Program Files/Corrade/include/Corrade/Utility/MurmurHash2.h;C:/Program Files/Corrade/include/Corrade/Utility/Resource.h;C:/Program Files/Corrade/include/Corrade/Utility/Sha1.h;C:/Program Files/Corrade/include/Corrade/Utility/String.h;C:/Program Files/Corrade/include/Corrade/Utility/TypeTraits.h;C:/Program Files/Corrade/include/Corrade/Utility/Unicode.h;C:/Program Files/Corrade/include/Corrade/Utility/utilities.h;C:/Program Files/Corrade/include/Corrade/Utility/Utility.h;C:/Program Files/Corrade/include/Corrade/Utility/VisibilityMacros.h;C:/Program Files/Corrade/include/Corrade/Utility/visibility.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Program Files/Corrade/include/Corrade/Utility" TYPE FILE FILES
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Arguments.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/AbstractHash.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Assert.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Configuration.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/ConfigurationGroup.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/ConfigurationValue.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Debug.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Directory.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Endianness.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Macros.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/MurmurHash2.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Resource.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Sha1.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/String.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/TypeTraits.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Unicode.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/utilities.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Utility.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/VisibilityMacros.h"
    "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/visibility.h"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/Corrade/bin/corrade-rc.exe")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files/Corrade/bin" TYPE EXECUTABLE FILES "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Debug/corrade-rc.exe")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/Corrade/bin/corrade-rc.exe")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files/Corrade/bin" TYPE EXECUTABLE FILES "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/Release/corrade-rc.exe")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/Corrade/bin/corrade-rc.exe")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files/Corrade/bin" TYPE EXECUTABLE FILES "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/MinSizeRel/corrade-rc.exe")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/Corrade/bin/corrade-rc.exe")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files/Corrade/bin" TYPE EXECUTABLE FILES "W:/testing/DD/include/magnum/corrade/src/Corrade/Utility/RelWithDebInfo/corrade-rc.exe")
  endif()
endif()

