#include <DemoSystem.h>
#include "SimpleDemo.h"

int main(int argc, char* argv[])
{
	// Create and initialize base demo system
	DemoSystem *demoSystem = new DemoSystem();
	demoSystem->Initialize();

	// Create and initialize the main demo object
	demoSystem->SetDemo(new SimpleDemo());

	// enter main loop
	while (demoSystem->IsRunning())
	{
		demoSystem->Clear();

		demoSystem->Update();

		demoSystem->Present();
	}

	// cleanup
	demoSystem->Destroy();

	delete demoSystem;

	return 0;
}