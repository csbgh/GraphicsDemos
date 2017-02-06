#include "UIManager.h"
#include "InputDefinitions.h"
#include "DemoSystem.h"
#include "IGraphicsDevice.h"

#include <imgui\imgui.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\matrix_major_storage.hpp>

BaseUIValues UIManager::lastUIValues;
BaseUIValues UIManager::curUIValues;
bool UIManager::applySettingsPressed = false;

DepthStencilState* UIManager::uiDepthStencilState;

Mesh* UIManager::uiMesh;
Shader* UIManager::uiShader;
Texture* UIManager::fontTexture;
DemoSystem* UIManager::demoSystem;
BlendState* UIManager::blendState;

MeshDataList UIManager::uiMeshDataList(128);
//MeshData* UIManager::uiIndices;

void UIManager::Initialize(DemoSystem* system)
{
	demoSystem = system;

	//uiVertices = new MeshData[1024];
	//uiIndices = new MeshData[1024];

	ImGuiIO& io = ImGui::GetIO();

	// setup key mappings
	io.KeyMap[ImGuiKey_Tab] = static_cast<int32>(KeyCode::TAB);
	io.KeyMap[ImGuiKey_LeftArrow] = static_cast<int32>(KeyCode::LEFT);
	io.KeyMap[ImGuiKey_RightArrow] = static_cast<int32>(KeyCode::RIGHT);
	io.KeyMap[ImGuiKey_UpArrow] = static_cast<int32>(KeyCode::UP);
	io.KeyMap[ImGuiKey_DownArrow] = static_cast<int32>(KeyCode::DOWN);
	io.KeyMap[ImGuiKey_Delete] = static_cast<int32>(KeyCode::DELETE_KEY);
	io.KeyMap[ImGuiKey_Backspace] = static_cast<int32>(KeyCode::BACKSPACE);
	io.KeyMap[ImGuiKey_Enter] = static_cast<int32>(KeyCode::ENTER);
	io.KeyMap[ImGuiKey_Escape] = static_cast<int32>(KeyCode::ESC);
	io.KeyMap[ImGuiKey_A] = static_cast<int32>(KeyCode::A);
	io.KeyMap[ImGuiKey_C] = static_cast<int32>(KeyCode::B);
	io.KeyMap[ImGuiKey_V] = static_cast<int32>(KeyCode::C);
	io.KeyMap[ImGuiKey_X] = static_cast<int32>(KeyCode::D);
	io.KeyMap[ImGuiKey_Y] = static_cast<int32>(KeyCode::E);
	io.KeyMap[ImGuiKey_Z] = static_cast<int32>(KeyCode::F);

	// setup callbacks
	io.RenderDrawListsFn = &UIManager::ImGuiDraw;
	io.SetClipboardTextFn = &UIManager::SetClipboardText;
	io.GetClipboardTextFn = &UIManager::GetClipboardText;

	// io.ImeWindowHandle
}

void UIManager::Destroy()
{
	ImGui::Shutdown();
}

void UIManager::CreateGraphics()
{
	IGraphicsDevice* gDevice = demoSystem->GetGraphicsDevice();

	// create UI render states
	DepthStencilStateDesc dsDesc;

	dsDesc.depthEnabled = false;
	dsDesc.depthWriteEnabled = true;
	dsDesc.depthFunc = ComparisonFunc::Always;

	dsDesc.stencilEnabled = false;
	dsDesc.frontFace.stencilfailOp = dsDesc.frontFace.depthFailOp = dsDesc.frontFace.stencilPassOp = StencilOp::Keep;
	dsDesc.frontFace.stencilFunc = ComparisonFunc::Always;
	dsDesc.backFace = dsDesc.frontFace;

	uiDepthStencilState = gDevice->CreateDepthStencilState(dsDesc);

	// create ui shader
	uiShader = gDevice->CreateShader("UIShader");

	// create blend state
	BlendProperties properties;

	properties.enabled = true;
	properties.srcBlend = BlendFactor::SrcAlpha;
	properties.dstBlend = BlendFactor::InvSrcAlpha;
	properties.blendOp = BlendOperation::Add;
	properties.srcBlendAlpha = BlendFactor::InvSrcAlpha;
	properties.dstBlendAlpha = BlendFactor::Zero;
	properties.blendOpAlpha = BlendOperation::Add;
	properties.colorMask = static_cast<uint8>(ColorMask::All);

	blendState = gDevice->CreateBlendState(properties);

	// create empty mesh
	VertexAttributes vertAttributeFlags = VertexAttributes::UIPosition  |
										  VertexAttributes::TexCoord |
										  VertexAttributes::Color32;
	ImDrawVert vert;
	uint16 index = 0;

	MeshData meshData(nullptr, 1, nullptr, 1);
	uiMesh = gDevice->CreateMesh(meshData, vertAttributeFlags, BufferUsage::Stream);

	// create ui font texture
	CreateFontTexture();
}

