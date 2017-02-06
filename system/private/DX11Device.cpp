#include "DX11Device.h"
#include "DemoCommon.h"

std::string DX11Device::GetAPIName()
{
	return "DirectX 11";
}

bool DX11Device::Create(const RenderInfo& info)
{
	LOG("Creating D3D11 context.");

	// check that a valid window pointer was passed
	if (!info.wndPointer)
	{
		LOG_ERROR("Could not create DirectX 11 context, window pointer was null");
		return false;
	}

	windowContext = *static_cast<HDC*>(info.wndPointer);
	windowHandle = WindowFromDC(windowContext);
	renderInfo = info;

	HRESULT result = S_OK;

	LOG("Setting swap chain description : [%ux%u]", renderInfo.resolutionX, renderInfo.resolutionY);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = renderInfo.resolutionX;
	swapChainDesc.BufferDesc.Height = renderInfo.resolutionY;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = windowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// create D3D device / swap chain
	LOG("Creating D3D11 Device and SwapChain.");

#ifdef _DEBUG
	uint32 deviceFlags = D3D11_CREATE_DEVICE_DEBUG;
	LOG("Setting device debug flag.");
#else
	uint32 deviceFlags = 0;
#endif

	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, nullptr, 0,
										   D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice,
										   nullptr, &pDeviceContext);

	if (FAILED(result))
	{
		LOG_DX_ERROR("Failed creating DX11 device and swap chain", result);
		return false;
	}

	IDXGIDevice* pDXGIDevice;
	HR(pDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice));

	IDXGIAdapter* pDXGIAdapter;
	HR(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter));

	IDXGIFactory* pIDXGIFactory;
	HR(pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory));

	HR(pIDXGIFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_WINDOW_CHANGES));

	// get the swap chain's back buffer
	LOG("Getting swapchain backbuffer.");

	ID3D11Texture2D* pBackBuffer = nullptr;
	result = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));

	if (FAILED(result))
	{
		LOG_DX_ERROR("Failed getting DX11 back buffer", result);
		return false;
	}

	// create render target view
	LOG("Creating render target view from back buffer.");

	result = pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
	pBackBuffer->Release();

	if (FAILED(result))
	{
		LOG_DX_ERROR("Failed creating DX11 render target view", result);
		return false;
	}

	if (!CreateDepthStencil(renderInfo.resolutionX, renderInfo.resolutionY))
	{
		return false;
	}

	// bind render target view
	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
	
	return true;
}

