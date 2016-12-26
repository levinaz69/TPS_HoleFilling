#include "PickingWindow.h"

unsigned PickingWindow::WindowCount = 0;

PickingWindow::PickingWindow()
{
	WindowID = WindowCount++; 
}

PickingWindow::~PickingWindow()
{

}

void PickingWindow::Initialize()
{
	// Model
	PLYReader = vtkSmartPointer<vtkPLYReader>::New();
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

	ostringstream oss;
	oss << "Window" << WindowID;
	RenderWindow->SetWindowName(oss.str().c_str());
	auto ssize = RenderWindow->GetScreenSize();
	RenderWindow->SetSize(ssize[1] / 2, ssize[1] / 2);
	RenderWindow->SetPosition((ssize[1]) * WindowID, 0);


	// Interactor
	RenderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	RenderWindowInteractor->SetRenderWindow(RenderWindow);
	SetInteractorStyle();
	
	// Setup OperatingMode and its indicator
	ModeIndicatorActor = vtkSmartPointer<vtkTextActor>::New();
	ModeIndicatorActor->SetPosition2(10, 40);
	ModeIndicatorActor->GetTextProperty()->SetFontSize(16);
	ModeIndicatorActor->GetTextProperty()->SetColor(1.0, 1.0, 1.0);
	Renderer->AddActor2D(ModeIndicatorActor);
	SetOperaringMode(CREATE_MODE);

}

void PickingWindow::LoadPLY(const std::string inputFilename)
{
	PLYReader->SetFileName(inputFilename.c_str());
	ModelMapper->SetInputConnection(PLYReader->GetOutputPort());
	ModelActor->SetMapper(ModelMapper);
	ModelActorCollection->AddItem(ModelActor);
	Renderer->AddActor(ModelActor);
	Renderer->ResetCamera();

	// Reset marker size according to model
	double* bound = ModelMapper->GetBounds();
	double Scale = min(min(bound[1] - bound[0], bound[3] - bound[2]), bound[5] - bound[4]);
	SetMarkerSize((Scale * 0.01) * 1.5);
}

void PickingWindow::WritePLY(const std::string outputFilename)
{
	auto markerPoints = vtkSmartPointer<vtkPoints>::New();
	for (auto idx = 0; idx < MarkerActors.size(); idx++)
	{
		markerPoints->InsertNextPoint(MarkerActors[idx]->GetPosition());
	}
	auto markerPointPolyData = vtkSmartPointer<vtkPolyData>::New();
	markerPointPolyData->SetPoints(markerPoints);

	auto plyWriter = vtkSmartPointer<vtkPLYWriter>::New();
	plyWriter->SetFileName(outputFilename.c_str());
	plyWriter->SetInputData(markerPointPolyData);
	plyWriter->Write();
}

void PickingWindow::Render()
{
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

	InteractorStyle->SetDefaultRenderer(Renderer);
	RenderWindowInteractor->SetInteractorStyle(InteractorStyle);
}

void PickingWindow::SetMarkerSize(double size)
{
	MarkerSphereSource->SetRadius(size);
	this->Render();
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
		else if (OperatingMode == CREATE_MODE)
		{
			CreateMarker(picked_pos);
		}
		else if (OperatingMode == MOVE_MODE)
		{
			MoveCurrentMarker(picked_pos[0], picked_pos[1], picked_pos[2]);
		}
	}

}

void PickingWindow::SetOperaringMode(const OPERATING_MODE mode)
{
	if (mode == SELECT_MODE)
	{
		OperatingMode = SELECT_MODE;
		ModeIndicatorActor->SetInput("Select");
	}
	else if (mode == CREATE_MODE)
	{
		OperatingMode = CREATE_MODE;
		ModeIndicatorActor->SetInput("Add");
	}
	else if (mode == MOVE_MODE)
	{
		OperatingMode = MOVE_MODE;
		ModeIndicatorActor->SetInput("Move");
	}
	this->Render();
#if _DEBUG
	std::cout << WindowID << " Set Current mode to " << this->OperatingMode << std::endl;
#endif
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

void PickingWindow::CreateMarker(double* pos)
{
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(MarkerSphereMapper);
	actor->SetPosition(pos[0], pos[1], pos[2]);

	//Add actor to list
	MarkerActors.push_back(actor);
	long idx = MarkerActors.size() - 1;
	SetCurrentMarker(idx);
	MarkerActorCollection->AddItem(actor);


	//Add actor to renderer
	Renderer->AddActor(actor);
	this->Render();

#ifdef _DEBUG
	std::cout << WindowID << " Add marker: " << idx << std::endl;
	std::cout << WindowID << " Current marker #:" << MarkerActors.size() << std::endl;
#endif

}

void PickingWindow::SetCurrentMarker(long index)
{
	if (index != NULL_MARKER_INDEX)
	{
		if (CurrentMarkerIndex != NULL_MARKER_INDEX && CurrentMarkerIndex < MarkerActors.size())
		{
			auto& currentMarkerActor = MarkerActors[CurrentMarkerIndex];
			currentMarkerActor->GetProperty()->
				SetColor(MARKER_COLOR[0], MARKER_COLOR[1], MARKER_COLOR[2]);
		}
		MarkerActors[index]->GetProperty()->
			SetColor(MARKER_COLOR_SELECTED[0], MARKER_COLOR_SELECTED[1], MARKER_COLOR_SELECTED[2]);
	}
	CurrentMarkerIndex = index;
#ifdef _DEBUG
	std::cout << WindowID << " Current actor: " << CurrentMarkerIndex << std::endl;
#endif
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
		std::cout << WindowID << " Delete marker:" << CurrentMarkerIndex << std::endl;
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

		this->Render();

	}
#ifdef _DEBUG
	std::cout << WindowID << " Current marker #:" << MarkerActors.size() << std::endl;
#endif
}
