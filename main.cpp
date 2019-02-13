#include "include.h"

static struct sTest : public ZL_Application
{
	sTest() : ZL_Application(60) { }

	virtual void Load(int argc, char *argv[])
	{
		//if (!ZL_Application::LoadReleaseDesktopDataBundle("Test.dat")) return;
		if (!ZL_Display::Init("Slotti", 1024, 768, ZL_DISPLAY_DEPTHBUFFER | ZL_DISPLAY_ALLOWRESIZEHORIZONTAL)) return;

		ZL_Display::SetAA(true);
		ZL_Audio::Init();

		ZL_Display3D::Init();
		ZL_Input::Init();

		ZL_Application::SettingsInit("Slotti");
		ZL_SceneManager::Init(SCENE_GAME);
	}
} Test;