void DX11Device::Initialize()
{
	// Setup default Depth/Stencil state
	DepthStencilStateDesc defaultDepthStencilDesc;
	defaultDepthStencilState = CreateDepthStencilState(defaultDepthStencilDesc);
	SetDepthStencilState(defaultDepthStencilState);

	// Setup default Blend state

	// Setup default Rasterizer state
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_NONE;
	desc.ScissorEnable = true;
	desc.DepthClipEnable = false;
	pDevice->CreateRasterizerState(&desc, &pRasterizerState);

	pDeviceContext->RSSetState(pRasterizerState);

	// Setup the viewport
	SetViewport(0, 0, renderInfo.resolutionX, renderInfo.resolutionY);

	DSRect defaultRect = DSRect(0, 0, renderInfo.resolutionX, renderInfo.resolutionY);
	SetScissorRects(1, &defaultRect);

	// Set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DX11Device::Destroy()
{
	if (pDeviceContext) pDeviceContext->ClearState();

	if (pRenderTargetView) pRenderTargetView->Release();
	if (pSwapChain) pSwapChain->Release();
	if (pDeviceContext) pDeviceContext->Release();
	if (pDevice) pDevice->Release();

	// release all cache input layouts
	for (auto const &layout : inputLayouts)
	{
		layout.second->inputLayout->Release();
	}
}

void DX11Device::Clear()
{
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, (const float*)&clearColor);
	pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DX11Device::Present()
{
	pSwapChain->Present(syncInterval, 0);
}

void DX11Device::SetVSync(bool enabled)
{
	if (enabled)
	{
		syncInterval = 1;
	}
	else
	{
		syncInterval = 0;
	}
}

void DX11Device::SetShader(Shader* shader)
{
	D3D11Shader* dxShader = static_cast<D3D11Shader*>(shader);

	if (dxShader)
	{
		pDeviceContext->VSSetShader(dxShader->pVertexShader, nullptr, 0);
		pDeviceContext->PSSetShader(dxShader->pPixelShader, nullptr, 0);
	}
}

void DX11Device::SetTexture(Texture* texture, uint32 slot)
{
	D3D11Texture* dxTexture = static_cast<D3D11Texture*>(texture);

	if (dxTexture)
	{
		pDeviceContext->PSSetSamplers(0, 1, &dxTexture->pSampler);
		pDeviceContext->PSSetShaderResources(slot, 1, &dxTexture->pTexResourceView);
	}
}

void DX11Device::DrawMesh(Mesh* mesh)
{
	D3D11Mesh* dxMesh = static_cast<D3D11Mesh*>(mesh);

	if (dxMesh)
	{
		pDeviceContext->IASetInputLayout(dxMesh->pInputLayout->inputLayout);
		pDeviceContext->IASetVertexBuffers(0, 1, &dxMesh->vertexBuffer->pBuffer, &dxMesh->pInputLayout->stride, &dxMesh->offset);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pDeviceContext->Draw(dxMesh->vertexCount, dxMesh->offset);
	}
}

void DX11Device::DrawMeshIndexed(Mesh* mesh, uint32 elementCount, uint32 vertexOffset, uint16 indexOffset)
{
	D3D11Mesh* dxMesh = static_cast<D3D11Mesh*>(mesh);

	if (dxMesh)
	{
		if (elementCount == 0)
		{
			elementCount = dxMesh->indexCount;
		}

		pDeviceContext->IASetInputLayout(dxMesh->pInputLayout->inputLayout);
		pDeviceContext->IASetVertexBuffers(0, 1, &dxMesh->vertexBuffer->pBuffer, &dxMesh->pInputLayout->stride, &dxMesh->offset);
		pDeviceContext->IASetIndexBuffer(dxMesh->indexBuffer->pBuffer, DXGI_FORMAT_R16_UINT, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pDeviceContext->DrawIndexed(elementCount, indexOffset, vertexOffset);
	}
}

Mesh* DX11Device::CreateMesh(const MeshData &meshData, VertexAttributes vertexAttributeFlags, BufferUsage usage)
{
	// Get input layout for mesh
	CachedInputLayout* pLayout = GetInputLayout(vertexAttributeFlags);

	// Create vertex and index buffers
	DX11Buffer* vertexBuffer = CreateBuffer(meshData.vertexData,
											meshData.vertexCount * pLayout->stride,
											BufferTarget::Vertex, usage);

	DX11Buffer* indexBuffer = CreateBuffer(meshData.indexData,
										   meshData.indexCount * sizeof(uint16),
										   BufferTarget::Index, usage);

	return new D3D11Mesh(vertexBuffer, indexBuffer, meshData.vertexCount, meshData.indexCount, pLayout);
}

Mesh* DX11Device::CreateMesh(const MeshDataList &meshData, VertexAttributes vertexAttributeFlags, BufferUsage usage)
{
	// get input layout for mesh
	CachedInputLayout* pLayout = GetInputLayout(vertexAttributeFlags);

	// Create vertex and index buffers
	DX11Buffer* vertexBuffer = CreateBuffer(meshData.vertices, meshData.dataCount,
											meshData.vertexCount * pLayout->stride,
											BufferTarget::Vertex, usage);

	DX11Buffer* indexBuffer = CreateBuffer(meshData.indices, meshData.dataCount,
										   meshData.indexCount * sizeof(uint16),
										   BufferTarget::Index, usage);

	return new D3D11Mesh(vertexBuffer, indexBuffer, meshData.vertexCount, meshData.indexCount, pLayout);
}

void DX11Device::UpdateMesh(Mesh* mesh, const MeshData &meshData)
{
	D3D11Mesh* dxMesh = static_cast<D3D11Mesh*>(mesh);

	UpdateBuffer(dxMesh->vertexBuffer, meshData.vertexData, meshData.vertexCount * dxMesh->pInputLayout->stride);
	UpdateBuffer(dxMesh->indexBuffer, meshData.indexData, meshData.indexCount * sizeof(uint16));

	dxMesh->vertexCount = meshData.vertexCount;
	dxMesh->indexCount = meshData.indexCount;
}

void DX11Device::UpdateMesh(Mesh* mesh, const MeshDataList &meshData)
{
	D3D11Mesh* dxMesh = static_cast<D3D11Mesh*>(mesh);

	UpdateBuffer(dxMesh->vertexBuffer, meshData.vertices, meshData.dataCount, meshData.vertexCount * dxMesh->pInputLayout->stride);
	UpdateBuffer(dxMesh->indexBuffer, meshData.indices, meshData.dataCount, meshData.indexCount * sizeof(uint16));

	dxMesh->vertexCount = meshData.vertexCount;
	dxMesh->indexCount = meshData.indexCount;
}

void DX11Device::ReleaseMesh(Mesh* mesh)
{
	D3D11Mesh* dxMesh = static_cast<D3D11Mesh*>(mesh);

	if (dxMesh == nullptr)
		return;

	ReleaseBuffer(dxMesh->vertexBuffer);
	ReleaseBuffer(dxMesh->indexBuffer);

	delete dxMesh;
}

Shader* DX11Device::CreateShader(const std::string &name)
{
	HRESULT compileResult;
	D3D11Shader* newShader = new D3D11Shader();

	std::string fileName = DX11_SHADER_LOC + name + ".fx";
	std::wstring wfileName(fileName.begin(), fileName.end());

	// compile vertex shader
	ID3DBlob* pVSBlob = nullptr;
	compileResult = CompileShaderFromFile(wfileName, "VS", "vs_4_0", &pVSBlob);

	if (FAILED(compileResult))
	{
		// Make Default shader and return
		LOG_DX_ERROR("Failed compiling vertex shader", compileResult);
		return nullptr;
	}

	// create vertex shader
	HR(pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &newShader->pVertexShader));

	// compile pixel shader
	ID3DBlob* pPSBlob = nullptr;
	compileResult = CompileShaderFromFile(wfileName, "PS", "ps_4_0", &pPSBlob);

	if (FAILED(compileResult))
	{
		LOG_DX_ERROR("Failed compiling pixel shader", compileResult);
		return nullptr;
	}

	// create pixel shader
	HR(pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &newShader->pPixelShader));

	return newShader;
}

