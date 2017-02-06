#ifndef _D3D11_API_H
#define _D3D11_API_H

#include "IGraphicsDevice.h"
#include "DX11Definitions.h"

#include <map>
#include <vector>

struct CachedInputLayout
{
	CachedInputLayout(ID3D11InputLayout* layout, uint32 strideSize)
	{
		inputLayout = layout;
		stride = strideSize;
	}

	ID3D11InputLayout* inputLayout = nullptr;
	uint32 stride = 0;
};

class D3D11Shader : public Shader
{
public:

	ID3D11VertexShader*	pVertexShader = nullptr;
	ID3D11PixelShader*	pPixelShader = nullptr;
};

struct D3D11Buffer
{
	D3D11Buffer(uint32 elementCount = 0, D3D11_USAGE usage = D3D11_USAGE_DEFAULT) :
		elementCount(elementCount),
		usage(usage)
	{}

	ID3D11Buffer* pBuffer = nullptr;
	uint32 elementCount = 0;
	D3D11_USAGE usage;
};

class D3D11Mesh : public Mesh
{
public:

	D3D11Mesh(DX11Buffer* vertexBuffer, DX11Buffer* indexBuffer,
			  uint32 vertexCount, uint32 indexCount,
			  CachedInputLayout* pInputLayout) :
		vertexBuffer(vertexBuffer),
		indexBuffer(indexBuffer),
		vertexCount(vertexCount),
		indexCount(indexCount),
		pInputLayout(pInputLayout)
	{}

	DX11Buffer* vertexBuffer;
	DX11Buffer* indexBuffer;
	uint32 vertexCount = 0;
	uint32 indexCount = 0;
	CachedInputLayout* pInputLayout = nullptr;

	uint32 offset = 0;
};

class D3D11Texture : public Texture
{
public:

	ID3D11ShaderResourceView* pTexResourceView = nullptr;
	ID3D11Texture2D* pTexture = nullptr;
	ID3D11SamplerState* pSampler = nullptr;
};

class D3D11UniformBuffer : public UniformBuffer
{
public:

	ID3D11Buffer* pBuffer;
};

class DX11Device : public IGraphicsDevice
{
public:

	std::string GetAPIName();

	bool Create(const RenderInfo& info);

	void Initialize() override;

	void Destroy();

	void Clear();

	void Present();

	void SetVSync(bool enabled);

	void SetShader(Shader* shader);

	void SetTexture(Texture* texture, uint32 slot);

	void SetClearColor(const vec4 &color);

	void SetViewport(int32 x, int32 y, int32 width, int32 height);

	void OnResolutionChanged(uint32 width, uint32 height);

	void DrawMesh(Mesh* mesh);
	void DrawMeshIndexed(Mesh* mesh, uint32 elementCount = 0, uint32 vertexOffset = 0, uint16 indexOffset = 0);

	Mesh* CreateMesh(const MeshData &meshData, VertexAttributes vertexAttributeFlags, BufferUsage usage);
	Mesh* CreateMesh(const MeshDataList &meshData, VertexAttributes vertexAttributeFlags, BufferUsage usage);

	void UpdateMesh(Mesh* mesh, const MeshData &meshData);
	void UpdateMesh(Mesh* mesh, const MeshDataList &meshData);

	void ReleaseMesh(Mesh* mesh);

	Shader* CreateShader(const std::string &name);
	void ReleaseShader(Shader* shader);

	Texture* CreateTexture(uint8 *data, const TextureSettings &settings);
	void ReleaseTexture(Texture* pTexture);

	// Uniform Buffer Resource Handling
	void SetUniformBuffer(uint32 slot, Buffer* buffer, ShaderStage stage) ;

	// Buffer Resource Handling
	DX11Buffer* CreateBuffer(const void* data, uint32 size, BufferTarget target, BufferUsage usage);
	DX11Buffer* CreateBuffer(const std::vector<BufferData> &data, uint32 dataCount, uint32 bufferSize, BufferTarget target, BufferUsage usage);

	void UpdateBuffer(Buffer* buffer, const void* data, uint32 size);
	void UpdateBuffer(Buffer* buffer, const std::vector<BufferData> &data, uint32 dataCount, uint32 bufferSize);

	void ReleaseBuffer(Buffer* buffer);

	// Scissor
	void SetScissorRects(uint32 numRects, const DSRect* pRects);

