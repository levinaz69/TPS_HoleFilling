#include "PickingWindow.h"
#include "PickingInteractorStyle.h"

std::list<PickingWindow*> PickingWindow::PickingWindows;
bool PickingWindow::OperatingSimultaneously = false;

PickingWindow::PickingWindow()
{
	PickingWindows.push_back(this);
	WindowID = PickingWindows.size() - 1;
}

PickingWindow::~PickingWindow()
{
	PickingWindows.remove(this);
}

void PickingWindow::Initialize()
{
	// Model
	ModelMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	ModelActor = vtkSmartPointer<vtkActor>::New();
	ModelActorCollection = vtkSmartPointer<vtkActorCollection>::New();

	// Marker
	MarkerSphereSource = vtkSmartPointer<vtkSphereSource>::New();
	MarkerSphereSource->SetCenter(0, 0, 0);
	MarkerSphereSource->SetRadius(0.1);		// default marker size
	MarkerSphereMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	MarkerSphereMapper->SetInputConnection(MarkerSphereSource->GetOutputPort());
	MarkerActorCollection = vtkSmartPointer<vtkActorCollection>::New();
	CurrentMarkerIndex = NULL_MARKER_INDEX;

	// Renderer
	Renderer = vtkSmartPointer<vtkRenderer>::New();
	Renderer->SetBackground(0, 0, 0);		// default background: black

	// RenderWindow
	RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	RenderWindow->AddRenderer(Renderer);
	//// size
	auto ssize = RenderWindow->GetScreenSize();
	RenderWindow->SetSize(ssize[1] / 2, ssize[1] / 2);
	RenderWindow->SetPosition((ssize[1]) * WindowID, 0);

	// Interactor
	RenderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	RenderWindowInteractor->SetRenderWindow(RenderWindow);
	SetInteractorStyle();
	
	// Setup OperatingMode and its indicator
	ModeIndicatorActor = vtkSmartPointer<vtkTextActor>::New();
	ModeIndicatorActor->SetPosition(0, 0);
	ModeIndicatorActor->GetTextProperty()->SetFontSize(16);
	ModeIndicatorActor->GetTextProperty()->SetColor(1.0, 1.0, 1.0);
	Renderer->AddActor2D(ModeIndicatorActor);

	// Setup marker number indicator
	MarkerNumberIndicatorActor = vtkSmartPointer<vtkTextActor>::New();
	MarkerNumberIndicatorActor->SetPosition(0, 16);
	MarkerNumberIndicatorActor->GetTextProperty()->SetFontSize(16);
	MarkerNumberIndicatorActor->GetTextProperty()->SetColor(1.0, 1.0, 1.0);
	Renderer->AddActor2D(MarkerNumberIndicatorActor);
	
	// Default Mode
	//OperatingSimultaneously = false;
	SetOperaringMode(APPEND_MODE);
	SetCurrentMarker(NULL_MARKER_INDEX);

	// Set RenderWindow title
	std::ostringstream oss;
	oss << "Window " << WindowID;
	RenderWindow->SetWindowName(oss.str().c_str());
}

void PickingWindow::SetModelFile(const std::string filename)
{
	ModelFilename = filename;
	// Auto-generate marker filename
	auto baseFilename = Utility::GetBaseFilename(ModelFilename.c_str());
	MarkerFilename = baseFilename + "_markers.xyz";
}
void PickingWindow::SetMarkerFile(const std::string filename)
{
	MarkerFilename = filename;
}

unsigned long PickingWindow::LoadModelFile()
{
	if (!Utility::IsFileExist(ModelFilename.c_str()))
		return vtkErrorCode::FileNotFoundError;
	auto plyReader = vtkSmartPointer<vtkPLYReader>::New();
	plyReader->SetFileName(ModelFilename.c_str());
	if (plyReader->GetErrorCode() != vtkErrorCode::NoError)
		return plyReader->GetErrorCode();
//#ifdef _DEBUG
	std::cout << WindowID << " Load model file: " << ModelFilename << std::endl;
//#endif

	ModelMapper->SetInputConnection(plyReader->GetOutputPort());
	ModelActor->SetMapper(ModelMapper);
	ModelActorCollection->AddItem(ModelActor);
	Renderer->AddActor(ModelActor);

	// Reset marker size according to model
	double* bound = ModelMapper->GetBounds();
	ModelScale = std::min(std::min(bound[1] - bound[0], bound[3] - bound[2]), bound[5] - bound[4]);
	SetMarkerSize((ModelScale / 100.0) * 2);

	Renderer->ResetCamera();
	Render();
	return vtkErrorCode::NoError;
}

