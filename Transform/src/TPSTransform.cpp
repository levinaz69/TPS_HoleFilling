#include "common/common.h"
#include "common/utilities.h"


// Settings
#define SHOW_MESH true
#define SHOW_COLOR false
#define SHOW_SOURCE false
#define FILENAME_POSTFIX "_tpsTransformed"

#include "TransformCommon.hpp"


vtkSmartPointer<vtkAbstractTransform> CalcTransform(vtkPoints* sourcePoints, vtkPoints* targetPoints)
{
	auto transform = vtkSmartPointer< vtkThinPlateSplineTransform >::New();
	transform->SetSourceLandmarks(sourcePoints);
	transform->SetTargetLandmarks(targetPoints);
	transform->SetBasisToR2LogR();
	//transform->SetBasisToR();
	//transform->Inverse();

	return transform;
}

//// Main
int main(int argc, char* argv[])
{
	return Main(argc, argv);

}
