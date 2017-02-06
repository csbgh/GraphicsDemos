#ifndef _UI_MANAGER_H
#define _UI_MANAGER_H

#include "DemoTypes.h"

// forward declarations
class Mesh;
class Texture;
class Shader;
class DemoSystem;
class Buffer;
class BlendState;
class DepthStencilState;
class MeshDataList;

struct MeshData;
struct ImDrawData;

struct BaseUIValues
{
	BaseUIValues()
	{
		graphicsAPIItemIndex = 0;
		fov = 60.0f;
		refreshRateIndex = 0;
		resolutionIndex = 0;
		fullscreenIndex = 0;
		vsyncChecked = true;
	}

	int32 graphicsAPIItemIndex;
	float fov;
	int32 refreshRateIndex;
	int32 resolutionIndex;
	int32 fullscreenIndex;
	bool vsyncChecked;
};

class UIManager
{
public:

	static void Initialize(DemoSystem* system);
	static void Destroy();

	static void CreateGraphics();
	static void ReleaseGraphics();

	static void StartFrame();
	static void DrawUI();
	static void EndFrame();

private:

	static void CreateFontTexture();

	// UI Callbacks
	static void ImGuiDraw(ImDrawData* drawData);

	static const char* GetClipboardText();
	static void SetClipboardText(const char* text);

	static bool GetRefreshRateCombo(void* data, int idx, const char** out_text);
	static bool GetResolutionCombo(void* data, int idx, const char** out_text);

	// Helper methods
	static void StartRow(const char* label, float labelWidth, float inputWidth);
	static void EndRow();

	// state of the UI values in the last frame, and those being set in the current frame
	static BaseUIValues lastUIValues;
	static BaseUIValues curUIValues;
	static bool applySettingsPressed;

	// UI graphics states
	static DepthStencilState* uiDepthStencilState;

	// UI graphics resources
	static Mesh* uiMesh;
	static Texture* fontTexture;
	static Shader* uiShader;
	static BlendState* blendState;

	// ui temp mesh data
	static MeshDataList uiMeshDataList;

	static DemoSystem* demoSystem;

};

#endif // _UI_MANAGER_H