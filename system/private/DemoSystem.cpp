#include "DemoSystem.h"
#include "IGraphicsDevice.h"

#include "Demo.h"
#include "DX11Device.h"
#include "GL3Device.h"

#include "UIManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#include <SOIL.h>
#include <glm\gtc\matrix_transform.hpp>

DemoSystem::DemoSystem()
{
	running = true;
	sdlWindow = nullptr;
	curGraphicsDevice = nullptr;
	curDemo = nullptr;
	curAPIOption = GraphicsAPIOptions::None;
}

DemoSystem::~DemoSystem()
{

}

void DemoSystem::Initialize()
{
	// initialize all SDL subsystems
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return;

	SDL_GetDesktopDisplayMode(0, &displayMode);

	int32 numDisplays = SDL_GetNumVideoDisplays();
	int32 numDisplayModes = SDL_GetNumDisplayModes(0);

	for (int32 d = 0; d < numDisplayModes; d++)
	{
		SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };
		SDL_GetDisplayMode(0, d, &mode);

		displayModeList.push_back(DisplayMode(mode.w, mode.h, mode.refresh_rate));
	}

	Time::Start();
	UIManager::Initialize(this);
}

void DemoSystem::Update()
{
	Time::UpdateTime();

	// reset input states
	input.Reset();

	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent))
	{
		switch (sdlEvent.type)
		{
			case SDL_QUIT:
				SetRunning(false);
				break;
			default:

				// pass any unused events to the input system
				input.ProcessInput(sdlEvent);

				break;
		}
	}

	// update per frame uniforms
	float dist = 2.0f;
	float x = sin(Time::time()) * dist;
	float y = cos(Time::time()) * dist;

	float resX = static_cast<float>(curDisplaySettings.width);
	float resY = static_cast<float>(curDisplaySettings.height);
	float aspectRatio = resX / resY;

	mat4 projection = glm::perspective(glm::radians(fov), aspectRatio, 0.01f, 10.0f);
	mat4 view = glm::lookAt(vec3(x, y, 2.0f), vec3(0.0f), World::Up);
	mat4 viewProjection = projection * view;

	perFrameUniform.viewProjection = viewProjection;
	perFrameUniform.uiOrthoProjection = glm::ortho(0.0f, resX, resY, 0.0f);

	curGraphicsDevice->UpdateBuffer(perFrameBuffer, &perFrameUniform, sizeof(PerFrameUniforms));

	if (curDemo != nullptr)
	{
		curDemo->Update();

		if (curGraphicsDevice != nullptr)
		{
			// do demo rendering
			curDemo->Draw(curGraphicsDevice);

			// Draw default demo system UI, must be done after demo Update, in case the demo changes the rendering API
			DrawBaseUI();
		}
	}
}

void DemoSystem::DrawBaseUI()
{
	UIManager::StartFrame();

	// Draw the base demo UI
	UIManager::DrawUI();

	// Tell the current demo to draw its UI now
	// curDemo->DrawUI();

	UIManager::EndFrame();
}

void DemoSystem::Destroy()
{
	if (curDemo != nullptr)
	{
		curDemo->Destroy();
		delete curDemo;
	}

	UIManager::Destroy();
}

void DemoSystem::SetDemo(Demo* newDemo)
{
	curDemo = newDemo;
	curDemo->SetDemoSystem(this);

	IGraphicsDevice* tempDevice = curGraphicsDevice;
	curDemo->Initialize();

	// if a render api exists, and the demo did not set a new API
	if (curGraphicsDevice != nullptr && tempDevice == curGraphicsDevice)
	{
		curDemo->CreateGraphics(curGraphicsDevice);
	}
}

