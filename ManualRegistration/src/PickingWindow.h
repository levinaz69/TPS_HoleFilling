#pragma once
#include "common/common.h"
#include "common/utilities.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <sstream>
#include <vector>
#include <functional>
#include <assert.h>

#include <vtkSmartPointer.h>
#include <vtkPLYReader.h>
#include <vtkPLYWriter.h>
#include <vtkSimplePointsReader.h>
#include <vtkSimplePointsWriter.h>
#include <vtkErrorCode.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkCollectionIterator.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
//#include <vtkLabeledDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>


const long NULL_MARKER_INDEX = -1;
const double MARKER_COLOR[3] = { 0.0, 1.0, 0.0 };
const double MARKER_COLOR_SELECTED[3] = { 1.0, 0.0, 0.0 };
enum OPERATING_MODE
{
	SELECT_MODE,
	APPEND_MODE,
	INSERT_MODE,
	MOVE_MODE
};

class PickingInteractorStyle;

class PickingWindow
{
public:
	PickingWindow();
	~PickingWindow();

	void Initialize();
	void SetModelFile(const std::string filename);
	void SetMarkerFile(const std::string filename);
	unsigned long LoadModelFile();
	unsigned long LoadMarkerFile();
	void Render();
	void SetInteractorStyle();
	void ToggleOperatingSimultaneously();
	void WriteMarkerFile();
	void SetMarkerSize(double size);
	void AdjustMarkerSize(int step);
	void PickingCallback(const int* clickPos);
	void SetOperaringMode(const OPERATING_MODE mode);
	long GetMarkerIndex(vtkSmartPointer<vtkActor> actor);
	void AppendMarker(double* pos);
	void AppendMarker(vtkPoints* points);
	void InsertMarker(long index, double* pos);
	void InsertMarker(long index, vtkPoints* points);
	void SelectPrevMarker();
	void SelectNextMarker();
	void SelectMarker(long index);
	void SetCurrentMarker(long index);
	void SetCurrentMarker(vtkSmartPointer<vtkActor> actor);
	void MoveCurrentMarker(double x, double y, double z);
	void RemoveCurrentMarker();

	vtkRenderWindowInteractor* GetRenderWindowInteractor() const { return RenderWindowInteractor; }

public:
	static std::list<PickingWindow*> PickingWindows;
	unsigned WindowID;
	OPERATING_MODE OperatingMode;
	static bool OperatingSimultaneously;

protected:
	// Basic
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

