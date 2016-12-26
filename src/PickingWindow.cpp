#include "PickingWindow.h"

void PickingWindow::Initialize()
{
	// Model
	PLYReader = vtkSmartPointer<vtkPLYReader>::New();
	ModelMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	ModelActor = vtkSmartPointer<vtkActor>::New();
	ModelActorList = vtkSmartPointer<vtkActorCollection>::New();

	// Marker
	MarkerSource = vtkSmartPointer<vtkSphereSource>::New();
	MarkerSource->SetCenter(0, 0, 0);
	MarkerSource->SetRadius(0.02);		// default marker size
	MarkerMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	MarkerMapper->SetInputConnection(MarkerSource->GetOutputPort());
	MarkerActorList = vtkSmartPointer<vtkActorCollection>::New();

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
	
}

void PickingWindow::LoadPLY(const std::string inputFilename)
{
	PLYReader->SetFileName(inputFilename.c_str());
	ModelMapper->SetInputConnection(PLYReader->GetOutputPort());
	ModelActor->SetMapper(ModelMapper);
	ModelActorList->AddItem(ModelActor);
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
	InteractorStyle->SetPickingCallback(std::bind(&PickingWindow::PickingCallback, this, std::placeholders::_1));
	InteractorStyle->SetModeSelectingCallback(std::bind(&PickingWindow::SetOperaringMode, this, std::placeholders::_1));
	InteractorStyle->SetRemoveCurrentMarkerCallback(std::bind(&PickingWindow::RemoveCurrentMarker, this));

	InteractorStyle->SetDefaultRenderer(Renderer);
	RenderWindowInteractor->SetInteractorStyle(InteractorStyle);
}

void PickingWindow::SetMarkerSize(double size)
{
	MarkerSource->SetRadius(size);
}

void PickingWindow::PickingCallback(const int* clickPos)
{
	// Pick from this location.

	auto picker = vtkSmartPointer<vtkPropPicker>::New();
	picker->PickProp(clickPos[0], clickPos[1], Renderer, OperatingMode == SELECT_MODE ? MarkerActorList : ModelActorList);

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
			CurrentMarkerActor->SetPosition(picked_pos[0], picked_pos[1], picked_pos[2]);
		}
	}

}

void PickingWindow::SetOperaringMode(const OPERATING_MODE mode)
{
	if (mode == SELECT_MODE)
	{
		OperatingMode = SELECT_MODE;
	}
	else if (mode == CREATE_MODE)
	{
		OperatingMode = CREATE_MODE;
	}
	else if (mode == MOVE_MODE)
	{
		OperatingMode = MOVE_MODE;
	}
#if _DEBUG
	std::cout << "Set Current mode to " << this->OperatingMode << std::endl;
#endif
}

void PickingWindow::CreateMarker(double* pos)
{
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(MarkerMapper);
	actor->SetPosition(pos[0], pos[1], pos[2]);

	SetCurrentMarker(actor);
	//actor->GetProperty()->SetColor(color[0], color[1], color[2]);
	//actor->PickableOff();

	//Add actor to renderer and list
	MarkerActorList->AddItem(actor);
	Renderer->AddActor(actor);
	RenderWindow->Render();
#ifdef _DEBUG
	std::cout << "Current marker #:" << MarkerActorList->GetNumberOfItems() << std::endl;
#endif

}

void PickingWindow::SetCurrentMarker(vtkSmartPointer<vtkActor> actor)
{
	if (actor == nullptr) 
		return;

	if (CurrentMarkerActor.Get() != nullptr)
	{
		CurrentMarkerActor->GetProperty()->SetColor(MARKER_COLOR[0], MARKER_COLOR[1], MARKER_COLOR[2]);
	}
	actor->GetProperty()->SetColor(MARKER_COLOR_SELECTED[0], MARKER_COLOR_SELECTED[1], MARKER_COLOR_SELECTED[2]);
	CurrentMarkerActor = actor;
#ifdef _DEBUG
	std::cout << "Current actor: " << CurrentMarkerActor.GetPointer() << std::endl;
#endif
}

void PickingWindow::RemoveCurrentMarker()
{
	auto& actor = CurrentMarkerActor;
	if (actor != nullptr)
	{
		//int idx = MarkerActorList->IsItemPresent(actor);
		//if (idx != 0)
		//{
		//	idx -= 1;
		//	std::cout << idx << std::endl;

		//	MarkerActorList->RemoveItem(idx);
		//	Renderer->RemoveActor(actor);

		//	actor = nullptr;	// auto free actor

		//	this->Render();
		//}
		

		//auto iter = MarkerActorList->NewIterator();
		//while (iter->GetCurrentObject() != nullptr)
		//{
		//	if(iter->GetCurrentObject() == actor)
		//	{
		//		MarkerActorList->RemoveItem(iter);
		//		if (iter->GetCurrentObject() != nullptr)
		//		{
		//			SetCurrentMarker(static_cast<vtkActor*>(iter->GetCurrentObject()));
		//		}
		//		break;
		//		std::cout << "delete" << std::endl;
		//	}
		//	std::cout << iter->GetCurrentObject() << std::endl;
		//	iter->GoToNextItem();
		//}

		MarkerActorList->InitTraversal();
		vtkSmartPointer<vtkActor> curActor;
		for (int idx = 0; idx < MarkerActorList->GetNumberOfItems(); idx++)
		{
			if ((curActor = MarkerActorList->GetNextActor()) != nullptr)
			{
				if (curActor.Get() == actor.Get())
				{
					MarkerActorList->RemoveItem(idx);
					Renderer->RemoveActor(actor);

#ifdef _DEBUG
					std::cout << "Delete marker:" << idx << std::endl;
#endif

					auto nextActor = MarkerActorList->GetNextActor();
					if (nextActor == nullptr)
						nextActor = MarkerActorList->GetLastActor();

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
