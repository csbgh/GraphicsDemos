#ifndef _I_GRAPHICS_DEVICE_H
#define _I_GRAPHICS_DEVICE_H

#include "GraphicsDefinitions.h"

#define ALLOW_UNIMPLEMENTED_API_CALLS

#if defined(_DEBUG) || defined(ALLOW_UNIMPLEMENTED_API_CALLS)

#define API_IMPLEMENT(_funcName, ...)														\
	{																						\
		Log::WriteError("RenderAPI does not implement called function " ## _funcName);		\
		return __VA_ARGS__;																	\
	}																						\

#else

#define API_IMPLEMENT(_funcName, ...) = 0

#endif

// IGraphicsDevice defines a simplistic abstraction of a Graphics API
class IGraphicsDevice
{
public:

	// Context
	virtual bool Create(const RenderInfo& info) = 0;

	virtual void Initialize() = 0;

	virtual void Destroy() = 0;

	virtual void Clear() = 0;

	virtual void Present() = 0;

	// Returns the name of the graphics API implemented by the class
	virtual std::string GetAPIName() = 0;

	// Drawing
	virtual void DrawMesh(Mesh* mesh) API_IMPLEMENT("DrawMesh");

	virtual void DrawMeshIndexed(Mesh* mesh, uint32 elementCount = 0, uint32 vertexOffset = 0, uint16 indexOffset = 0) API_IMPLEMENT("DrawMeshIndexed");

	// Render API State
	virtual void SetVSync(bool enabled) API_IMPLEMENT("SetVsync");

	virtual void SetShader(Shader* shader) API_IMPLEMENT("SetShader");

	virtual void SetTexture(Texture* texture, uint32 slot) API_IMPLEMENT("SetTexture");

	virtual void SetClearColor(const vec4 &color) API_IMPLEMENT("SetClearColor");

	virtual void SetViewport(int32 x, int32 y, int32 width, int32 height) API_IMPLEMENT("SetViewport");

	// Events
	virtual void OnResolutionChanged(uint32 width, uint32 height) API_IMPLEMENT("OnResolutionChanged");

	// Mesh Resource Handling
	virtual Mesh* CreateMesh(const MeshData &meshData, VertexAttributes vertexAttributeFlags, BufferUsage usage) API_IMPLEMENT("CreateMesh", nullptr);

	virtual Mesh* CreateMesh(const MeshDataList &meshData, VertexAttributes vertexAttributeFlags, BufferUsage usage) API_IMPLEMENT("CreateMesh", nullptr);

	virtual void UpdateMesh(Mesh* mesh, const MeshData &meshData) API_IMPLEMENT("UpdateMesh");

	virtual void UpdateMesh(Mesh* mesh, const MeshDataList &meshData) API_IMPLEMENT("UpdateMesh");

	virtual void ReleaseMesh(Mesh* mesh) = 0;

	// Shader Resource Handling
	virtual Shader* CreateShader(const std::string &name) API_IMPLEMENT("CreateShader", nullptr);

	virtual void ReleaseShader(Shader* shader) API_IMPLEMENT("UpdateMesh");

	// Texture Resource Handling
	virtual Texture* CreateTexture(uint8 *data, const TextureSettings &settings) API_IMPLEMENT("CreatureTexture", nullptr);

	virtual void ReleaseTexture(Texture* pTexture) API_IMPLEMENT("ReleaseTexture");

	// Uniform Buffer Resource Handling
	virtual void SetUniformBuffer(uint32 slot, Buffer* buffer, ShaderStage stage) API_IMPLEMENT("SetUniformBuffer");

	// Buffer Resource Handling
	virtual Buffer* CreateBuffer(const void* data, uint32 size, BufferTarget target, BufferUsage usage) API_IMPLEMENT("CreateBuffer", nullptr);

	virtual Buffer* CreateBuffer(const std::vector<BufferData> &data, uint32 bufferSize, BufferTarget target, BufferUsage usage) API_IMPLEMENT("CreateBuffer", nullptr);

	virtual void UpdateBuffer(Buffer* buffer, const void* data, uint32 size) API_IMPLEMENT("UpdateBuffer");

	virtual void UpdateBuffer(const std::vector<BufferData> &data, uint32 dataCount, uint32 bufferSize) API_IMPLEMENT("UpdateBuffer");

	virtual void ReleaseBuffer(Buffer* buffer) API_IMPLEMENT("ReleaseBuffer");

	// UpdateBufferRegion?

	// Scissor
	virtual void SetScissorRects(uint32 numRects, const DSRect* pRects) API_IMPLEMENT("SetScissorRects");

	virtual void GetScissorRects(uint32* pNumRects, DSRect* pRects) API_IMPLEMENT("GetScissorRects");

	// Blend State
	virtual BlendState* CreateBlendState(BlendProperties properties) API_IMPLEMENT("CreateBlendState", nullptr);

	virtual void SetBlendState(BlendState* state) API_IMPLEMENT("SetBlendState");

	// Depth/Stencil State
	virtual DepthStencilState* CreateDepthStencilState(DepthStencilStateDesc& desc) API_IMPLEMENT("CreateDepthStenciLState", nullptr);

	virtual DepthStencilState* GetCurrentDepthStencilState() API_IMPLEMENT("GetCurrentDepthStencilState", nullptr);

	virtual void SetDepthStencilState(DepthStencilState* state) API_IMPLEMENT("SetDepthStencilState");
};

#endif // _I_GRAPHICS_DEVICE_H