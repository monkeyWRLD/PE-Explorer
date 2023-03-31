#include "GUI/gui.h"
#include "SDK/GrabPEInfo.h"
#include <thread>


void ExitPrgm() {

	// destroy gui
	GUI::DestroyImGui();
	GUI::DestroyDevice();
	GUI::DestroyHWindow();


	std::exit(0);
	
}

int __stdcall wWinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	PWSTR arguments,
	int commandShow)
{

	// create gui
	GUI::CreateHWindow("PE GUI");
	GUI::CreateDevice();
	
	GUI::CreateImGui();

	while (GUI::isRunning)
	{
		GUI::BeginRender();
		GUI::Render();
		GUI::EndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(3));
	}

	
	ExitPrgm();


	return EXIT_SUCCESS;
}


