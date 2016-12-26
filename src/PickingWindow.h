#pragma once
#include "common.h"
#include "PickingInteractorStyle.h"

#define NULL_MARKER_INDEX -1

class PickingInteractorStyle;

class PickingWindow
{
public:

	OPERATING_MODE OperatingMode;

	PickingWindow() = default;
	~PickingWindow() = default;

	void Initialize();
	void LoadPLY(const std::string inputFilename);
	void Render();
	void SetInteractorStyle();

	void SetMarkerSize(double size);
	void PickingCallback(const int* clickPos);
	void SetOperaringMode(const OPERATING_MODE mode);
	int GetMarkerIndex(vtkSmartPointer<vtkActor> actor);
	//protected:
public:
	// Model
	vtkSmartPointer<vtkPLYReader> PLYReader;
	vtkSmartPointer<vtkPolyDataMapper> ModelMapper;
	vtkSmartPointer<vtkActor> ModelActor;
	vtkSmartPointer<vtkActorCollection> ModelActorCollection;
	// Marker
	vtkSmartPointer<vtkSphereSource> MarkerSphereSource;
	vtkSmartPointer<vtkPolyDataMapper> MarkerSphereMapper;

	int CurrentMarkerIndex;
	std::vector< vtkSmartPointer<vtkActor> > MarkerActors;	// position
	vtkSmartPointer<vtkActorCollection> MarkerActorCollection;

	vtkSmartPointer<vtkPoints> MarkerPoints;	// position
	vtkSmartPointer<vtkPolyData> MarkerPointPolyData;
	vtkSmartPointer<vtkLabeledDataMapper> MarkerLabelMapper;
	vtkSmartPointer<vtkActor2D> MarkerLabelActor;
	// Render
	vtkSmartPointer<vtkRenderer> Renderer;
	vtkSmartPointer<vtkRenderWindow> RenderWindow;
	vtkSmartPointer<vtkRenderWindowInteractor> RenderWindowInteractor;
	vtkSmartPointer<PickingInteractorStyle> InteractorStyle;
	// Others
	vtkSmartPointer<vtkTextActor> ModeIndicatorActor;

	void CreateMarker(double* pos);
	void SetCurrentMarker(int index);
	void SetCurrentMarker(vtkSmartPointer<vtkActor> actor);
	void MoveCurrentMarker(double x, double y, double z);
	void RemoveCurrentMarker();
};

