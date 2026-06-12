# Install script for directory: /wintfs/papers/snp/anonymous-submission/openfhe-sgx/openfhe

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/wintfs/papers/snp/anonymous-submission/openfhe-sgx/installed")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/openfhe/core" TYPE FILE MESSAGE_LAZY FILES "/wintfs/papers/snp/anonymous-submission/openfhe-sgx/build/src/core/config_core.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/openfhe" TYPE DIRECTORY MESSAGE_LAZY FILES "/wintfs/papers/snp/anonymous-submission/openfhe-sgx/openfhe/third-party/cereal/include/")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/wintfs/papers/snp/anonymous-submission/openfhe-sgx/build/openfhe/src/core/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/wintfs/papers/snp/anonymous-submission/openfhe-sgx/build/openfhe/src/pke/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/wintfs/papers/snp/anonymous-submission/openfhe-sgx/build/openfhe/src/binfhe/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/wintfs/papers/snp/anonymous-submission/openfhe-sgx/installed/lib/OpenFHE/OpenFHEConfig.cmake;/wintfs/papers/snp/anonymous-submission/openfhe-sgx/installed/lib/OpenFHE/OpenFHEConfigVersion.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/wintfs/papers/snp/anonymous-submission/openfhe-sgx/installed/lib/OpenFHE" TYPE FILE MESSAGE_LAZY FILES
    "/wintfs/papers/snp/anonymous-submission/openfhe-sgx/build/openfhe/CMakeFiles/OpenFHEConfig.cmake"
    "/wintfs/papers/snp/anonymous-submission/openfhe-sgx/build/openfhe/OpenFHEConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/wintfs/papers/snp/anonymous-submission/openfhe-sgx/installed/lib/OpenFHE/OpenFHETargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}/wintfs/papers/snp/anonymous-submission/openfhe-sgx/installed/lib/OpenFHE/OpenFHETargets.cmake"
         "/wintfs/papers/snp/anonymous-submission/openfhe-sgx/build/openfhe/CMakeFiles/Export/d2ab7033864867e081c8883a6acc7ee1/OpenFHETargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}/wintfs/papers/snp/anonymous-submission/openfhe-sgx/installed/lib/OpenFHE/OpenFHETargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}/wintfs/papers/snp/anonymous-submission/openfhe-sgx/installed/lib/OpenFHE/OpenFHETargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/wintfs/papers/snp/anonymous-submission/openfhe-sgx/installed/lib/OpenFHE/OpenFHETargets.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/wintfs/papers/snp/anonymous-submission/openfhe-sgx/installed/lib/OpenFHE" TYPE FILE MESSAGE_LAZY FILES "/wintfs/papers/snp/anonymous-submission/openfhe-sgx/build/openfhe/CMakeFiles/Export/d2ab7033864867e081c8883a6acc7ee1/OpenFHETargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "/wintfs/papers/snp/anonymous-submission/openfhe-sgx/installed/lib/OpenFHE/OpenFHETargets-release.cmake")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    file(INSTALL DESTINATION "/wintfs/papers/snp/anonymous-submission/openfhe-sgx/installed/lib/OpenFHE" TYPE FILE MESSAGE_LAZY FILES "/wintfs/papers/snp/anonymous-submission/openfhe-sgx/build/openfhe/CMakeFiles/Export/d2ab7033864867e081c8883a6acc7ee1/OpenFHETargets-release.cmake")
  endif()
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/wintfs/papers/snp/anonymous-submission/openfhe-sgx/build/openfhe/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
