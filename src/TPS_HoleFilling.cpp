#include "common.h"
#include "PickingWindow.h"

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage: " << argv[0] << "<point_cloud_file> <template_mesh_file>" << std::endl;
		std::cout << "Input file format should be .ply" << std::endl;
		return EXIT_FAILURE;
	}
	std::string inputFilename1 = argv[1];
	std::string inputFilename2 = argv[2];

	auto meshWindow1 = std::make_shared<PickingWindow>();
	auto meshWindow2 = std::make_shared<PickingWindow>();

	meshWindow1->Initialize();
	meshWindow1->SetModelFile(inputFilename1);
	meshWindow1->LoadModelFile();
	meshWindow1->LoadMarkerFile();
	meshWindow1->Render();

	meshWindow2->Initialize();
	meshWindow2->SetModelFile(inputFilename2);
	meshWindow2->LoadModelFile();
	meshWindow2->LoadMarkerFile();

	meshWindow2->Render();

	std::vector< vtkSmartPointer<vtkRenderWindowInteractor> > interactors;
	interactors.push_back(meshWindow1->RenderWindowInteractor);
	interactors.push_back(meshWindow2->RenderWindowInteractor);
	interactors.back()->Start();

	return EXIT_SUCCESS;
}
