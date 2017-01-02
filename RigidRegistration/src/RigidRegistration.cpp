#include "common/common.h"
#include "RigidTransform.h"
#include <memory>

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cout << "Usage: " << argv[0] << " <source_marker_file> <target_marker_file> [source_model_file] [target_model_file]" << std::endl;
		std::cout << "Input file format should be .xyz" << std::endl;
		return EXIT_FAILURE;
	}

	bool useSourceMesh = argc >= 4;
	bool useTargetMesh = argc >= 5;

	std::string sourceMarkerFilename = argv[1];
	std::string targetMarkerFilename = argv[2];
	std::string sourceMeshFilename = useSourceMesh ? argv[3] : "";
	std::string targetMeshFilename = useTargetMesh ? argv[4] : "";

	// Read Marker
	auto sourcePointsReader = vtkSmartPointer<vtkSimplePointsReader>::New();
	sourcePointsReader->SetFileName(sourceMarkerFilename.c_str());
	sourcePointsReader->Update();

	auto targetPointsReader = vtkSmartPointer<vtkSimplePointsReader>::New();
	targetPointsReader->SetFileName(targetMarkerFilename.c_str());
	targetPointsReader->Update();

	vtkPolyData* source = sourcePointsReader->GetOutput();
	vtkPolyData* target = targetPointsReader->GetOutput();

	auto sourcePtsNum = source->GetNumberOfPoints();
	auto targetPtsNum = target->GetNumberOfPoints();

	std::cout << "Source Marker Point Number: " << sourcePtsNum << std::endl;
	std::cout << "Target Marker Point Number: " << targetPtsNum << std::endl;
	if (sourcePtsNum != targetPtsNum)
	{
		std::cout << "Point number mismatch. Exit." << std::endl;
		return EXIT_FAILURE;
	}

	vtkPoints* sourcePoints = source->GetPoints();
	vtkPoints* targetPoints = target->GetPoints();

	// Read Mesh
	vtkSmartPointer<vtkPolyData> sourceMesh = nullptr;
	vtkSmartPointer<vtkPolyData> targetMesh = nullptr;
	if (useSourceMesh)
	{
		auto sourceMeshReader = vtkSmartPointer<vtkPLYReader>::New();
		sourceMeshReader->SetFileName(sourceMeshFilename.c_str());
		sourceMeshReader->Update();
		sourceMesh = sourceMeshReader->GetOutput();
	}

	if (useTargetMesh)
	{
		auto targetMeshReader = vtkSmartPointer<vtkPLYReader>::New();
		targetMeshReader->SetFileName(targetMeshFilename.c_str());
		targetMeshReader->Update();
		targetMesh = targetMeshReader->GetOutput();
	}


	// Calculate Transform
	auto transform = std::make_shared<RigidTransform>();

	transform->SourceMarkerPoints = sourcePoints;
	transform->TargetMarkerPoints = targetPoints;
	transform->SourceModel = sourceMesh;
	transform->TargetModel = targetMesh;

	transform->CalcTransform();
	transform->ApplyTransform();

	transform->VisualizeInit();
	transform->Visualize();

	transform->GetRenderWindowInteractor()->Start();

	return EXIT_SUCCESS;
}