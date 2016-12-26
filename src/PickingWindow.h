#pragma once
#include "common.h"
#include "PickingInteractorStyle.h"

#define NULL_MARKER_INDEX -1

class PickingInteractorStyle;

class PickingWindow
{
public:
	OPERATING_MODE OperatingMode;

	PickingWindow();
	~PickingWindow();

	void Initialize();
	void LoadPLY(const std::string inputFilename);
	void WritePLY(const std::string outputFilename);
	void Render();
	void SetInteractorStyle();

	void SetMarkerSize(double size);
	void PickingCallback(const int* clickPos);
	void SetOperaringMode(const OPERATING_MODE mode);
	long GetMarkerIndex(vtkSmartPointer<vtkActor> actor);
	void CreateMarker(double* pos);
	void SetCurrentMarker(long index);
	void SetCurrentMarker(vtkSmartPointer<vtkActor> actor);
	void MoveCurrentMarker(double x, double y, double z);
	void RemoveCurrentMarker();

	//protected:
public:
	// Basic
	static unsigned WindowCount;
	unsigned WindowID;

	// Model
	vtkSmartPointer<vtkPLYReader> PLYReader;
	vtkSmartPointer<vtkPolyDataMapper> ModelMapper;
	vtkSmartPointer<vtkActor> ModelActor;
	vtkSmartPointer<vtkActorCollection> ModelActorCollection;
	double Scale;	// scale of the model, which is the min bound size
	// Marker
	vtkSmartPointer<vtkSphereSource> MarkerSphereSource;
	vtkSmartPointer<vtkPolyDataMapper> MarkerSphereMapper;

	long CurrentMarkerIndex;
	std::vector< vtkSmartPointer<vtkActor> > MarkerActors;
	vtkSmartPointer<vtkActorCollection> MarkerActorCollection;	// contains same actors, possibly in different order

	// Render
	vtkSmartPointer<vtkRenderer> Renderer;
	vtkSmartPointer<vtkRenderWindow> RenderWindow;
	vtkSmartPointer<vtkRenderWindowInteractor> RenderWindowInteractor;
	vtkSmartPointer<PickingInteractorStyle> InteractorStyle;
	// Others
	vtkSmartPointer<vtkTextActor> ModeIndicatorActor;

};