void UIManager::ReleaseGraphics()
{
	IGraphicsDevice* gDevice = demoSystem->GetGraphicsDevice();
}

void UIManager::StartFrame()
{
	ImGuiIO& io = ImGui::GetIO();

	// update mouse state
	io.MouseDown[0] = demoSystem->input.GetMouse(MouseButton::LEFT);
	io.MouseDown[1] = demoSystem->input.GetMouse(MouseButton::RIGHT);
	io.MouseDown[2] = demoSystem->input.GetMouse(MouseButton::MIDDLE);

	// Get the display size and the resolution, so that we can map the mouse x,y coordinates correctly
	uint32 dispWidth, dispHeight;
	demoSystem->GetDisplaySize(&dispWidth, &dispHeight);

	const DisplaySettings& curDisplay = demoSystem->GetDisplaySettings();
	float resX = static_cast<float>(curDisplay.width);
	float resY = static_cast<float>(curDisplay.height);

	// Get the current mouse position
	int32 x, y;
	demoSystem->input.GetMousePos(x, y);

	float mx = static_cast<float>(x) * (resX/static_cast<float>(dispWidth));
	float my = static_cast<float>(y) * (resY/static_cast<float>(dispHeight));

	// Set ImGui values
	io.MousePos = ImVec2(mx, my);
	io.DisplaySize = ImVec2(static_cast<float>(curDisplay.width), static_cast<float>(curDisplay.height));
	io.DisplayFramebufferScale = ImVec2(1, 1);
	io.DeltaTime = Time::deltaTime();

	ImGui::NewFrame();
}

