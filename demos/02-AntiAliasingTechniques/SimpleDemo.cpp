#include "SimpleDemo.h"
#include "DemoSystem.h"

struct VERTEX { vec3 pos; vec2 texCoord; uint32_t color; };
struct UI_VERTEX { vec2 pos; vec2 texCoord; uint32_t color; };

//struct VERTEX_COLOR { vec3 pos; vec4 color; };

#define GET_COLOR32(r, g, b, a) static_cast<uint8_t>(r*255.0f) | (static_cast<uint8_t>(g*255.0f)) << 8 | (static_cast<uint8_t>(b*255.0f)) << 16 | (static_cast<uint8_t>(a*255.0f)) << 24

static const uint32_t color = GET_COLOR32(1, 1, 1, 1);

static const VERTEX quadVertices[] =
{
	{ vec3(-0.5f, -0.5f, 0.0f), vec2(0, 0), color }, // 0
	{ vec3(-0.5f, 0.5f, 0.0f), vec2(1, 0), color },	// 1
	{ vec3(0.5f, 0.5f, 0.0f), vec2(1, 1), color },	// 2
	{ vec3(0.5f, -0.5f, 0.0f), vec2(0, 1), color }	// 3
};

static const uint16 quadIndices[] = { 0, 1, 3, 3, 1, 2 };

static const VERTEX triVertices[] =
{
	{ vec3(-0.5f, -0.5f, 0.0f), vec2(0, 0), color }, // 0
	{ vec3(0.0f, 0.5f, 0.0f), vec2(1, 0), color },	// 1
	{ vec3(0.5f,-0.5f, 0.0f), vec2(1, 1), color },	// 2
};

static const uint16 triIndices[] = { 0, 1, 2 };

static VERTEX cubeVertices[] = 
{
	// TOP
	{ vec3(-0.5f, -0.5f, -0.5f), vec2(0, 0), color }, // 0
	{ vec3( 0.5f, -0.5f, -0.5f), vec2(1, 0), color }, // 1
	{ vec3( 0.5f,  0.5f, -0.5f), vec2(1, 1), color }, // 2
	{ vec3(-0.5f,  0.5f, -0.5f), vec2(0, 1), color }, // 3

	// BOTTOM
	{ vec3(-0.5f, -0.5f, 0.5f), vec2(0, 0), color },	// 4
	{ vec3( 0.5f, -0.5f, 0.5f), vec2(1, 0), color },	// 5
	{ vec3( 0.5f,  0.5f, 0.5f), vec2(1, 1), color },	// 6
	{ vec3(-0.5f,  0.5f, 0.5f), vec2(0, 1), color },	// 7
};

static uint16 cubeIndices[] = 
{
	0, 1, 2, 0, 2, 3,	// BOTTOM	- Y
	7, 6, 5, 7, 5, 4,	// TOP		+ Y

	2, 1, 5, 2, 5, 6,	// FRONT	+ X
	0, 3, 7, 0, 7, 4	// BACK		- X


};

static uint16 texCubeIndices[36];
static VERTEX texCubeVertices[36];

void SimpleDemo::SetFace(uint32 offset, uint32 i1, uint32 i2, uint32 i3, uint32 i4)
{
	uint32 idx = offset;

	texCubeVertices[idx] = { cubeVertices[i1].pos, vec2(0,0), color };
	texCubeVertices[idx + 1] = { cubeVertices[i2].pos, vec2(1,0), color };
	texCubeVertices[idx + 2] = { cubeVertices[i3].pos, vec2(1,1), color };

	texCubeVertices[idx + 3] = { cubeVertices[i1].pos, vec2(0,0), color };
	texCubeVertices[idx + 4] = { cubeVertices[i3].pos, vec2(1,1), color };
	texCubeVertices[idx + 5] = { cubeVertices[i4].pos, vec2(0,1), color };
}

void SimpleDemo::Initialize()
{
	WindowSettings wndSettings;

	wndSettings.fullscreen = false;
	wndSettings.width = 1152;
	wndSettings.height = 648;
	wndSettings.windowTitle = "Simple";

	for (uint32 i = 0; i < 36; i++)
	{
		texCubeIndices[i] = i;
	}

	uint32 idx = 0;
	SetFace(0, 0, 1, 2, 3); // Bottom
	SetFace(6, 7, 6, 5, 4); // Top

	SetFace(12, 2, 1, 5, 6);
	SetFace(18, 0, 3, 7, 4);

	SetFace(24, 3, 2, 6, 7);
	SetFace(30, 1, 0, 4, 5);

	demoSystem->MakeWindow(wndSettings);
	//demoSystem->SetRenderAPI(RenderAPIOptions::OpenGL3);
	demoSystem->SetRenderAPI(RenderAPIOptions::DirectX11);
}

