#include "common/common.h"
#include "common/utilities.h"

#include <vtkVersion.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkLandmarkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkVertexGlyphFilter.h>

#include <vtkSimplePointsReader.h>
#include <vtkSimplePointsWriter.h>
#include <vtkPLYReader.h>
#include <vtkPLYWriter.h>

#include <string>
#include <vtkArrayWriter.h>
#include <vtkDenseArray.h>
#include <vtkArrayData.h>
#include <vtkInteractorStyleTrackballCamera.h>

#include <vtkThinPlateSplineTransform.h>


/*

#define SHOW_MESH false
#define SHOW_COLOR false
#define SHOW_SOURCE true
#define FILENAME_POSTFIX "_Transformed"

*/


vtkSmartPointer<vtkAbstractTransform> CalcTransform(vtkPoints* sourcePoints, vtkPoints* targetPoints);

//vtkSmartPointer<vtkPolyData> CreatePolyData(vtkPoints* points)
//{
//	vtkSmartPointer<vtkPolyData> temp =
//		vtkSmartPointer<vtkPolyData>::New();
//	temp->SetPoints(points);
//
//	vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter =
//		vtkSmartPointer<vtkVertexGlyphFilter>::New();
//
//	vertexFilter->SetInputData(temp);
//	vertexFilter->Update();
//	return vertexFilter->GetOutput();
//}
//vtkSmartPointer<vtkAbstractTransform> CalcTransform(vtkPoints* sourcePoints, vtkPoints* targetPoints)
//{
//	// Setup ICP transform
//	vtkSmartPointer<vtkIterativeClosestPointTransform> icp =
//		vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
//	icp->SetSource(CreatePolyData(sourcePoints));
//	icp->SetTarget(CreatePolyData(targetPoints));
//	icp->GetLandmarkTransform()->SetModeToRigidBody();
//	icp->SetMaximumNumberOfIterations(20);
//	//icp->StartByMatchingCentroidsOn();
//	icp->Modified();
//	icp->Update();
//
//	// Get the resulting transformation matrix (this matrix takes the source points to the target points)
//	vtkSmartPointer<vtkMatrix4x4> m = icp->GetMatrix();
//	std::cout << "The resulting matrix is: " << *m << std::endl;
//
//	return icp;
//}


vtkSmartPointer<vtkPolyData> ApplyTransform(vtkSmartPointer<vtkAbstractTransform> transform, vtkSmartPointer<vtkPolyData> polyData)
{
	auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(polyData);
	transformFilter->SetTransform(transform);
	transformFilter->Update();
	return transformFilter->GetOutput();
}

//// Visualization 
vtkSmartPointer<vtkActor> GetPointsActor(vtkPolyData* pointsPolyData)
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


vtkSmartPointer<vtkRenderWindowInteractor> VisualizeMarker(vtkPolyData* source, vtkPolyData* target, vtkPolyData* solution)
{
	auto sourceActor = GetPointsActor(source);
	auto targetActor = GetPointsActor(target);
	auto solutionActor = GetPointsActor(solution);

	sourceActor->GetProperty()->SetColor(0, 1, 0);
	sourceActor->GetProperty()->SetPointSize(4);

	targetActor->GetProperty()->SetColor(1, 0, 0);
	targetActor->GetProperty()->SetPointSize(4);

	solutionActor->GetProperty()->SetColor(0, 0, 1);
	solutionActor->GetProperty()->SetPointSize(3);

	// Create a renderer, render window, and interactor
	auto renderer = vtkSmartPointer<vtkRenderer>::New();
	auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);
	auto interactorStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	interactorStyle->SetDefaultRenderer(renderer);
	renderWindowInteractor->SetInteractorStyle(interactorStyle);

	// Add the actor to the scene
	//renderer->AddActor(sourceActor);
	renderer->AddActor(targetActor);
	renderer->AddActor(solutionActor);
	renderer->SetBackground(0, 0, 0);

	renderWindow->Render();
	return renderWindowInteractor;
}

