#pragma once
#include "common/common.h"
#include "RigidTransform.h"

// Handle mouse events
class TransfromInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	static TransfromInteractorStyle* New();
	vtkTypeMacro(TransfromInteractorStyle, vtkInteractorStyleTrackballCamera);

	virtual void OnKeyPress() override
	{
		// Get the keypress
		std::string key = this->GetInteractor()->GetKeySym();

		// Handle a key
		// Enter different OPERATING_MODE according to the key
		if (key == "z")
		{

		}
		// Forward events
		vtkInteractorStyleTrackballCamera::OnKeyPress();
	}

	//virtual void OnMiddleButtonDown() override;
	//virtual void OnMouseMove() override;

};
