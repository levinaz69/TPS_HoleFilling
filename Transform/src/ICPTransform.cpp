#include "common/common.h"
#include "common/utilities.h"

// Settings
#define SHOW_MESH false
#define SHOW_COLOR false
#define SHOW_SOURCE false
#define FILENAME_POSTFIX "_transformed"

#include "TransformCommon.hpp"


vtkSmartPointer<vtkAbstractTransform> CalcTransform(vtkPoints* sourcePoints, vtkPoints* targetPoints)
{
	// Setup the transform
	auto transform = vtkSmartPointer<vtkLandmarkTransform>::New();
	transform->SetSourceLandmarks(sourcePoints);
	transform->SetTargetLandmarks(targetPoints);
	//transform->SetModeToRigidBody();
	transform->SetModeToSimilarity();
	//transform->SetModeToAffine();
	transform->Update();

	// Display the transformation matrix that was computed
	vtkMatrix4x4* mat = transform->GetMatrix();
	std::cout << "Matrix: " << *mat << std::endl;

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
	std::cout << "Write transform matrix to: " << matFilename << std::endl;

	return transform;
}

//// Main
int main(int argc, char* argv[])
{
	return Main(argc, argv);

}