#include "RigidTransform.h"
#include "TransformInteractorStyle.h"

void RigidTransform::CalcTransform()
{
	// Setup the transform
    Transform = vtkSmartPointer<vtkLandmarkTransform>::New();
	Transform->SetSourceLandmarks(SourceMarkerPoints);
	Transform->SetTargetLandmarks(TargetMarkerPoints);

	//transform->SetModeToRigidBody();
	Transform->SetModeToSimilarity();
	//transform->SetModeToAffine();

	Transform->Update();
}

void RigidTransform::ApplyTransform()
{
	//SolutionMarkerPoints = ApplyTransform(SourceMarkerPoints);
	SolutionModel = ApplyTransform(SourceModel);
}
vtkSmartPointer<vtkPolyData> RigidTransform::ApplyTransform(vtkSmartPointer<vtkPolyData> polyData)
{
	auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(polyData);
	transformFilter->SetTransform(Transform);
	transformFilter->Update();
	return transformFilter->GetOutput();
}


void RigidTransform::ShowTransformMatrix()
{
	// Display the transformation matrix that was computed
	vtkMatrix4x4* mat = Transform->GetMatrix();
	std::cout << "Matrix: " << *mat << std::endl;
}
void RigidTransform::SaveTransformMatrix()
{
	vtkMatrix4x4* mat = Transform->GetMatrix();
	// Save matrix to file
	auto matArray = vtkSmartPointer< vtkDenseArray<double> >::New();
	matArray->Resize(4, 4);
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			matArray->SetValue(i, j, mat->GetElement(i, j));

	//vtkStdString matFilename = "";
	//matFilename += Utility::GetBaseFilename(sourceMarkerFilename.c_str());
	//matFilename += "-";
	//matFilename += Utility::GetBaseFilename(targetMarkerFilename.c_str());
	//matFilename += "_mat.txt";
	vtkStdString matFilename = "transform_mat.txt";
	auto matArrayWriter = vtkSmartPointer<vtkArrayWriter>::New();
	matArrayWriter->Write(matArray, matFilename);
}

vtkSmartPointer<vtkActor> RigidTransform::GetPointsActor(vtkPolyData* pointsPolyData)
{
	auto glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
	glyphFilter->SetInputData(pointsPolyData);
	glyphFilter->Update();
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(glyphFilter->GetOutputPort());
	auto actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	return actor;
}

vtkStandardNewMacro(TransfromInteractorStyle);
void RigidTransform::VisualizeInit()
{
	// Create a renderer, render window, and interactor
	Renderer = vtkSmartPointer<vtkRenderer>::New();
	RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	RenderWindow->AddRenderer(Renderer);
	RenderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	RenderWindowInteractor->SetRenderWindow(RenderWindow);
	InteractorStyle = vtkSmartPointer<TransfromInteractorStyle>::New();
	InteractorStyle->SetDefaultRenderer(Renderer);
	RenderWindowInteractor->SetInteractorStyle(InteractorStyle);

}
void RigidTransform::Visualize()
{
	auto sourceActor = GetPointsActor(SourceModel);
	auto targetActor = GetPointsActor(TargetModel);
	auto solutionActor = GetPointsActor(SolutionModel);

	sourceActor->GetProperty()->SetColor(0, 1, 0);
	sourceActor->GetProperty()->SetPointSize(4);

	targetActor->GetProperty()->SetColor(1, 0, 0);
	targetActor->GetProperty()->SetPointSize(4);

	solutionActor->GetProperty()->SetColor(0, 0, 1);
	solutionActor->GetProperty()->SetPointSize(3);

	// Add the actor to the scene
	//Renderer->AddActor(sourceActor);
	Renderer->AddActor(targetActor);
	Renderer->AddActor(solutionActor);
	Renderer->SetBackground(0, 0, 0);

	RenderWindow->Render();
}
