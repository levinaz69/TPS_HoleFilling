#pragma once
#include "common.h"
#include "utilities.h"
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
	void SetModelFile(const std::string filename);
	void SetMarkerFile(const std::string filename);
	unsigned long LoadModelFile();
	unsigned long LoadMarkerFile();
	void Render();
	void SetInteractorStyle();
	void WriteMarkerFile();
	void SetMarkerSize(double size);
	void AdjustMarkerSize(int step);
	void PickingCallback(const int* clickPos);
	void SetOperaringMode(const OPERATING_MODE mode);
	long GetMarkerIndex(vtkSmartPointer<vtkActor> actor);
	void CreateMarker(double* pos);
	void CreateMarker(vtkPoints* points);
	void SetCurrentMarker(long index);
	void SetCurrentMarker(vtkSmartPointer<vtkActor> actor);
	void MoveCurrentMarker(double x, double y, double z);
	void RemoveCurrentMarker();

	//protected:
public:
	// Basic
	static unsigned WindowCount;
	unsigned WindowID;
	std::string ModelFilename;
	std::string MarkerFilename;

	// Model
	vtkSmartPointer<vtkPolyDataMapper> ModelMapper;
	vtkSmartPointer<vtkActor> ModelActor;
	vtkSmartPointer<vtkActorCollection> ModelActorCollection;
	double ModelScale;	// scale of the model, which is the min bound size
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
	vtkSmartPointer<vtkTextActor> MarkerNumberIndicatorActor;

};