vtkSmartPointer<vtkRenderWindowInteractor> VisualizeMesh(vtkPolyData* sourceMesh, vtkPolyData* targetMesh, vtkPolyData* solutionMesh)
{

	// Visualize
	vtkSmartPointer<vtkPolyDataMapper> sourceMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	sourceMapper->SetInputData(sourceMesh);

	vtkSmartPointer<vtkActor> sourceActor =
		vtkSmartPointer<vtkActor>::New();
	sourceActor->SetMapper(sourceMapper);
	sourceActor->GetProperty()->SetColor(0, 1, 0);

	vtkSmartPointer<vtkPolyDataMapper> targetMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	targetMapper->SetInputData(targetMesh);

	vtkSmartPointer<vtkActor> targetActor =
		vtkSmartPointer<vtkActor>::New();
	targetActor->SetMapper(targetMapper);
	targetActor->GetProperty()->SetColor(1, 0, 0);
	targetActor->GetProperty()->SetOpacity(0.5);

	vtkSmartPointer<vtkPolyDataMapper> solutionMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	solutionMapper->SetInputData(solutionMesh);

	vtkSmartPointer<vtkActor> solutionActor =
		vtkSmartPointer<vtkActor>::New();
	solutionActor->SetMapper(solutionMapper);
	solutionActor->GetProperty()->SetColor(0, 0, 1);
	solutionActor->GetProperty()->SetOpacity(0.5);

	// Create a renderer, render window, and interactor
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	auto interactorStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	interactorStyle->SetDefaultRenderer(renderer);
	renderWindowInteractor->SetInteractorStyle(interactorStyle);

	// Add the actor to the scene
	//renderer->AddActor(sourceActor);
	renderer->AddActor(targetActor);
	renderer->AddActor(solutionActor);
	renderer->SetBackground(0, 0, 0);

	renderWindow->Render();
	return renderWindowInteractor;
}

vtkSmartPointer<vtkRenderWindowInteractor> Visualize(
	vtkPolyData* source, vtkPolyData* target, vtkPolyData* solution,
	vtkPolyData* sourceMesh, vtkPolyData* targetMesh, vtkPolyData* solutionMesh)
{
	// Create a renderer, render window, and interactor
	auto renderer = vtkSmartPointer<vtkRenderer>::New();
	auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);
	auto interactorStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	interactorStyle->SetDefaultRenderer(renderer);
	renderWindowInteractor->SetInteractorStyle(interactorStyle);

	// Markers
	{
		auto sourceActor = GetPointsActor(source);
		auto targetActor = GetPointsActor(target);
		auto solutionActor = GetPointsActor(solution);

		sourceActor->GetProperty()->SetColor(0, 1, 0);
		sourceActor->GetProperty()->SetPointSize(5);

		targetActor->GetProperty()->SetColor(1, 0, 0);
		targetActor->GetProperty()->SetPointSize(5);

		solutionActor->GetProperty()->SetColor(0, 0, 1);
		solutionActor->GetProperty()->SetPointSize(5);

		// Add the actor to the scene
#if SHOW_SOURCE
		renderer->AddActor(sourceActor);
#endif //SHOW_SOURCE
		renderer->AddActor(targetActor);
		renderer->AddActor(solutionActor);
	}

	// Mesh
	{
#if SHOW_MESH
		//// mesh form
		vtkSmartPointer<vtkPolyDataMapper> sourceMapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		sourceMapper->SetInputData(sourceMesh);

		vtkSmartPointer<vtkActor> sourceActor =
			vtkSmartPointer<vtkActor>::New();
		sourceActor->SetMapper(sourceMapper);
		sourceActor->GetProperty()->SetColor(0, 1, 0);
		sourceActor->GetProperty()->SetOpacity(0.6);

		vtkSmartPointer<vtkPolyDataMapper> targetMapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		targetMapper->SetInputData(targetMesh);

		vtkSmartPointer<vtkActor> targetActor =
			vtkSmartPointer<vtkActor>::New();
		targetActor->SetMapper(targetMapper);
		targetActor->GetProperty()->SetColor(1, 0, 0);
		targetActor->GetProperty()->SetOpacity(0.6);

		vtkSmartPointer<vtkPolyDataMapper> solutionMapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		solutionMapper->SetInputData(solutionMesh);

		vtkSmartPointer<vtkActor> solutionActor =
			vtkSmartPointer<vtkActor>::New();
		solutionActor->SetMapper(solutionMapper);
		solutionActor->GetProperty()->SetColor(0, 0, 1);
		solutionActor->GetProperty()->SetOpacity(0.6);
#else //SHOW_MESH
		//// vertices form
		auto sourceActor = GetPointsActor(sourceMesh);
		auto targetActor = GetPointsActor(targetMesh);
		auto solutionActor = GetPointsActor(solutionMesh);

		sourceActor->GetProperty()->SetColor(0, 0.5, 0);
		sourceActor->GetProperty()->SetPointSize(2);
		targetActor->GetProperty()->SetColor(0.5, 0, 0);
		targetActor->GetProperty()->SetPointSize(2);
		solutionActor->GetProperty()->SetColor(0, 0, 0.5);
		solutionActor->GetProperty()->SetPointSize(2);
#endif //SHOW_MESH

		// Add the actor to the scene
#if SHOW_SOURCE
		renderer->AddActor(sourceActor);
#endif //SHOW_SOURCE
		renderer->AddActor(targetActor);
		renderer->AddActor(solutionActor);
	}

	//renderer->SetBackground(0, 0, 0);	// black
	renderer->SetBackground(0.5, 0.5, 0.5);
	auto ssize = renderWindow->GetScreenSize();
	renderWindow->SetSize(ssize[1] * 0.8, ssize[1] * 0.8);
	renderWindow->Render();
	return renderWindowInteractor;

}