unsigned long PickingWindow::LoadMarkerFile()
{
	if (!Utility::IsFileExist(MarkerFilename.c_str()))
		return vtkErrorCode::FileNotFoundError;
	auto simpleReader = vtkSmartPointer<vtkSimplePointsReader>::New();
	simpleReader->SetFileName(MarkerFilename.c_str());
	if (simpleReader->GetErrorCode() != vtkErrorCode::NoError)
		return simpleReader->GetErrorCode();
//#ifdef _DEBUG
	std::cout << WindowID << " Load marker file: " << MarkerFilename << std::endl;
//#endif

	auto markerMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	markerMapper->SetInputConnection(simpleReader->GetOutputPort());
	markerMapper->Update();
	auto markerPointPolyData = markerMapper->GetInput();
	AppendMarker(markerPointPolyData->GetPoints());

	SetOperaringMode(SELECT_MODE);
	Render();
	return vtkErrorCode::NoError;
}

void PickingWindow::WriteMarkerFile()
{
	auto markerPoints = vtkSmartPointer<vtkPoints>::New();
	for (auto idx = 0; idx < MarkerActors.size(); idx++)
	{
		assert(MarkerActors[idx] != nullptr);
		markerPoints->InsertNextPoint(MarkerActors[idx]->GetPosition());
	}
	auto markerPointPolyData = vtkSmartPointer<vtkPolyData>::New();
	markerPointPolyData->SetPoints(markerPoints);

	auto simpleWriter = vtkSmartPointer<vtkSimplePointsWriter>::New();
	simpleWriter->SetFileName(MarkerFilename.c_str());
	simpleWriter->SetInputData(markerPointPolyData);
	simpleWriter->Write();

	//auto plyWriter = vtkSmartPointer<vtkPLYWriter>::New();
	//plyWriter->SetFileName(MarkerFilename.c_str());
	//plyWriter->SetInputData(markerPointData);
	//plyWriter->SetFileTypeToASCII();
	//plyWriter->Write();
#ifdef _DEBUG
	std::cout << WindowID << " Output marker file: " << MarkerFilename << std::endl;
#endif
}

void PickingWindow::Render()
{
	// Render now
	RenderWindow->Render();

	//Renderer->ResetCamera();
	//RenderWindowInteractor->Start();

}

vtkStandardNewMacro(PickingInteractorStyle);
void PickingWindow::SetInteractorStyle()
{
	InteractorStyle = vtkSmartPointer<PickingInteractorStyle>::New();
	// Set callback functions
	InteractorStyle->SetPickingCallback(
		std::bind(&PickingWindow::PickingCallback, this, std::placeholders::_1) );
	InteractorStyle->SetModeSelectingCallback(
		std::bind(&PickingWindow::SetOperaringMode, this, std::placeholders::_1) );
	InteractorStyle->SetRemoveCurrentMarkerCallback(
		std::bind(&PickingWindow::RemoveCurrentMarker, this) );
	InteractorStyle->SetAdjustMarkerSizeCallback(
		std::bind(&PickingWindow::AdjustMarkerSize, this, std::placeholders::_1) );
	InteractorStyle->SetWriteMarkerFileCallback(
		std::bind(&PickingWindow::WriteMarkerFile, this));
	InteractorStyle->SetSelectToggleOperatingSimultaneouslyCallback(
		std::bind(&PickingWindow::ToggleOperatingSimultaneously, this));
	InteractorStyle->SetSelectPrevMarkerCallback(
		std::bind(&PickingWindow::SelectPrevMarker, this));
	InteractorStyle->SetSelectNextMarkerCallback(
		std::bind(&PickingWindow::SelectNextMarker, this));

	InteractorStyle->SetDefaultRenderer(Renderer);
	RenderWindowInteractor->SetInteractorStyle(InteractorStyle);
}

void PickingWindow::ToggleOperatingSimultaneously()
{
	OperatingSimultaneously = !OperatingSimultaneously;
	for (auto iter = PickingWindows.begin(); iter != PickingWindows.end(); ++iter)
	{
		(*iter)->SetOperaringMode(OperatingMode);
	}
}

void PickingWindow::SetMarkerSize(double size)
{
	MarkerSphereSource->SetRadius(size);
	this->Render();
}

void PickingWindow::AdjustMarkerSize(int step)
{
	double radius = MarkerSphereSource->GetRadius();
	double stepSize = ModelScale / 1000.0;
	radius += stepSize * step;
	SetMarkerSize(radius > 0.0? radius : 0.0);
#ifdef _DEBUG
	std::cout << WindowID << " Marker size: " << MarkerSphereSource->GetRadius() << std::endl;
#endif
}