void DX11Device::ReleaseShader(Shader* shader)
{
	D3D11Shader* dxShader = static_cast<D3D11Shader*>(shader);

	if (dxShader == nullptr)
		return;
	
	dxShader->pPixelShader->Release();
	dxShader->pVertexShader->Release();

	delete dxShader;
}

Texture* DX11Device::CreateTexture(uint8 *data, const TextureSettings &settings)
{
	D3D11Texture* newTexture = new D3D11Texture();

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	int32 mipCount = 0;

	newTexture->pTexture = CreateTexture2D_D3D(settings.width, settings.height, format,
												D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
												D3D11_RESOURCE_MISC_GENERATE_MIPS, mipCount);

	if (&newTexture->pTexture != nullptr)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		memset(&SRVDesc, 0, sizeof(SRVDesc));
		SRVDesc.Format = format;

		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		if (settings.mipMaps)
		{
			SRVDesc.Texture2D.MipLevels = mipCount <= 0 ? -1 : mipCount;
		}
		else
		{
			SRVDesc.Texture2D.MipLevels = 1;
		}

		HR(pDevice->CreateShaderResourceView(newTexture->pTexture, &SRVDesc, &newTexture->pTexResourceView));
	}
	else
	{
		LOG_ERROR("Failed creating Texture2D");
		return nullptr;
	}

	uint32 bpp = 32;
	uint32 rowBytes = (settings.width * bpp + 7) / 8;
	uint32 numRows = settings.height;
	uint32 numBytes = rowBytes * settings.height;

	pDeviceContext->UpdateSubresource(newTexture->pTexture, 0, nullptr, data, rowBytes, numBytes);

	if (settings.mipMaps)
	{
		pDeviceContext->GenerateMips(newTexture->pTexResourceView);
	}

	if (newTexture->pTexture == nullptr)
	{
		newTexture->pTexture->Release();
	}

	// create sampler state
	D3D11_TEXTURE_ADDRESS_MODE wrapMode = DX11WrapMode[static_cast<int32>(settings.wrapMode)];

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.AddressU = wrapMode;
	samplerDesc.AddressV = wrapMode;
	samplerDesc.AddressW = wrapMode;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (settings.anisoLevel <= 0.0f)
	{
		switch (settings.filterMode)
		{
			case TextureFilterMode::Point:
				samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			break;
			case TextureFilterMode::Bilinear:
				samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
				break;
			case TextureFilterMode::Trilinear:
				samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		}
	}
	else
	{
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.MaxAnisotropy = static_cast<uint32>(settings.anisoLevel);
	}

	HR(pDevice->CreateSamplerState(&samplerDesc, &newTexture->pSampler));

	return newTexture;
}