//// Main
int Main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cout << "Usage: " << argv[0] << " <source_marker_file> <target_marker_file> [source_mesh_file] [target_mesh_file]" << std::endl;
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

#if !SHOW_COLOR
	// clean up color information
	if (auto colorArray = sourceMesh->GetPointData()->GetScalars()) colorArray->Reset();
	if (auto colorArray = targetMesh->GetPointData()->GetScalars()) colorArray->Reset();
#endif //SHOW_COLOR

	// Calculate Transform
	auto transform = CalcTransform(sourcePoints, targetPoints);

	// Apply Transfrom
	auto solution = ApplyTransform(transform, source);
	auto solutionMesh = useSourceMesh ? ApplyTransform(transform, sourceMesh) : nullptr;

	// Save Solution
	//// marker
	vtkStdString solutionPointsFilename = Utility::GetBaseFilename(sourceMarkerFilename.c_str());
	solutionPointsFilename += FILENAME_POSTFIX;
	solutionPointsFilename += ".xyz";

	auto solutionPointsWriter = vtkSmartPointer<vtkSimplePointsWriter>::New();
	solutionPointsWriter->SetFileName(solutionPointsFilename.c_str());
	solutionPointsWriter->SetInputData(solution);
	solutionPointsWriter->Update();
	solutionPointsWriter->Write();

	std::cout << "Write transformed marker to: " << solutionPointsFilename << std::endl;

	//// mesh
	if (useSourceMesh)
	{
		vtkStdString solutionMeshFilename = Utility::GetBaseFilename(sourceMeshFilename.c_str());
		solutionMeshFilename += FILENAME_POSTFIX;
		solutionMeshFilename += ".ply";

		auto solutionMeshWriter = vtkSmartPointer<vtkPLYWriter>::New();
		solutionMeshWriter->SetFileName(solutionMeshFilename.c_str());
		solutionMeshWriter->SetInputData(solutionMesh);
		solutionMeshWriter->Update();
		solutionMeshWriter->Write();

		std::cout << "Write transformed model to: " << solutionMeshFilename << std::endl;
	}

	// Visualization
	std::vector< vtkSmartPointer<vtkRenderWindowInteractor> > interactors;
	//interactors.push_back( VisualizeMarker(source, target, solution) );
	if (useTargetMesh)
	{
		//interactors.push_back(VisualizeMesh(sourceMesh, targetMesh, solutionMesh));
		interactors.clear();
		interactors.push_back(Visualize(source, target, solution, sourceMesh, targetMesh, solutionMesh));
	}
	interactors.back()->Start();
	return EXIT_SUCCESS;
}