	void GetScissorRects(uint32* pNumRects, DSRect* pRects);

	// Blend State
	BlendState* CreateBlendState(BlendProperties properties);

	void SetBlendState(BlendState* state);

	// Depth/Stencil State
	DepthStencilStateD3D11* CreateDepthStencilState(DepthStencilStateDesc& desc);

	DepthStencilState* GetCurrentDepthStencilState();

	void SetDepthStencilState(DepthStencilState* state);

private:

	bool CreateDepthStencil(uint32 width, uint32 height);

	// helper method for compiling shaders
	HRESULT CompileShaderFromFile(const std::wstring &fileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	CachedInputLayout* GetInputLayout(VertexAttributes vertexAttributeFlags);

	HRESULT GetDummyLayoutShader(VertexAttributes vertexAttributeFlags, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	// helper function to create and reisze buffers
	inline ID3D11Texture2D* CreateTexture2D_D3D(uint32 width, uint32 height, DXGI_FORMAT format, D3D11_USAGE usage,
												uint32 bindFlags, uint32 miscFlags = 0, uint32 mipLevels = 1, uint32 arraySize = 1,
												uint32 sampleCount = 1, uint32 sampleQuality = 0, uint32 cpuAccessFlag = 0);

	void SetBufferDesc(D3D11_BUFFER_DESC &desc, uint32 bufferSize, BufferTarget target, BufferUsage usage)
	{
		D3D11_USAGE dxUsage = DX11UsageMap[static_cast<int32>(usage)];
		UINT bindFlags = DX11BindTargetMap[static_cast<int32>(target)];

		desc.ByteWidth = bufferSize;
		desc.Usage = dxUsage;
		desc.BindFlags = bindFlags;
		desc.CPUAccessFlags = dxUsage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.MiscFlags = 0;
	}

	void ExpandBuffer(DX11Buffer* dxBuffer, uint32 newSize)
	{
		if (dxBuffer->size < newSize)
		{
			// release old buffer
			if (dxBuffer->pBuffer)
			{
				dxBuffer->pBuffer->Release();
				dxBuffer->pBuffer = NULL;
			}

			// set new size and create buffer
			dxBuffer->desc.ByteWidth = newSize;
			HR(pDevice->CreateBuffer(&dxBuffer->desc, NULL, &dxBuffer->pBuffer));
			dxBuffer->size = newSize;
		}
	}

	// map of cached input layouts
	std::map<int32, CachedInputLayout*> inputLayouts;

	RenderInfo renderInfo;

	// handles to window
	HDC windowContext;
	HWND windowHandle;

	// States
	DepthStencilStateD3D11* defaultDepthStencilState;
	DepthStencilStateD3D11* curDepthStencilState;

	// pointers to DirectX objects 
	ID3D11Device*           pDevice			  = nullptr;
	ID3D11DeviceContext*	pDeviceContext	  = nullptr;
	IDXGISwapChain*         pSwapChain		  = nullptr;
	ID3D11RenderTargetView* pRenderTargetView = nullptr;
	ID3D11Texture2D*		pDepthStencil	  = nullptr;
	ID3D11DepthStencilView* pDepthStencilView = nullptr;

	ID3D11RasterizerState*	 pRasterizerState	= nullptr;

	// DirectX state
	uint32 syncInterval = 1;
	vec4 clearColor;
};

ID3D11Texture2D* DX11Device::CreateTexture2D_D3D(uint32 width, uint32 height, DXGI_FORMAT format, D3D11_USAGE usage,
												 uint32 bindFlags, uint32 miscFlags, uint32 mipLevels, uint32 arraySize,
												 uint32 sampleCount, uint32 sampleQuality, uint32 cpuAccessFlags)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

	desc.Width				= width;
	desc.Height				= height;
	desc.MipLevels			= mipLevels;
	desc.ArraySize			= arraySize;
	desc.Format				= format;
	desc.SampleDesc.Count	= sampleCount;
	desc.SampleDesc.Quality = sampleQuality;
	desc.Usage				= usage;
	desc.BindFlags			= bindFlags;
	desc.CPUAccessFlags		= cpuAccessFlags;
	desc.MiscFlags			= miscFlags;

	ID3D11Texture2D* newTexture;
	HR(pDevice->CreateTexture2D(&desc, NULL, &newTexture));

	return newTexture;
}

#endif // _D3D11_API_H