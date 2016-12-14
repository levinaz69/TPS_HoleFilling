cmake_minimum_required(VERSION 2.8.8)
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

PROJECT(TPS_HoleFilling)

# guard against in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

# if(WIN32)
  # set(ENV{PATH} ${VTK_DIR};$ENV{PATH})
# endif()

#set(CMAKE_CXX_STANDARD 11)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


#set(SOURCE_FILES main.cpp)
#add_executable(TPS_HoleFilling ${SOURCE_FILES})

add_executable(TPS_HoleFilling MACOSX_BUNDLE src/TPS_HoleFilling.cpp)

if(VTK_LIBRARIES)
  target_link_libraries(TPS_HoleFilling ${VTK_LIBRARIES})
else()
  target_link_libraries(TPS_HoleFilling vtkHybrid vtkWidgets)
endif()
