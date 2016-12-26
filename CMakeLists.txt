CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
IF(POLICY CMP0020)
  CMAKE_POLICY(SET CMP0020 NEW)
ENDIF()


# guard against in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  MESSAGE(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

PROJECT(TPS_HoleFilling)

FIND_PACKAGE(VTK REQUIRED)

INCLUDE(${VTK_USE_FILE})

# IF(WIN32)
  # SET(ENV{PATH} ${VTK_DIR};$ENV{PATH})
# ENDIF()

#SET(CMAKE_CXX_STANDARD 11)

INCLUDE_DIRECTORIES(include)
SET(source_files 
	src/TPS_HoleFilling.cpp
	src/PickingInteractorStyle.cpp
	src/PickingWindow.cpp
	)
SET(include_files
	include/common.h
	src/PickingInteractorStyle.h
	src/PickingWindow.h
	)

ADD_EXECUTABLE(TPS_HoleFilling MACOSX_BUNDLE ${source_files} ${include_files})

IF(VTK_LIBRARIES)
  TARGET_LINK_LIBRARIES(TPS_HoleFilling ${VTK_LIBRARIES})
ELSE()
  TARGET_LINK_LIBRARIES(TPS_HoleFilling vtkHybrid vtkWidgets)
ENDIF()
