#ifndef _GL3_API_H
#define _GL3_API_H

#include "IGraphicsDevice.h"
#include "GL3Definitions.h"

class GL3Shader : public Shader
{
public:

	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint programID;

};

class MeshGL3 : public Mesh
{
public:

	MeshGL3(GLuint vertexArrayID, BufferGL3* vertexBuffer, BufferGL3* indexBuffer, uint32 vertexCount, uint32 indexCount, uint32 stride) :
		vertexArrayID(vertexArrayID),
		vertexBuffer(vertexBuffer),
		indexBuffer(indexBuffer),
		vertexCount(vertexCount),
		indexCount(indexCount),
		stride(stride)
	{}

	GLuint vertexArrayID;
	BufferGL3* vertexBuffer;
	BufferGL3* indexBuffer;

	uint32 vertexCount;
	uint32 indexCount;

	uint32 stride;
};

class GL3Texture : public Texture
{
public:

	GL3Texture(TextureSettings settings)
	{
		SetTextureFormat(settings.format);
		SetFilterMode(settings.filterMode, settings.mipMaps);
		glWrapMode = GL3WrapMode[static_cast<int32>(settings.wrapMode)];
	}

	void SetTextureFormat(TextureFormat format)
	{
		glType = GL_UNSIGNED_BYTE;

		switch (format)
		{
		case TextureFormat::RGB:
			glFormat = GL_RGB;
			glInternalFormat = GL_RGB8;
			break;
		case TextureFormat::RGBA:
			glFormat = GL_RGBA;
			glInternalFormat = GL_RGBA8;
			break;
		case TextureFormat::BGR:
			glFormat = GL_BGR;
			glInternalFormat = GL_RGB8;
			break;
		case TextureFormat::BGRA:
			glFormat = GL_BGRA;
			glInternalFormat = GL_RGBA8;
			break;
		default:
			glFormat = GL_RGB;
			glInternalFormat = GL_RGB8;
			break;
		}
	}

	void SetFilterMode(TextureFilterMode filterMode, bool mipMaps)
	{
		switch (filterMode)
		{
		case TextureFilterMode::Point:
			glMinFilter = mipMaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
			glMagFilter = GL_NEAREST;
			break;
		case TextureFilterMode::Bilinear:
			glMinFilter = mipMaps ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR;
			glMagFilter = GL_LINEAR;
			break;
		case TextureFilterMode::Trilinear:

			// can only use trilinear filtering with mip maps
			if (mipMaps)
				glMinFilter = GL_LINEAR_MIPMAP_LINEAR;
			else
				glMinFilter = GL_LINEAR;

			glMagFilter = GL_LINEAR;

			break;
		}
	}

	GLuint textureID;

	GLint glWrapMode;
	GLint glInternalFormat;
	GLenum glFormat;
	GLenum glType;
	GLint glMinFilter;
	GLint glMagFilter;

};

class GL3UniformBuffer : public UniformBuffer
{
public:

	GLuint glUniformBufferID;
	uint32 size;
};

class GL3Device : public IGraphicsDevice
{
public:

	std::string GetAPIName();

	bool Create(const RenderInfo& info);

	void Initialize();
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
	void DrawMeshIndexed(Mesh* mesh, uint32 elementCount, uint32 vertexOffset = 0, uint16 indexOffset = 0);

	// Mesh Resource Handling
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
	void SetUniformBuffer(uint32 slot, Buffer* buffer, ShaderStage stage);

	// Buffer Resource Handling
	BufferGL3* CreateBuffer(const void* data, uint32 size, BufferTarget target, BufferUsage usage);
	BufferGL3* CreateBuffer(const std::vector<BufferData> &data, uint32 dataCount, uint32 bufferSize, BufferTarget target, BufferUsage usage);

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
	DepthStencilStateGL3* CreateDepthStencilState(DepthStencilStateDesc& desc);

	DepthStencilState* GetCurrentDepthStencilState();

	void SetDepthStencilState(DepthStencilState* state);

private:

	GLuint CompileShaderObject(const std::string &fileName, GLenum shaderType);

	void SetVertexAttributes(VertexAttributes vertexAttributeFlags, uint32 stride);

	RenderInfo renderInfo;

	HDC windowContext;
	HGLRC glContextHandle;

	// States
	DepthStencilStateGL3* defaultDepthStencilState = nullptr;
	DepthStencilStateGL3* curDepthStencilState = nullptr;

};

#endif // _GL3_API_H