#pragma once
#include "common/common.h"
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
	std::function< void(int) > AdjustMarkerSizeCallback;
	std::function< void(void) > WriteMarkerFileCallback;
	std::function< void(void) > SelectToggleOperatingSimultaneouslyCallback;
	std::function< void(void) > SelectPrevMarkerCallback;
	std::function< void(void) > SelectNextMarkerCallback;

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
	void SetAdjustMarkerSizeCallback(std::function< void(int) > callback)
	{
		AdjustMarkerSizeCallback = callback;
	}
	void SetWriteMarkerFileCallback(std::function< void(void) > callback)
	{
		WriteMarkerFileCallback = callback;
	}
	void SetSelectToggleOperatingSimultaneouslyCallback(std::function< void(void) > callback)
	{
		SelectToggleOperatingSimultaneouslyCallback = callback;
	}
	void SetSelectPrevMarkerCallback(std::function< void(void) > callback)
	{
		SelectPrevMarkerCallback = callback;
	}
	void SetSelectNextMarkerCallback(std::function< void(void) > callback)
	{
		SelectNextMarkerCallback = callback;
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
		else // Uncomment this line to enable camara adjustment without control key pressed
		{
			// Forward events
			vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
		}
	}

	// Use right mouse button to rotate camera
	// rotate if not press control button
	// otherwise scale
	virtual void OnRightButtonDown() override
	{
		if (!(this->GetInteractor()->GetControlKey() ||
			this->GetInteractor()->GetShiftKey() ||
			this->GetInteractor()->GetAltKey()))
		{
			vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
		}
		else
		{
			vtkInteractorStyleTrackballCamera::OnRightButtonDown();
		}
	}
	virtual void OnRightButtonUp() override
	{

		if (!(this->GetInteractor()->GetControlKey() ||
			this->GetInteractor()->GetShiftKey() ||
			this->GetInteractor()->GetAltKey()))
		{
			vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
		}
		else 
		{
			vtkInteractorStyleTrackballCamera::OnRightButtonUp();
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
			ModeSelectingCallback(APPEND_MODE);
		} 
		else if (key == "i")
		{
			ModeSelectingCallback(INSERT_MODE);
		}
		else if (key == "x")
		{
			ModeSelectingCallback(MOVE_MODE);
		}
		else if (key == "d")	// Apply removing action immediately
		{
			RemoveCurrentMarkerCallback();
		}
		else if (key == "n")	// decrease marker size
		{
			AdjustMarkerSizeCallback(-1);
		}
		else if (key == "m")	// increase marker size
		{
			AdjustMarkerSizeCallback(1);
		}
		else if (key == "g")	// write marker file now
		{
			WriteMarkerFileCallback();
		}
		else if (key == "v")
		{
			SelectToggleOperatingSimultaneouslyCallback();
		}
		else if (key == "j")
		{
			SelectPrevMarkerCallback();
		}
		else if (key == "k")
		{
			SelectNextMarkerCallback();
		}

		// Forward events
		vtkInteractorStyleTrackballCamera::OnKeyPress();
	}

	//virtual void OnMiddleButtonDown() override;
	//virtual void OnMouseMove() override;
	
};