void UIManager::DrawUI()
{
	if (true)
	{
		// Stats window
		ImGui::SetNextWindowPos(ImVec2(20, 16), ImGuiSetCond_Once);
		ImGui::SetNextWindowSize(ImVec2(138, 72), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Statistics");

		ImGui::Text("ms/frame : %.1f", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("fps      : %.1f", ImGui::GetIO().Framerate);

		ImGui::End();

		// Settings window
		ImGui::SetNextWindowPos(ImVec2(892, 18), ImGuiSetCond_Once);
		ImGui::SetNextWindowSize(ImVec2(239, 0.0f), ImGuiSetCond_Once);
		ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_AutoResizeYOnly);

		float padRight = 10.0f;
		float labelWidth = 110.0f;
		float inputWidth = ImGui::GetWindowWidth() - labelWidth - padRight;

		if (ImGui::CollapsingHeader("Graphics", nullptr, true, true))
		{
			StartRow("Graphics API", labelWidth, inputWidth);
			ImGui::Combo("##GraphicsAPI", &curUIValues.graphicsAPIItemIndex, "DirectX 11\0OpenGL 3\0\0");
			EndRow();

			StartRow("FOV", labelWidth, inputWidth);
			ImGui::SliderFloat("##FOV", &curUIValues.fov, 1.0f, 90.0f, "%.1f");

			//static int fovI = 60;
			//ImGui::SliderInt("##FOV", &fovI, 1, 90, "%.0f");
			//curUIValues.fov = static_cast<float>(fovI);

			EndRow();
		}

		//const char* refreshRateItems[] = { "59","60","75" };
		const char* resolutionItems[] = { "1920x1080","1366x768","640x480" };
		const char* fullscreenItems[] = { "Windowed", "Borderless","Fullscreen" };

		const std::vector<DisplayMode>& displayModes = demoSystem->GetAvailableDisplayModes();

		if (ImGui::CollapsingHeader("Display", nullptr, true, true))
		{
			StartRow("Resolution", labelWidth, inputWidth);
			ImGui::Combo("##Resolution", &curUIValues.resolutionIndex, &GetRefreshRateCombo, (void*)(&displayModes), displayModes.size());
			EndRow();

			StartRow("Fullscreen", labelWidth, inputWidth);
			ImGui::Combo("##Fullscreen", &curUIValues.fullscreenIndex, fullscreenItems, 3);
			EndRow();

			StartRow("VSync", labelWidth, inputWidth);
			ImGui::Checkbox("##VSync", &curUIValues.vsyncChecked);
			EndRow();
		}

		applySettingsPressed = ImGui::Button("Apply", ImVec2(ImGui::GetWindowWidth() - 15, 20));
	}
}

void UIManager::EndFrame()
{
	ImGui::End();
	ImGui::Render();

	if (applySettingsPressed)
	{
		// get a copy of the current display settings to modify
		DisplaySettings newSettings = demoSystem->GetDisplaySettings();
		bool displaySettingsChanged = false;

		// check if vsync was enabled/disabled
		if (lastUIValues.vsyncChecked != curUIValues.vsyncChecked)
		{
			newSettings.vsync = curUIValues.vsyncChecked;
			displaySettingsChanged = true;
		}

		// check if the fullscreen mode was changed
		if (lastUIValues.fullscreenIndex != curUIValues.fullscreenIndex)
		{
			switch (curUIValues.fullscreenIndex)
			{
				case 0:
					newSettings.fullscreenMode = FullscreenMode::Windowed;
				break;
				case 1:
					newSettings.fullscreenMode = FullscreenMode::BorderlessWindowed;
				break;
				case 2:
					newSettings.fullscreenMode = FullscreenMode::Fullscreen;
				break;
			}
			displaySettingsChanged = true;
		}

		// check if the resolution was changed
		if (lastUIValues.resolutionIndex != curUIValues.resolutionIndex)
		{
			const std::vector<DisplayMode>& displayModes = demoSystem->GetAvailableDisplayModes();

			newSettings.width = displayModes[curUIValues.resolutionIndex].width;
			newSettings.height = displayModes[curUIValues.resolutionIndex].height;
			newSettings.refreshRate = displayModes[curUIValues.resolutionIndex].refreshRate;

			displaySettingsChanged = true;
		}

		// update the display settings if they have changed
		if (displaySettingsChanged)
		{
			demoSystem->SetDisplaySettings(newSettings);
		}

		// Set a new graphics API if it changed in the UI
		if (lastUIValues.graphicsAPIItemIndex != curUIValues.graphicsAPIItemIndex)
		{
			if (curUIValues.graphicsAPIItemIndex == 0)
			{
				demoSystem->SetGraphicsAPI(GraphicsAPIOptions::DirectX11);
			}
			else
			{
				demoSystem->SetGraphicsAPI(GraphicsAPIOptions::OpenGL3);
			}
		}

		lastUIValues = curUIValues;
	}

	if (lastUIValues.fov != curUIValues.fov)
	{
		demoSystem->SetFOV(curUIValues.fov);
		lastUIValues.fov = curUIValues.fov;
	}

	applySettingsPressed = false;
}

void UIManager::ImGuiDraw(ImDrawData* drawData)
{
	ImGuiIO& io = ImGui::GetIO();
	IGraphicsDevice* gDevice = demoSystem->GetGraphicsDevice();

	// copy ImGui vertex/index data to MeshData objects and copy that data to our uiMesh
	uiMeshDataList.dataCount = drawData->CmdListsCount;

	uiMeshDataList.vertexCount = 0;
	uiMeshDataList.indexCount = 0;
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = drawData->CmdLists[n];

		uiMeshDataList.SetVertices(n, &cmd_list->VtxBuffer[0], cmd_list->VtxBuffer.size() * sizeof(ImDrawVert));
		uiMeshDataList.vertexCount += cmd_list->VtxBuffer.size();

		uiMeshDataList.SetIndices(n, &cmd_list->IdxBuffer[0], cmd_list->IdxBuffer.size() * sizeof(uint16));
		uiMeshDataList.indexCount += cmd_list->IdxBuffer.size();
	}

	gDevice->UpdateMesh(uiMesh, uiMeshDataList);

	// setup ortho projection
	//UIUniforms temp;
	//temp.orthoProjection = glm::ortho(0.0f, 1152.0f, 648.0f, 0.0f);

	//gDevice->UpdateBuffer(uiUniformBuffer, &temp, sizeof(UIUniforms));
	DepthStencilState* oldDepthStencilState = gDevice->GetCurrentDepthStencilState();
	
	gDevice->SetDepthStencilState(uiDepthStencilState);
	gDevice->SetBlendState(blendState);

	gDevice->SetShader(uiShader);

	#define MAX_NUM_SCISSOR_RECTS 16

	DSRect oldRects[MAX_NUM_SCISSOR_RECTS];
	uint32 numRects = 0;
	gDevice->GetScissorRects(&numRects, oldRects);

	// execute ImGui draw calls
	int vtx_offset = 0;
	int idx_offset = 0;
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmdList = drawData->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmdList->CmdBuffer.size(); cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmdList, pcmd);
			}
			else
			{
				DSRect scissor =
				{
					static_cast<int32>(pcmd->ClipRect.x),
					static_cast<int32>(pcmd->ClipRect.y),
					static_cast<int32>(pcmd->ClipRect.z),
					static_cast<int32>(pcmd->ClipRect.w)
				};
				gDevice->SetTexture((Texture*)pcmd->TextureId, 0);
				gDevice->SetScissorRects(1, &scissor);
				gDevice->DrawMeshIndexed(uiMesh, pcmd->ElemCount, vtx_offset, idx_offset);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += cmdList->VtxBuffer.size();
	}

	// reset previous state
	gDevice->SetScissorRects(numRects, oldRects);
	gDevice->SetDepthStencilState(oldDepthStencilState);
}

