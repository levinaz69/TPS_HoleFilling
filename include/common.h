#pragma once

#include <vtkPolyData.h>
#include <vtkPLYReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkCollectionIterator.h>

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>

#include <sstream>
#include <vector>
#include <functional>

//#include <Eigen/Dense>
//#include <Eigen/Sparse>

using namespace std;


const double MARKER_COLOR[3] = { 0.0, 1.0, 0.0 };
const double MARKER_COLOR_SELECTED[3] = { 1.0, 0.0, 0.0 };
enum OPERATING_MODE
{
	SELECT_MODE,
	CREATE_MODE,
	MOVE_MODE
};

//// Geometry data types
//typedef double Scalar;
//typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
//typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> Vector;
//
//typedef Eigen::Matrix<Scalar, 2, 2> Matrix2D;
//typedef Eigen::Matrix<Scalar, 2, 1> Vector2D;
//
//typedef Vector2D Point2D;
//typedef Vector2D Normal2D;
//
//typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 2> PointSet2D;
//typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 2> NormalSet2D;
//
//typedef Eigen::Matrix<Scalar, 3, 3> Matrix3D;
//typedef Eigen::Matrix<Scalar, 3, 1> Vector3D;
//
//typedef Eigen::Matrix<Scalar, 4, 4> Matrix4D;
//typedef Eigen::Matrix<Scalar, 4, 1> Vector4D;
//
//typedef Vector3D Point3D;
//typedef Vector3D Normal3D;
//
//typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 3> PointSet3D;
//typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 3> NormalSet3D;
//
//typedef Eigen::Transform<Scalar, 3, Eigen::Affine> Affine3D;
//typedef Eigen::Translation<Scalar, 3> Translation3D;
//typedef Eigen::AngleAxis<Scalar> AngleAxis;
//typedef Eigen::Quaternion<Scalar> Quaternion;
//
//typedef Eigen::SparseMatrix<Scalar> SparseMatrix;
//typedef Eigen::Triplet<Scalar> Triplet;
//typedef std::vector<Triplet> TripletList;

