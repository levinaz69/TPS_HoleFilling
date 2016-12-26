#pragma once
#include "common.h"
#include "PickingInteractorStyle.h"

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
	//protected:
public:
	
	vtkSmartPointer<vtkPLYReader> PLYReader;
	vtkSmartPointer<vtkPolyDataMapper> ModelMapper;
	vtkSmartPointer<vtkActor> ModelActor;
	vtkSmartPointer<vtkActorCollection> ModelActorList;

	vtkSmartPointer<vtkSphereSource> MarkerSource;
	vtkSmartPointer<vtkPolyDataMapper> MarkerMapper;
	vtkSmartPointer<vtkActorCollection> MarkerActorList;
	vtkSmartPointer<vtkActor> CurrentMarkerActor;

	vtkSmartPointer<vtkRenderer> Renderer;
	vtkSmartPointer<vtkRenderWindow> RenderWindow;
	vtkSmartPointer<vtkRenderWindowInteractor> RenderWindowInteractor;
	vtkSmartPointer<PickingInteractorStyle> InteractorStyle;

	void CreateMarker(double* pos);
	void SetCurrentMarker(vtkSmartPointer<vtkActor> actor);
	void RemoveCurrentMarker();
};