void DemoSystem::SetGraphicsAPI(GraphicsAPIOptions api)
{
	// don't change the api if it is the same as the current one
	if (api == curAPIOption)
		return;

	curAPIOption = api;

	// cleanup any existing api
	if (curGraphicsDevice != nullptr)
	{
		ReleaseResources();

		curGraphicsDevice->Destroy();
		delete curGraphicsDevice;
		curGraphicsDevice = nullptr;
	}

	// create and initialize the given API choice
	switch (curAPIOption)
	{
		case GraphicsAPIOptions::DirectX11:
			curGraphicsDevice = new DX11Device();
		break;
		case GraphicsAPIOptions::OpenGL3:
			curGraphicsDevice = new GL3Device();
		break;
	}

	RenderInfo renderInfo;

	renderInfo.resolutionX	= curDisplaySettings.width;
	renderInfo.resolutionY	= curDisplaySettings.height;
	renderInfo.wndPointer	= (void*)&sdlInfo.info.win.hdc;

	curGraphicsDevice->Create(renderInfo);
	curGraphicsDevice->Initialize();

	curGraphicsDevice->SetVSync(curDisplaySettings.vsync);
	curGraphicsDevice->SetClearColor(vec4(0.1f, 0.1f, 0.1f, 1.0f));

	std::string windowTitle = curDisplaySettings.windowTitle + " - " + curGraphicsDevice->GetAPIName();
	SDL_SetWindowTitle(sdlWindow, windowTitle.c_str());

	CreateResources();
}

IGraphicsDevice* DemoSystem::GetGraphicsDevice()
{
	return curGraphicsDevice;
}

void DemoSystem::Clear()
{
	if (curGraphicsDevice == nullptr)
		return;

	curGraphicsDevice->Clear();
}

void DemoSystem::Present()
{
	if (curGraphicsDevice == nullptr)
		return;

	curGraphicsDevice->Present();
}

void DemoSystem::MakeWindow(const DisplaySettings &newSettings)
{
	curDisplaySettings = newSettings;

	uint32 wndFlags = 0;
	int32 wndX = 0;
	int32 wndY = 0;

	switch (curDisplaySettings.fullscreenMode)
	{
		case FullscreenMode::Windowed:
			// place window in center of screen if not fullscreen
			wndX = (displayMode.w / 2) - (curDisplaySettings.width / 2);
			wndY = (displayMode.h / 2) - (curDisplaySettings.height / 2);
		break;
		case FullscreenMode::BorderlessWindowed:
			wndFlags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN;
		break;
		case FullscreenMode::Fullscreen:
			wndFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN;
		break;
	}

	sdlWindow = SDL_CreateWindow(curDisplaySettings.windowTitle.c_str(), wndX, wndY, curDisplaySettings.width, curDisplaySettings.height, wndFlags);
	
	SDL_VERSION(&sdlInfo.version);
	if (!SDL_GetWindowWMInfo(sdlWindow, &sdlInfo))
	{
		// std::cout << "error getting sdl window info!\n";
	}

	// std::cout << "Running SDL Version : " << (int)sdlInfo.version.major << "." << (int)sdlInfo.version.minor << "." << (int)sdlInfo.version.patch << "\n";
}

void DemoSystem::SetDisplaySettings(const DisplaySettings &newSettings)
{
	if (curDisplaySettings.vsync != newSettings.vsync && curGraphicsDevice)
	{
		curGraphicsDevice->SetVSync(newSettings.vsync);
	}

	if (curDisplaySettings.width != newSettings.width ||
		curDisplaySettings.height != newSettings.height ||
		curDisplaySettings.refreshRate != newSettings.refreshRate)
	{
		SDL_DisplayMode target, closest;

		// Set the desired display mode
		target.w = newSettings.width;
		target.h = newSettings.height;
		target.refresh_rate = newSettings.refreshRate;
		target.format = 0;
		target.driverdata = 0;

		// Check that there was no error
		if (SDL_GetClosestDisplayMode(0, &target, &closest) != NULL)
		{
			SDL_SetWindowDisplayMode(sdlWindow, &closest);
		}
		else
		{
			LOG_ERROR("SDL Error getting display mode %ix%i@%i", newSettings.width, newSettings.height, newSettings.refreshRate);
		}

		SDL_SetWindowSize(sdlWindow, newSettings.width, newSettings.height);

		if (curGraphicsDevice)
		{
			curGraphicsDevice->OnResolutionChanged(newSettings.width, newSettings.height);
		}
	}

	if (curDisplaySettings.fullscreenMode != newSettings.fullscreenMode)
	{
		uint32 fullscreenFlags = 0; // default windowed

		switch (newSettings.fullscreenMode)
		{
			case FullscreenMode::BorderlessWindowed:
				fullscreenFlags = SDL_WINDOW_FULLSCREEN_DESKTOP;
				break;
			case FullscreenMode::Fullscreen:
				fullscreenFlags = SDL_WINDOW_FULLSCREEN;
				break;
		}

		CHECK_SDL(SDL_SetWindowFullscreen(sdlWindow, fullscreenFlags));
	}

	curDisplaySettings = newSettings;
}