void PickingWindow::PickingCallback(const int* clickPos)
{
	// Pick from this location.

	auto picker = vtkSmartPointer<vtkPropPicker>::New();
	picker->PickProp(clickPos[0], clickPos[1], Renderer, 
		OperatingMode == SELECT_MODE ? MarkerActorCollection : ModelActorCollection);

	double* picked_pos = picker->GetPickPosition();
	vtkActor* picked_actor = picker->GetActor();

#ifdef _DEBUG
	std::cout << WindowID <<" Pick position (world coordinates) is: "
		<< picked_pos[0] << " " << picked_pos[1]
		<< " " << picked_pos[2] << std::endl;
	std::cout << WindowID << " Picked actor: " << picked_actor << std::endl;
#endif
#if _DEBUG
	std::cout << WindowID << " Current mode:" << OperatingMode << std::endl;
#endif
	if (picked_actor != nullptr)
	{
		if (OperatingMode == SELECT_MODE)
		{
			SetCurrentMarker(picked_actor);
			//// Next: move selected actor
			//OperatingMode = MOVE_MODE;
		}
		else if (OperatingMode == APPEND_MODE)
		{
			AppendMarker(picked_pos);
		}
		else if (OperatingMode == INSERT_MODE)
		{
			InsertMarker(CurrentMarkerIndex, picked_pos);
		}
		else if (OperatingMode == MOVE_MODE)
		{
			MoveCurrentMarker(picked_pos[0], picked_pos[1], picked_pos[2]);
		}
	}

}

void PickingWindow::SetOperaringMode(const OPERATING_MODE mode)
{
	std::string indicatorText;
	if (mode == SELECT_MODE)
	{
		OperatingMode = SELECT_MODE;
		indicatorText = "Select";
	}
	else if (mode == APPEND_MODE)
	{
		OperatingMode = APPEND_MODE;
		indicatorText = "Append";
	}
	else if (mode == INSERT_MODE)
	{
		OperatingMode = INSERT_MODE;
		indicatorText = "Insert";
	}
	else if (mode == MOVE_MODE)
	{
		OperatingMode = MOVE_MODE;
		indicatorText = "Move";
	}
	if (OperatingSimultaneously)
	{
		indicatorText += " [S]";
	}
	ModeIndicatorActor->SetInput(indicatorText.c_str());
	Render();
//#if _DEBUG
//	std::cout << WindowID << " Set Current mode to " << this->OperatingMode << std::endl;
//#endif
}

long PickingWindow::GetMarkerIndex(const vtkSmartPointer<vtkActor> actor)
{
	if (actor != nullptr)
	{
		for (auto idx = 0; idx < MarkerActors.size(); idx++)
		{
			if (MarkerActors[idx] == actor)
				return idx;
		}
	}
	return NULL_MARKER_INDEX;
}


void PickingWindow::AppendMarker(double* pos)
{
	InsertMarker(NULL_MARKER_INDEX, pos);	// append
}

void PickingWindow::AppendMarker(vtkPoints* points)
{
	InsertMarker(NULL_MARKER_INDEX, points);	// append
}

void PickingWindow::InsertMarker(long index, double* pos)
{
	assert(index >= NULL_MARKER_INDEX && index <= MarkerActors.size());
	if (index == NULL_MARKER_INDEX)		// Append
		index = MarkerActors.size();

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(MarkerSphereMapper);
	actor->SetPosition(pos[0], pos[1], pos[2]);

	//Add actor to list
	if (CurrentMarkerIndex >= index)
		CurrentMarkerIndex = index + 1;		// previous marker
	MarkerActors.insert(MarkerActors.begin() + index, actor);
	SetCurrentMarker(index);
	MarkerActorCollection->AddItem(actor);

	//Add actor to renderer
	Renderer->AddActor(actor);
	Render();

#ifdef _DEBUG
	std::cout << WindowID << " Add marker: " << idx << std::endl;
	//std::cout << WindowID << " Current marker #: " << MarkerActors.size() << std::endl;
#endif
}

