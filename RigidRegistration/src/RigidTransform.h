#pragma once

#include "common/common.h"
#include "common/utilities.h"

#include <string>

#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkLandmarkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkSimplePointsReader.h>

#include <vtkArrayWriter.h>
#include <vtkDenseArray.h>
#include <vtkArrayData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPLYReader.h>
#include <vtkGlyph3D.h>

class TransfromInteractorStyle;

class RigidTransform
{
public:
	void CalcTransform();
	void ApplyTransform();
	vtkSmartPointer<vtkPolyData> ApplyTransform(vtkSmartPointer<vtkPolyData> polyData);
	void ShowTransformMatrix();
	void SaveTransformMatrix();
	vtkSmartPointer<vtkActor> GetPointsActor(vtkPolyData* pointsPolyData);

	void VisualizeInit();
	void Visualize();
	vtkRenderWindowInteractor* GetRenderWindowInteractor() const { return RenderWindowInteractor; }

public:
	vtkSmartPointer<vtkPolyData> SourceMarker;
	vtkSmartPointer<vtkPolyData> TargetMarker;
	vtkSmartPointer<vtkPolyData> SolutionMarker;

	vtkSmartPointer<vtkPoints> SourceMarkerPoints;
	vtkSmartPointer<vtkPoints> TargetMarkerPoints;
	vtkSmartPointer<vtkPoints> SolutionMarkerPoints;

	vtkSmartPointer<vtkPolyData> SourceModel;
	vtkSmartPointer<vtkPolyData> TargetModel;
	vtkSmartPointer<vtkPolyData> SolutionModel;

	vtkSmartPointer<vtkLandmarkTransform> Transform;

	vtkSmartPointer<vtkRenderer> Renderer;
	vtkSmartPointer<vtkRenderWindow> RenderWindow;
	vtkSmartPointer<vtkRenderWindowInteractor> RenderWindowInteractor;
	vtkSmartPointer<TransfromInteractorStyle> InteractorStyle;

};