void DX11Device::ReleaseTexture(Texture* pTexture)
{
	D3D11Texture* dxTexture = static_cast<D3D11Texture*>(pTexture);

	if (dxTexture == nullptr)
		return;

	dxTexture->pTexture->Release();
	dxTexture->pTexResourceView->Release();
	dxTexture->pSampler->Release();

	delete dxTexture;
}

void DX11Device::SetUniformBuffer(uint32 slot, Buffer* buffer, ShaderStage stage)
{
	DX11Buffer *dxBuffer = static_cast<DX11Buffer*>(buffer);

	DS_ASSERT(dxBuffer); // dxBuffer must not be null

	if (CheckFlags(stage, ShaderStage::Vertex))
	{
		pDeviceContext->VSSetConstantBuffers(slot, 1, &dxBuffer->pBuffer);
	}

	if (CheckFlags(stage, ShaderStage::Pixel))
	{
		pDeviceContext->PSSetConstantBuffers(slot, 1, &dxBuffer->pBuffer);
	}
}

DX11Buffer* DX11Device::CreateBuffer(const void* data, uint32 size, BufferTarget target, BufferUsage usage)
{
	// setup properties of the new buffer to be created
	D3D11_BUFFER_DESC desc;
	SetBufferDesc(desc, size, target, usage);

	ID3D11Buffer* dxBuffer = NULL;
	if (data != nullptr)
	{
		// set buffer data
		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = data;

		// create the new buffer with initial data
		HR(pDevice->CreateBuffer(&desc, &resourceData, &dxBuffer));
	}
	else
	{
		// if no data was passed create uninitialized buffer with the given size
		HR(pDevice->CreateBuffer(&desc, NULL, &dxBuffer));
	}

	return new DX11Buffer(desc.Usage, dxBuffer, desc, size);
}

DX11Buffer* DX11Device::CreateBuffer(const std::vector<BufferData> &data, uint32 dataCount, uint32 bufferSize, BufferTarget target, BufferUsage usage)
{
	D3D11_BUFFER_DESC desc;
	SetBufferDesc(desc, bufferSize, target, usage);

	ID3D11Buffer* dxBuffer = NULL;

	if (desc.Usage == D3D11_USAGE_IMMUTABLE || dataCount == 1)
	{
		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = data[0].pData;

		HR(pDevice->CreateBuffer(&desc, &resourceData, &dxBuffer));
	}
	else
	{
		HR(pDevice->CreateBuffer(&desc, NULL, &dxBuffer));

		D3D11_MAPPED_SUBRESOURCE elementResource;
		HR(pDeviceContext->Map(dxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &elementResource));

		uint8* destination = (uint8*)(elementResource.pData);
		for (uint32 md = 0; md < dataCount; md++)
		{
			memcpy(destination, data[md].pData, data[md].sizeBytes);
			destination += data[md].sizeBytes;
		}

		pDeviceContext->Unmap(dxBuffer, 0);
	}

	return new DX11Buffer(desc.Usage, dxBuffer, desc, bufferSize);
}

void DX11Device::UpdateBuffer(Buffer* buffer, const void* data, uint32 size)
{
	DX11Buffer* dxBuffer = static_cast<DX11Buffer*>(buffer);

	DS_ASSERT(dxBuffer);									// dxBuffer must not be null
	DS_ASSERT(dxBuffer->dxUsage != D3D11_USAGE_IMMUTABLE);	// Immutable buffers cannot be modified

	// recreate buffer if it is not big enough to store the new data
	ExpandBuffer(dxBuffer, size);

	// copy new data to buffer
	D3D11_MAPPED_SUBRESOURCE elementResource;
	HR(pDeviceContext->Map(dxBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &elementResource));

	memcpy(elementResource.pData, data, size);

	pDeviceContext->Unmap(dxBuffer->pBuffer, 0);
}