void SimpleDemo::Update()
{
	if (demoSystem->input.GetKeyPressed(KeyCode::F1))
	{
		LOG("Setting render API to OpenGL");
		demoSystem->SetRenderAPI(RenderAPIOptions::OpenGL3);
	}

	if (demoSystem->input.GetKeyPressed(KeyCode::F2))
	{
		LOG("Setting render API to DirectX");
		demoSystem->SetRenderAPI(RenderAPIOptions::DirectX11);
	}

	static bool triangle = true;

	uint32_t color = GET_COLOR32(1, 1, 1, 1);

	static float lastTime = 0.0f;
	if (Time::time - lastTime > 1.0f)
	{
		IRenderAPI* renderAPI = demoSystem->GetRenderAPI();

		VERTEX quadVertices[] =
		{
			{ vec3(-0.5f, -0.5f, 0.0f), vec2(0, 0), color }, // 0
			{ vec3(-0.5f, 0.5f, 0.0f), vec2(1, 0), color },	// 1
			{ vec3(0.5f, 0.5f, 0.0f), vec2(1, 1), color },	// 2
			{ vec3(0.5f, -0.5f, 0.0f), vec2(0, 1), color }	// 3
		};

		vec3 a = World::Up;

		uint16_t quadIndices[] = { 0, 1, 3, 3, 1, 2 };

		VERTEX triVertices[] =
		{
			{ vec3(-0.5f, -0.5f, 0.0f), vec2(0, 0), color }, // 0
			{ vec3(0.0f, 0.5f, 0.0f), vec2(1, 0), color },	// 1
			{ vec3(0.5f,-0.5f, 0.0f), vec2(1, 1), color },	// 2
		};

		uint16_t triIndices[] = { 0, 1, 2};

		MeshData myMeshData(&quadVertices, 4, &quadIndices, 6);

		/*UI_VERTEX quadVertices[] =
		{
			{ vec2(-0.5f, -0.5f), vec2(0, 0), color }, // 0
			{ vec2(-0.5f, 0.5f), vec2(1, 0), color },	// 1
			{ vec2(0.5f, 0.5f), vec2(1, 1), color },	// 2
			{ vec2(0.5f, -0.5f), vec2(0, 1), color }	// 3
		};

		uint16_t quadIndices[] = { 0, 1, 3, 3, 1, 2 };

		UI_VERTEX triVertices[] =
		{
			{ vec2(-0.5f, -0.5f), vec2(0, 0), color }, // 0
			{ vec2(0.0f, 0.5f), vec2(1, 0), color },	// 1
			{ vec2(0.5f,-0.5f), vec2(1, 1), color },	// 2
		};

		uint16_t triIndices[] = { 0, 1, 2 };

		MeshData vertexData(quadVertices, 4);
		MeshData indexData(quadIndices, 6);*/

		if (triangle)
		{
			myMeshData = MeshData(&triVertices, 3, &triIndices, 3);
		}
		
		//renderAPI->UpdateMesh(myMesh, myMeshData);

		triangle = !triangle;
		lastTime = Time::time;
	}
}

void SimpleDemo::Destroy()
{

}

void SimpleDemo::CreateGraphics(IRenderAPI* renderAPI)
{
	VertexAttributes vertAttributeFlags = VertexAttributes::Position |
										  VertexAttributes::TexCoord |
										  VertexAttributes::Color32;

	MeshData myMeshData(texCubeVertices, 36, texCubeIndices, 36);
	myMesh = renderAPI->CreateMesh(myMeshData, vertAttributeFlags, BufferUsage::Stream);
	myShader = renderAPI->CreateShader("TestShader");

	// create texture
	myTexture = demoSystem->LoadTexture("Resources/TestTexture.png");
}

void SimpleDemo::Draw(IRenderAPI* renderAPI)
{
	if (true)
	{
		renderAPI->SetShader(myShader);
		renderAPI->SetTexture(myTexture, 0);
		renderAPI->DrawMeshIndexed(myMesh, 0);
	}
}

void SimpleDemo::ReleaseGraphics(IRenderAPI* renderAPI)
{
	renderAPI->ReleaseShader(myShader);
	renderAPI->ReleaseMesh(myMesh);
	renderAPI->ReleaseTexture(myTexture);
	//renderAPI->ReleaseMesh(myMesh2);
}