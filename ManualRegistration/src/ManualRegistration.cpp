#include "common/common.h"
#include "PickingWindow.h"

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage: " << argv[0] << " <target_mesh_file> <template_mesh_file>" << std::endl;
		std::cout << "Input file format should be .ply" << std::endl;
		return EXIT_FAILURE;
	}
	std::string inputFilename1 = argv[1];
	std::string inputFilename2 = argv[2];

	auto meshWindow1 = std::make_shared<PickingWindow>();
	auto meshWindow2 = std::make_shared<PickingWindow>();

	meshWindow1->Initialize();
    meshWindow1->SetPickerMode(1);
	meshWindow1->SetModelFile(inputFilename1);
	meshWindow1->LoadModelFile();
	meshWindow1->LoadMarkerFile();
    meshWindow1->InitPicker();
	meshWindow1->Render();

	meshWindow2->Initialize();
    meshWindow2->SetPickerMode(1);
	meshWindow2->SetModelFile(inputFilename2);
	meshWindow2->LoadModelFile();
	meshWindow2->LoadMarkerFile();
    meshWindow2->InitPicker();
	meshWindow2->Render();

	std::vector<vtkRenderWindowInteractor*> interactors;
	interactors.push_back(meshWindow1->GetRenderWindowInteractor());
	interactors.push_back(meshWindow2->GetRenderWindowInteractor());
	interactors.back()->Start();

	return EXIT_SUCCESS;
}