void DX11Device::UpdateBuffer(Buffer* buffer, const std::vector<BufferData> &data, uint32 dataCount, uint32 bufferSize)
{
	DX11Buffer *dxBuffer = static_cast<DX11Buffer*>(buffer);

	DS_ASSERT(dxBuffer);									// dxBuffer must not be null
	DS_ASSERT(dxBuffer->dxUsage != D3D11_USAGE_IMMUTABLE);	// Immutable buffers cannot be modified

	// recreate buffer if it is not big enough to store the new data
	ExpandBuffer(dxBuffer, bufferSize);

	D3D11_MAPPED_SUBRESOURCE elementResource;
	HR(pDeviceContext->Map(dxBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &elementResource));

	// copy data from each BufferData element in to the DX11 buffer contiguously
	uint8* destination = static_cast<uint8*>(elementResource.pData);
	for (uint32 md = 0; md < dataCount; md++)
	{
		memcpy(destination, data[md].pData, data[md].sizeBytes);
		destination += data[md].sizeBytes; // increment the position we write data to in the buffer
	}

	pDeviceContext->Unmap(dxBuffer->pBuffer, 0);
}

void DX11Device::ReleaseBuffer(Buffer* buffer)
{
	DX11Buffer *dxBuffer = static_cast<DX11Buffer*>(buffer);

	if (dxBuffer)
	{
		dxBuffer->pBuffer->Release();
		delete buffer;
	}
}

void DX11Device::SetScissorRects(uint32 numRects, const DSRect* pRects)
{
	if (!pRects)
		return;

	D3D11_RECT rects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];

	for (uint32 r = 0; r < numRects; r++)
	{
		rects[r].left = pRects[r].left;
		rects[r].top = pRects[r].top;
		rects[r].right = pRects[r].right;
		rects[r].bottom = pRects[r].bottom;
	}

	pDeviceContext->RSSetScissorRects(numRects, rects);
}

void DX11Device::GetScissorRects(uint32* pNumRects, DSRect* pRects)
{
	UINT rectCount = 1;
	D3D11_RECT rect[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	pDeviceContext->RSGetScissorRects(&rectCount, rect);

	if (pNumRects)
		*pNumRects = rectCount;

	if (pRects)
	{
		for (uint32 r = 0; r < rectCount; r++)
		{
			pRects[r].left	 = rect[r].left;
			pRects[r].top	 = rect[r].top;
			pRects[r].right	 = rect[r].right;
			pRects[r].bottom = rect[r].bottom;
		}
	}
}

BlendState* DX11Device::CreateBlendState(BlendProperties properties)
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;

	blendDesc.RenderTarget[0].BlendEnable = properties.enabled;
	blendDesc.RenderTarget[0].SrcBlend = GET_D3D11_BLEND(properties.srcBlend);
	blendDesc.RenderTarget[0].DestBlend = GET_D3D11_BLEND(properties.dstBlend);
	blendDesc.RenderTarget[0].BlendOp = GET_D3D11_BLEND_OP(properties.blendOp);
	blendDesc.RenderTarget[0].SrcBlendAlpha = GET_D3D11_BLEND(properties.srcBlendAlpha);
	blendDesc.RenderTarget[0].DestBlendAlpha = GET_D3D11_BLEND(properties.dstBlendAlpha);
	blendDesc.RenderTarget[0].BlendOpAlpha = GET_D3D11_BLEND_OP(properties.blendOpAlpha);
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	DX11BlendState* dxBlendState = new DX11BlendState();
	pDevice->CreateBlendState(&blendDesc, &dxBlendState->blendState);

	return dxBlendState;
}

void DX11Device::SetBlendState(BlendState* state)
{
	DX11BlendState* dxBlendState = static_cast<DX11BlendState*>(state);

	if (dxBlendState == nullptr)
		return;

	const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
	pDeviceContext->OMSetBlendState(dxBlendState->blendState, blend_factor, 0xffffffff);
}

