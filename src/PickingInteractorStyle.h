#pragma once
#include "common.h"
#include "PickingWindow.h"

// Handle mouse events
class PickingInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	static PickingInteractorStyle* New();
	vtkTypeMacro(PickingInteractorStyle, vtkInteractorStyleTrackballCamera);

	vtkSmartPointer<vtkPropPicker> Picker = vtkSmartPointer<vtkPropPicker>::New();
	std::function< void(const int*) > PickingCallback;
	std::function< void(const OPERATING_MODE) > ModeSelectingCallback;
	std::function< void(void) > RemoveCurrentMarkerCallback;

	void SetPickingCallback(std::function< void(const int*) > callback)
	{
		PickingCallback = callback;
	}
	void SetModeSelectingCallback(std::function< void(const OPERATING_MODE) > callback)
	{
		ModeSelectingCallback = callback;
	}
	void SetRemoveCurrentMarkerCallback(std::function< void(void) > callback)
	{
		RemoveCurrentMarkerCallback = callback;
	}
	
	virtual void OnLeftButtonDown() override
	{
		// Handle picking callback only when not press control keys
		if (  !(this->GetInteractor()->GetControlKey() ||
				this->GetInteractor()->GetShiftKey() ||
				this->GetInteractor()->GetAltKey())  )
		{
			int* clickPos = this->GetInteractor()->GetEventPosition();
			PickingCallback(clickPos);
		}
		//else
		{
			// Forward events
			vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
		}
	}

	virtual void OnKeyPress() override
	{
		// Get the keypress
		std::string key = this->GetInteractor()->GetKeySym();

		// Handle a key
		// Enter different OPERATING_MODE according to the key
		if (key == "z")
		{
				ModeSelectingCallback(SELECT_MODE);
		} 
		else if (key == "a")
		{
			ModeSelectingCallback(CREATE_MODE);
		} 
		else if (key == "x")
		{
			ModeSelectingCallback(MOVE_MODE);
		}
		else if (key == "d")	// Apply removing action immediately
		{
			RemoveCurrentMarkerCallback();
		}
		

		// Forward events
		//vtkInteractorStyleTrackballCamera::OnKeyPress();
	}

	//virtual void OnMiddleButtonDown() override;
	//virtual void OnMouseMove() override;
	
};