const char* UIManager::GetClipboardText()
{
	return demoSystem->GetClipboardText();
}

void UIManager::SetClipboardText(const char* text)
{
	demoSystem->SetClipboardText(text);
}

void UIManager::CreateFontTexture()
{
	ImGuiIO& io = ImGui::GetIO();
	IGraphicsDevice* gDevice = demoSystem->GetGraphicsDevice();

	//io.Fonts->AddFontFromFileTTF("Resources/Fonts/SourceCodePro-Semibold.ttf", 16, NULL, io.Fonts->GetGlyphRangesDefault());
	ImFontConfig fc;

	//io.Fonts->AddFontDefault(&fc);
	//io.Fonts->AddFontFromFileTTF("Resources/Fonts/roboto.regular.ttf", 16, NULL, io.Fonts->GetGlyphRangesDefault());

	// Build texture atlas
	uint8* pixels;
	int32 width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	// upload texture to graphics api
	TextureSettings fontSettings = TextureSettings(width, height,
												   TextureFormat::RGBA, TextureWrapMode::Repeat,
												   TextureFilterMode::Trilinear, 0.0f, false);

	fontTexture = gDevice->CreateTexture(pixels, fontSettings);
	io.Fonts->TexID = (void*)fontTexture;
}

bool UIManager::GetRefreshRateCombo(void* data, int idx, const char** out_text)
{
	if (!data)
	{
		return false;
	}

	std::vector<DisplayMode>* displayModes = (std::vector<DisplayMode>*)data;

	if (out_text)
		*out_text = (*displayModes)[idx].displayModeStr.c_str();

	return true;
}

bool UIManager::GetResolutionCombo(void* data, int idx, const char** out_text)
{
	return true;
}

void UIManager::StartRow(const char* label, float labelWidth, float inputWidth)
{
	ImGui::PushItemWidth(labelWidth);
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Text(label);
	ImGui::PopItemWidth();
	ImGui::PushItemWidth(inputWidth);
	ImGui::SameLine(labelWidth);
}

void UIManager::EndRow()
{
	ImGui::PopItemWidth();
}