DepthStencilStateD3D11* DX11Device::CreateDepthStencilState(DepthStencilStateDesc& desc)
{
	// Depth Stencil State
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = desc.depthEnabled;
	dsDesc.DepthWriteMask = desc.depthWriteEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = GET_D3D11_COMPARISON(desc.depthFunc);

	// Stencil test parameters
	dsDesc.StencilEnable = desc.stencilEnabled;
	dsDesc.StencilReadMask = desc.stencilRead ? 0xFF : 0x00;
	dsDesc.StencilWriteMask = desc.stencilWrite ? 0xFF : 0x00;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = GET_D3D11_STENCIL_OP(desc.frontFace.stencilfailOp);
	dsDesc.FrontFace.StencilDepthFailOp = GET_D3D11_STENCIL_OP(desc.frontFace.depthFailOp);
	dsDesc.FrontFace.StencilPassOp = GET_D3D11_STENCIL_OP(desc.frontFace.stencilPassOp);
	dsDesc.FrontFace.StencilFunc = GET_D3D11_COMPARISON(desc.frontFace.stencilFunc);

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = GET_D3D11_STENCIL_OP(desc.backFace.stencilfailOp);
	dsDesc.BackFace.StencilDepthFailOp = GET_D3D11_STENCIL_OP(desc.backFace.depthFailOp);
	dsDesc.BackFace.StencilPassOp = GET_D3D11_STENCIL_OP(desc.backFace.stencilPassOp);
	dsDesc.BackFace.StencilFunc = GET_D3D11_COMPARISON(desc.backFace.stencilFunc);

	ID3D11DepthStencilState* newDepthStencilState;
	HR(pDevice->CreateDepthStencilState(&dsDesc, &newDepthStencilState));

	return new DepthStencilStateD3D11(newDepthStencilState);
}

DepthStencilState* DX11Device::GetCurrentDepthStencilState()
{
	return curDepthStencilState;
}

void DX11Device::SetDepthStencilState(DepthStencilState* state)
{
	DepthStencilStateD3D11* dxDepthStencilState = static_cast<DepthStencilStateD3D11*>(state);
	curDepthStencilState = dxDepthStencilState;

	if (dxDepthStencilState == nullptr)
		return;

	pDeviceContext->OMSetDepthStencilState(dxDepthStencilState->pDepthStencilState, 1);
}

bool DX11Device::CreateDepthStencil(uint32 width, uint32 height)
{
	LOG("Creating Depth/Stencil : [%ux%u]", width, height);

	HRESULT result;

	pDepthStencil = CreateTexture2D_D3D(width, height, DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
										D3D11_USAGE_DEFAULT, D3D11_BIND_DEPTH_STENCIL);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	// Create the depth stencil view
	result = pDevice->CreateDepthStencilView(pDepthStencil, &descDSV, &pDepthStencilView);

	if (FAILED(result))
	{
		LOG_DX_ERROR("Failed creating DX11 depth stencil view", result);
		return false;
	}

	return true;
}

void DX11Device::SetClearColor(const vec4 &color)
{
	clearColor = color;
}

void DX11Device::SetViewport(int32 x, int32 y, int32 width, int32 height)
{
	// Setup the viewport
	D3D11_VIEWPORT viewPort;
	ZeroMemory(&viewPort, sizeof(D3D11_VIEWPORT));

	viewPort.TopLeftX = static_cast<FLOAT>(x);
	viewPort.TopLeftY = static_cast<FLOAT>(y);
	viewPort.Width	  = static_cast<FLOAT>(width);
	viewPort.Height	  = static_cast<FLOAT>(height);
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	pDeviceContext->RSSetViewports(1, &viewPort);
}

// TODO : change to OnDisplayModeChanged(width, height, refreshRate, fullscreen);
//					OnDisplayModeChanged(DisplayMode newMode);
void DX11Device::OnResolutionChanged(uint32 width, uint32 height)
{
	renderInfo.resolutionX = width;
	renderInfo.resolutionY = height;

	pDeviceContext->OMSetRenderTargets(0, 0, 0);
	pRenderTargetView->Release();
	pDepthStencilView->Release();
	pDepthStencil->Release();

	// Resize swap chain
	HR(pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	// Get the swap chain's back buffer
	ID3D11Texture2D* pBackBuffer = nullptr;
	HR(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&pBackBuffer)));

	// Create render target view
	HR(pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView));

	pBackBuffer->Release();

	// Recreate depth stencil 
	CreateDepthStencil(renderInfo.resolutionX, renderInfo.resolutionY);

	// Bind render target view
	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);

	// Setup the viewport
	SetViewport(0, 0, renderInfo.resolutionX, renderInfo.resolutionY);

	DSRect defaultRect = DSRect(0, 0, renderInfo.resolutionX, renderInfo.resolutionY);
	SetScissorRects(1, &defaultRect);
}