void DemoSystem::SetFOV(float newFOV)
{
	fov = newFOV;
}

void DemoSystem::SetClipboardText(const char* text)
{
	SDL_SetClipboardText(text);
}

void DemoSystem::GetDisplaySize(uint32* width, uint32* height)
{
	uint32 w, h;

	if (curDisplaySettings.fullscreenMode == FullscreenMode::Windowed ||
		curDisplaySettings.fullscreenMode == FullscreenMode::Fullscreen)
	{
		w = curDisplaySettings.width;
		h = curDisplaySettings.height;
	}
	else
	{
		int32 displayIndex = SDL_GetWindowDisplayIndex(sdlWindow);

		SDL_DisplayMode mode;
		SDL_GetDesktopDisplayMode(displayIndex, &mode);

		w = mode.w;
		h = mode.h;
	}

	if (width)
		*width = w;

	if (height)
		*height = h;
}

const DisplaySettings& DemoSystem::GetDisplaySettings()
{
	return curDisplaySettings;
}

const std::vector<DisplayMode>& DemoSystem::GetAvailableDisplayModes()
{
	return displayModeList;
}

const char* DemoSystem::GetClipboardText()
{
	return SDL_GetClipboardText();
}

Texture* DemoSystem::LoadTexture(std::string fileName)
{
	// load image from file
	int32 texWidth, texHeight, texDepth;
	uint8* imageData = stbi_load(fileName.c_str(), &texWidth, &texHeight, &texDepth, 4);

	if (imageData == nullptr)
	{
		const char *result = stbi_failure_reason();
		LOG_ERROR("Could not load texture [%s] : %s", fileName.c_str(), result);
		return nullptr;
	}

	// create texture resource in graphics API
	TextureSettings settings;
	settings.width = texWidth;
	settings.height = texHeight;
	settings.filterMode = TextureFilterMode::Trilinear;
	settings.mipMaps = true;

	Texture* newTexture = curGraphicsDevice->CreateTexture(imageData, settings);

	stbi_image_free(imageData);

	return newTexture;
}

bool DemoSystem::IsRunning()
{
	return running;
}

void DemoSystem::SetRunning(bool IsRunning)
{
	running = IsRunning;
}

void DemoSystem::CreateResources()
{
	// create the resources required by the demo system
	UIManager::CreateGraphics();

	perFrameBuffer = curGraphicsDevice->CreateBuffer(nullptr, sizeof(PerFrameUniforms),
												BufferTarget::Uniform, BufferUsage::Stream);
	curGraphicsDevice->SetUniformBuffer(0, perFrameBuffer, ShaderStage::Vertex);

	// ================

	// if we have an active demo tell it to recreate its graphics in the new API
	if (curDemo != nullptr)
	{
		curDemo->CreateGraphics(curGraphicsDevice);
	}
}

void DemoSystem::ReleaseResources()
{
	// Release resources created by the demo system
	UIManager::ReleaseGraphics();
	if (perFrameBuffer) { curGraphicsDevice->ReleaseBuffer(perFrameBuffer); perFrameBuffer = nullptr; }

	// tell any active demo to release its resources
	if (curDemo != nullptr)
	{
		curDemo->ReleaseGraphics(curGraphicsDevice);
	}
}

void DemoSystem::OnRenderAPIChanged()
{

}