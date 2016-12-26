#include "PickingWindow.h"



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
	MarkerSphereSource->SetRadius(0.02);		// default marker size
	MarkerSphereMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	MarkerSphereMapper->SetInputConnection(MarkerSphereSource->GetOutputPort());
	MarkerActorCollection = vtkSmartPointer<vtkActorCollection>::New();
	CurrentMarkerIndex = NULL_MARKER_INDEX;

	// Marker Labels
	MarkerPoints = vtkSmartPointer<vtkPoints>::New();
	MarkerLabelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
	MarkerPointPolyData = vtkSmartPointer<vtkPolyData>::New();
	MarkerPointPolyData->SetPoints(MarkerPoints);
	MarkerLabelMapper->SetInputData(MarkerPointPolyData);

	MarkerLabelActor = vtkSmartPointer<vtkActor2D>::New();
	MarkerLabelActor->SetMapper(MarkerLabelMapper);
	Renderer->AddActor(MarkerLabelActor);

	// Renderer
	Renderer = vtkSmartPointer<vtkRenderer>::New();
	Renderer->SetBackground(0, 0, 0);		// default background: black

	// RenderWindow
	RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	RenderWindow->AddRenderer(Renderer);

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
	std::cout << "Pick position (world coordinates) is: "
		<< picked_pos[0] << " " << picked_pos[1]
		<< " " << picked_pos[2] << std::endl;
	std::cout << "Picked actor: " << picked_actor << std::endl;
#endif
#if _DEBUG
	std::cout << "Current mode:" << OperatingMode << std::endl;
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
	std::cout << "Set Current mode to " << this->OperatingMode << std::endl;
#endif
}

int PickingWindow::GetMarkerIndex(const vtkSmartPointer<vtkActor> actor)
{
	if (actor != nullptr)
	{
		int idx;
		for (idx = 0; idx < MarkerActors.size(); idx++)
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
	MarkerActorCollection->AddItem(actor);
	MarkerActors.push_back(actor);
	int idx = MarkerActors.size() - 1;
	SetCurrentMarker(idx);

	//Add point to list
	MarkerPoints->InsertNextPoint(pos);

	//Add actor to renderer
	Renderer->AddActor(actor);
	this->Render();

#ifdef _DEBUG
	std::cout << "Current marker #:" << idx << std::endl;
#endif

}

void PickingWindow::SetCurrentMarker(int index)
{
	if (index == NULL_MARKER_INDEX)
		return;

	if (CurrentMarkerIndex != NULL_MARKER_INDEX)
	{
		auto& currentMarkerActor = MarkerActors[CurrentMarkerIndex];
		currentMarkerActor->GetProperty()->
			SetColor(MARKER_COLOR[0], MARKER_COLOR[1], MARKER_COLOR[2]);
	}
	CurrentMarkerIndex = index;
	MarkerActors[CurrentMarkerIndex]->GetProperty()->
		SetColor(MARKER_COLOR_SELECTED[0], MARKER_COLOR_SELECTED[1], MARKER_COLOR_SELECTED[2]);

#ifdef _DEBUG
	std::cout << "Current actor: " << MarkerActors[index].GetPointer() << std::endl;
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
		MarkerPoints->SetPoint(CurrentMarkerIndex, x, y, z);
	}
}


void PickingWindow::RemoveCurrentMarker()
{
	// TODO
	if (CurrentMarkerIndex != NULL_MARKER_INDEX)
	{
		auto& currentMarkerActor = MarkerActors[CurrentMarkerIndex];
		MarkerActorCollection->RemoveItem(CurrentMarkerIndex);
		MarkerActors.erase();
		Renderer->RemoveActor(currentMarkerActor);

		auto nextActor = MarkerActorCollection->GetNextActor();
		if (nextActor == nullptr)
			nextActor = MarkerActorCollection->GetLastActor();

		SetCurrentMarker(nextActor);
		this->Render();

#ifdef _DEBUG
		std::cout << "Delete marker:" << CurrentMarkerIndex << std::endl;
#endif


		MarkerActorCollection->InitTraversal();
		vtkSmartPointer<vtkActor> curActor;
		for (int idx = 0; idx < MarkerActorCollection->GetNumberOfItems(); idx++)
		{
			if ((curActor = MarkerActorCollection->GetNextActor()) != nullptr)
			{
				if (curActor.Get() == currentMarkerActor.Get())
				{
					MarkerActorCollection->RemoveItem(idx);
					Renderer->RemoveActor(currentMarkerActor);

#ifdef _DEBUG
					std::cout << "Delete marker:" << idx << std::endl;
#endif

					auto nextActor = MarkerActorCollection->GetNextActor();
					if (nextActor == nullptr)
						nextActor = MarkerActorCollection->GetLastActor();

					SetCurrentMarker(nextActor);
					this->Render();
					break;
				}
			}
		}
		
	}
#ifdef _DEBUG
	std::cout << "Current marker #:" << MarkerActorList->GetNumberOfItems() << std::endl;
#endif
}