HRESULT DX11Device::CompileShaderFromFile(const std::wstring &fileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT result = S_OK;

#ifdef _DEBUG
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	result = D3DCompileFromFile(fileName.c_str(), nullptr, nullptr, szEntryPoint, szShaderModel, shaderFlags, 0, ppBlobOut, &pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob)
		{
			LOG_ERROR("%s", reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return result;
	}

	if (pErrorBlob)
	{
		pErrorBlob->Release();
	}

	return S_OK;
}

CachedInputLayout* DX11Device::GetInputLayout(VertexAttributes vertexAttributeFlags)
{
	DS_ASSERT(vertexAttributeFlags != VertexAttributes::None);	// must have at least 1 vertex attribute

	HRESULT result;

	// check if an input layout has already been created for the given vertex attribute configuration
	if (inputLayouts.find(ToIntegral(vertexAttributeFlags)) != inputLayouts.end())
	{
		return inputLayouts[ToIntegral(vertexAttributeFlags)];
	}

	// add each flagged vertex attribute to layout list
	uint32 stride = 0;
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout;

	for (uint32 mask = ToIntegral(vertexAttributeFlags); mask; mask &= mask - 1)
	{
		uint32 index = Bit::LeastSignifcantBit(mask);
		const AttributeProperties &properties = attributeProperties[index];
		uint32 typeIndex = static_cast<uint32>(properties.type);

		DXGI_FORMAT format = GetDXGIFormat(properties.type, properties.components, properties.normalized);
		layout.push_back({ D3D11InputElementName[index], 0, format, 0, stride, D3D11_INPUT_PER_VERTEX_DATA, 0 });

		stride += properties.components * properties.typeSizeBytes;
	}

	// get dummy shader for given input layout
	ID3DBlob* pVSBlob = nullptr;
	result = GetDummyLayoutShader(vertexAttributeFlags, "VS", "vs_4_0", &pVSBlob);

	if (FAILED(result))
	{
		LOG_DX_ERROR("Failed compiling dummy input layout shader", result);
		return nullptr;
	}

	// Create the input layout
	ID3D11InputLayout* pVertexLayout = nullptr;
	result = pDevice->CreateInputLayout(&layout[0], layout.size(), pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &pVertexLayout);

	pVSBlob->Release();
	if (FAILED(result))
	{
		LOG_DX_ERROR("Failed creating input layout", result);
	}

	CachedInputLayout* newLayout = new CachedInputLayout(pVertexLayout, stride);
	inputLayouts[ToIntegral(vertexAttributeFlags)] = newLayout;

	return newLayout;
}

HRESULT DX11Device::GetDummyLayoutShader(VertexAttributes vertexAttributeFlags, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	std::string dummySource ="struct VS_INPUT {";

	if (CheckFlags(vertexAttributeFlags, VertexAttributes::Position))
	{
		dummySource += "float4 Pos : POSITION;";
	}

	if (CheckFlags(vertexAttributeFlags, VertexAttributes::UIPosition))
	{
		dummySource += "float2 Pos : POSITION;";
	}

	if (CheckFlags(vertexAttributeFlags, VertexAttributes::TexCoord))
	{
		dummySource += "float2 Tex : TEXCOORD0;";
	}

	if (CheckFlags(vertexAttributeFlags, VertexAttributes::Color32))
	{
		dummySource += "float4 Col : COLOR0;";
	}

	dummySource += "};";

	dummySource += "struct PS_INPUT { float4 Pos : SV_POSITION; };";
	dummySource += "PS_INPUT VS(VS_INPUT input) { PS_INPUT output; output.Pos.xyzw = 0.0f;return output; }";

	HRESULT result = S_OK;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	ID3DBlob* pErrorBlob = nullptr;
	result = D3DCompile(dummySource.c_str(), dummySource.size(), nullptr, nullptr, nullptr,
						szEntryPoint, szShaderModel, shaderFlags, 0, ppBlobOut, &pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob)
		{
			LOG_ERROR("%s", reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return result;
	}

	if (pErrorBlob)
	{
		pErrorBlob->Release();
	}

	return S_OK;
}