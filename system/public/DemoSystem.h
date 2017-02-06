#ifndef _DEMO_SYSTEM_H
#define _DEMO_SYSTEM_H

#pragma once

#include <SDL.h>
#include <SDL_syswm.h>
#include <imgui\imgui.h>

#include "DemoCommon.h"
#include "Input.h"
#include "IGraphicsDevice.h"

#include <map>
#include <vector>

#if defined(_DEBUG)

#define CHECK_SDL(_call)														\
{																				\
	int32 res = _call;															\
	if(res < 0)																	\
	{																			\
		HRESULT hr = _call;														\
		Log::WriteError("SDL Error on line %i of file %s", __LINE__, __FILE__);	\
		Log::WriteError(SDL_GetError());										\
	}																			\
}

#else
#define CHECK_SDL(_call) (_call)
#endif

class Demo;
class Texture;

enum class GraphicsAPIOptions
{
	None,
	DirectX11,
	OpenGL3
};

struct PerFrameUniforms
{
	glm::mat4 viewProjection;
	glm::mat4 uiOrthoProjection;
};

enum class FullscreenMode
{
	Windowed			= 0,
	BorderlessWindowed	= 1,
	Fullscreen			= 2
};
ENUM_FLAGS(FullscreenMode)

struct DisplaySettings
{
	DisplaySettings()
	{
		width = 1366;
		height = 768;
		refreshRate = 60;
		vsync = true;
		fullscreenMode = FullscreenMode::Windowed;
		windowTitle = "Window";
	}

	int32 width;
	int32 height;
	int32 refreshRate;
	bool vsync;
	FullscreenMode fullscreenMode;
	std::string windowTitle;
};

struct DisplayMode
{
	DisplayMode(uint32 width, uint32 height, uint32 refreshRate) :
		width(width),
		height(height),
		refreshRate(refreshRate)
	{
		displayModeStr = std::to_string(width)  + "x"	+
						 std::to_string(height) + " @ " +
						 std::to_string(refreshRate);
	}

	//SDL_DisplayMode displayMode;
	uint32 width;
	uint32 height;
	uint32 refreshRate;
	std::string displayModeStr;
};

//typedef std::vector<SDL_DisplayMode> DisplayModeList;

class DemoSystem
{
public:

	DemoSystem();
	~DemoSystem();

	void Initialize();
	void Update();
	void Destroy();

	void SetDemo(Demo* newDemo);

	void SetGraphicsAPI(GraphicsAPIOptions api);
	IGraphicsDevice* GetGraphicsDevice();

	void Clear();
	void Present();

	void MakeWindow(const DisplaySettings &newSettings);

	// Setters
	void SetDisplaySettings(const DisplaySettings &newSettings);

	void SetFOV(float newFOV);

	void SetClipboardText(const char* text);

	// Gettters
	void GetDisplaySize(uint32* width, uint32* height);

	const DisplaySettings& GetDisplaySettings();

	const std::vector<DisplayMode>& GetAvailableDisplayModes();

	const char* GetClipboardText();

	Texture* LoadTexture(std::string fileName);

	bool IsRunning();
	void SetRunning(bool IsRunning);

	Input input;

private:

	void CreateResources();
	void ReleaseResources();

	void OnRenderAPIChanged();

	// UI
	void DrawBaseUI();

	bool running;

	Demo*			   curDemo;
	IGraphicsDevice*   curGraphicsDevice;
	GraphicsAPIOptions curAPIOption;

	// Display
	DisplaySettings	curDisplaySettings;
	std::vector<DisplayMode> displayModeList;
	float fov = 70.0f;

	SDL_Window*		sdlWindow;
	SDL_DisplayMode displayMode;
	SDL_SysWMinfo	sdlInfo;

	// Uniform Buffers
	PerFrameUniforms perFrameUniform;
	Buffer* perFrameBuffer;
};

#endif // _DEMO_SYSTEM_H