void PickingWindow::InsertMarker(long index, vtkPoints* points)
{
	assert(index >= NULL_MARKER_INDEX && index <= MarkerActors.size());
	if (index == NULL_MARKER_INDEX)		// Append
		index = MarkerActors.size();

	if (points->GetNumberOfPoints() == 0)
		return;

	auto pointsNum = points->GetNumberOfPoints();
	std::vector< vtkSmartPointer<vtkActor> > actors(pointsNum);
	for (vtkIdType i = 0; i < pointsNum; i++)
	{
		double xyz[3];
		points->GetPoint(i, xyz);

		vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(MarkerSphereMapper);
		actor->SetPosition(xyz[0], xyz[1], xyz[2]);
		actor->GetProperty()->SetColor(MARKER_COLOR[0], MARKER_COLOR[1], MARKER_COLOR[2]);

		actors[i] = actor;		
		MarkerActorCollection->AddItem(actor);
		Renderer->AddActor(actor);
	}
	if (CurrentMarkerIndex >= index)
		CurrentMarkerIndex = index + pointsNum;		// previous marker
	MarkerActors.insert(MarkerActors.begin() + index, actors.begin(), actors.end());
	auto idx = index + pointsNum - 1;

#ifdef _DEBUG
	std::cout << WindowID << " Add marker: " << idx << std::endl;
	//std::cout << WindowID << " Current marker #: " << MarkerActors.size() << std::endl;
#endif

	SetCurrentMarker(idx);
	Render();
}


void PickingWindow::SelectPrevMarker()
{
	auto index = CurrentMarkerIndex;
	index -= 1;
	if (index >= 0 && index < MarkerActors.size())
		SelectMarker(index);
}
void PickingWindow::SelectNextMarker()
{
	auto index = CurrentMarkerIndex;
	index += 1;
	if (index >= 0 && index < MarkerActors.size())
		SelectMarker(index);
}


void PickingWindow::SelectMarker(long index)
{
	if (OperatingSimultaneously)
	{
		for (auto iter = PickingWindows.begin(); iter != PickingWindows.end(); ++iter)
		{
			(*iter)->SetCurrentMarker(index);
		}
	}
	else
	{
		SetCurrentMarker(index);
	}
}

void PickingWindow::SetCurrentMarker(long index)
{
	if (CurrentMarkerIndex != NULL_MARKER_INDEX && CurrentMarkerIndex < MarkerActors.size())
	{
		auto& currentMarkerActor = MarkerActors[CurrentMarkerIndex];
		currentMarkerActor->GetProperty()->
			SetColor(MARKER_COLOR[0], MARKER_COLOR[1], MARKER_COLOR[2]);
	}
	if (index != NULL_MARKER_INDEX && index < MarkerActors.size())
	{
		MarkerActors[index]->GetProperty()->
			SetColor(MARKER_COLOR_SELECTED[0], MARKER_COLOR_SELECTED[1], MARKER_COLOR_SELECTED[2]);
		CurrentMarkerIndex = index;
	}
	else
		CurrentMarkerIndex = NULL_MARKER_INDEX;
	// Refresh marker number indicator
	std::ostringstream oss;
	oss << MarkerActors.size() << " [" << CurrentMarkerIndex << "]";
	MarkerNumberIndicatorActor->SetInput(oss.str().c_str());
	// Refresh render
	Render();
//#ifdef _DEBUG
//	std::cout << WindowID << " Current marker: " << CurrentMarkerIndex << std::endl;
//#endif
}

void PickingWindow::SetCurrentMarker(vtkSmartPointer<vtkActor> actor)
{
	SetCurrentMarker(GetMarkerIndex(actor));
}

void PickingWindow::MoveCurrentMarker(double x, double y, double z)
{
	if (CurrentMarkerIndex != NULL_MARKER_INDEX)
	{
		MarkerActors[CurrentMarkerIndex]->SetPosition(x, y, z);
	}
	Render();
}

void PickingWindow::RemoveCurrentMarker()
{
	if (CurrentMarkerIndex != NULL_MARKER_INDEX)
	{
		auto& currentMarkerActor = MarkerActors[CurrentMarkerIndex];
		MarkerActorCollection->RemoveItem(currentMarkerActor);
		Renderer->RemoveActor(currentMarkerActor);

		auto nextActorIter = MarkerActors.erase(MarkerActors.begin() + CurrentMarkerIndex);

#ifdef _DEBUG
		std::cout << WindowID << " Delete marker: " << CurrentMarkerIndex << std::endl;
#endif

		if (nextActorIter == MarkerActors.end())
		{
			if (nextActorIter != MarkerActors.begin())
			{
				SetCurrentMarker(--nextActorIter - MarkerActors.begin());
			}
			else
			{
				SetCurrentMarker(NULL_MARKER_INDEX);
			}
		}

		Render();

	}
//#ifdef _DEBUG
//	std::cout << WindowID << " Current marker #: " << MarkerActors.size() << std::endl;
//#endif
}
