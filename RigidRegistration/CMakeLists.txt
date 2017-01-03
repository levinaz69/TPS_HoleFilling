cmake_minimum_required(VERSION 2.8.12)

PROJECT(RigidRegistration)

MESSAGE(STATUS ${PROJECT_NAME} " BINARY dir " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS ${PROJECT_NAME} " SOURCE dir " ${PROJECT_SOURCE_DIR})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)

FIND_PACKAGE(Eigen3 REQUIRED)
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

SET(gmnr_libraries
	leastsquares
)


SET(source_files
	src/RigidRegistration.cpp
	src/RigidTransform.cpp
)
SET(include_files
	src/RigidTransform.h
	src/TransformInteractorStyle.h
)

#ADD_EXECUTABLE(${PROJECT_NAME} MACOSX_BUNDLE ${source_files} ${include_files})
ADD_EXECUTABLE(${PROJECT_NAME} ${source_files} ${include_files})
ADD_DEPENDENCIES(${PROJECT_NAME} ${gmnr_libraries})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${VTK_LIBRARIES} ${gmnr_libraries})
