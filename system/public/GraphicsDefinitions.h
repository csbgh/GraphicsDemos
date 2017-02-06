#ifndef _GRAPHICS_DEFINITIONS_H
#define _GRAPHICS_DEFINITIONS_H

#include "DemoCommon.h"
#include "utility/ResourceContainer.h"

// create handle objects for all resource types
DEFINE_RESOURCE_HANDLE(BufferHandle);
DEFINE_RESOURCE_HANDLE(ShaderHandle);
DEFINE_RESOURCE_HANDLE(TextureHandle);
DEFINE_RESOURCE_HANDLE(MeshHandle);

namespace World
{
	static const vec3 Right(1, 0, 0);
	static const vec3 Left(-1, 0, 0);
	static const vec3 Forward(0, 1, 0);
	static const vec3 Back(0, -1, 0);
	static const vec3 Up(0, 0, 1);
	static const vec3 Down(0, 0, -1);
}

struct RenderInfo
{
	RenderInfo()
	{
		resolutionX = 1366;
		resolutionY = 768;
		colorBits	= 32;
		depthBits	= 32;
		stencilBits = 8;
		wndPointer	= nullptr;
	}

	uint32 resolutionX, resolutionY;
	uint32 colorBits;
	uint32 depthBits;
	uint32 stencilBits;
	void* wndPointer;
};

// ==============================================
// D3D11 Buffers
// ==============================================

// Specifies how a buffer resource is bound
enum class BufferTarget
{
	Vertex	= 0,	// bind as vertex buffer
	Index	= 1,	// bind as index buffer
	Uniform = 2		// bind as uniform buffer object
};

// Specifies how the buffer will be read/written to, the API can make performance tradeoffs based on this
enum class BufferUsage
{
	Static	= 0,	// Buffer data is set once when the buffer is created, and never modified after that
	Dynamic = 1,	// Buffer data is modified infrequently
	Stream	= 2		// Buffer data modified very often, for example every frame
};

struct BufferData
{
	BufferData(const void* pData = nullptr,
			   uint32 sizeBytes = 0) :
		pData(pData),
		sizeBytes(sizeBytes)
	{}

	const void* pData;
	uint32 sizeBytes;
};

class Buffer { };

// ==============================================

// ==============================================
// Vertex Attributes
// ==============================================

enum class BaseType : int32
{
	Int8	= 0,
	UInt8	= 1,
	Int16	= 2,
	UInt16	= 3,
	Int32	= 4,
	UInt32	= 5,
	Float	= 6,
	Double	= 7
};

static const uint32 BaseTypeSize[] = { 1, 1, 2, 2, 4, 4, 4, 8 };

#define DS_TYPE_SIZE(_type) BaseTypeSize[static_cast<int32>(_type)]

enum class VertexAttributes
{
	None		= 0,
	Position	= 1,
	UIPosition	= 2,
	TexCoord	= 4,
	Color32		= 8,
	Normal		= 16
};
ENUM_FLAGS(VertexAttributes)

class AttributeProperties
{
public:

	AttributeProperties(BaseType type, uint32 components, bool normalized) :
		type(type),
		components(components),
		normalized(normalized)
	{
		typeSizeBytes = DS_TYPE_SIZE(type);
	}

	BaseType type;
	uint32 components;
	bool normalized;

	uint32 typeSizeBytes;
};

static const AttributeProperties attributeProperties[] =
{
	AttributeProperties(BaseType::Float, 3, false),	// ATTR_POSITION
	AttributeProperties(BaseType::Float, 2, false),	// ATTR_UI_POSITION
	AttributeProperties(BaseType::Float, 2, false),	// ATTR_TEXCOORD
	AttributeProperties(BaseType::UInt8, 4, true),	// ATTR_COLOR32
	AttributeProperties(BaseType::Float, 3, false)	// ATTR_NORMAL
};

// returns the total size of the vertex attributes active in attributeFlags
static uint32 GetAttributeMaskSize(VertexAttributes attributeFlags)
{
	uint32 sizeBytes = 0;

	// find each set bit in the mask, get the index of the bit and then clear it, continue until all bits cleared
	for (uint32 mask = ToIntegral(attributeFlags); mask; mask &= mask - 1)
	{
		uint32 index = Bit::LeastSignifcantBit(mask);
		sizeBytes += attributeProperties[index].components * attributeProperties[index].typeSizeBytes;
	}

	return sizeBytes;
}

// ==============================================
// Mesh
// ==============================================

struct MeshData
{
	MeshData(void* vertexData, uint32 vertexCount, void* indexData, uint32 indexCount) :
		vertexData(vertexData),
		vertexCount(vertexCount),
		indexData(indexData),
		indexCount(indexCount)
	{}

	void* vertexData;
	uint32 vertexCount;

	void* indexData;
	uint32 indexCount;
};

class MeshDataList
{
public:

	MeshDataList(uint32 initialSize)
	{
		vertices = std::vector<BufferData>(initialSize);
		indices = std::vector<BufferData>(initialSize);
	}

	void SetVertices(uint32 index, const void* data, uint32 sizeBytes)
	{
		vertices[index].pData = data;
		vertices[index].sizeBytes = sizeBytes;
	}

	void SetIndices(uint32 index, const void* data, uint32 sizeBytes)
	{
		indices[index].pData = data;
		indices[index].sizeBytes = sizeBytes;
	}

	std::vector<BufferData> vertices;
	std::vector<BufferData> indices;
	uint32 dataCount;

	uint32 vertexCount;
	uint32 indexCount;
};

class Mesh
{

};

// ==============================================

// ==============================================
// Textures
// ==============================================

enum class TextureWrapMode
{
	Repeat,
	Clamp
};

enum class TextureFilterMode
{
	Point,
	Bilinear,
	Trilinear
};

enum class TextureFormat
{
	RGB,
	RGBA,
	BGR,
	BGRA,
	DXT1,
	DXT5
};

struct TextureSettings
{
	TextureSettings(uint32 width = 32,
					uint32 height = 32,
					TextureFormat format = TextureFormat::RGBA,
					TextureWrapMode wrapMode = TextureWrapMode::Repeat,
					TextureFilterMode filterMode = TextureFilterMode::Trilinear,
					float anisoLevel = 16.0f,
					bool mipMaps = true,
					bool editable = false) :
		width(width),
		height(height),
		format(format),
		wrapMode(wrapMode),
		filterMode(filterMode),
		anisoLevel(anisoLevel),
		mipMaps(mipMaps),
		editable(editable)
	{}

	uint32 width;
	uint32 height;
	TextureFormat format;
	TextureFilterMode filterMode;
	float anisoLevel;
	bool mipMaps;
	bool editable;
	TextureWrapMode wrapMode;
};

class Texture {};

// ==============================================

// ==============================================
// Shaders
// ==============================================

class Shader {};

enum class ShaderStage : int32
{
	None	= 0,
	Vertex	= 1,
	Pixel	= 2
};
ENUM_FLAGS(ShaderStage)

// ==============================================

// ==============================================
// Blend State
// ==============================================

enum class BlendOperation : int32
{
	Add					= 0,	// GL_FUNC_ADD
	Subtract			= 1,	// GL_FUNC_SUBTRACT
	ReverseSubtract		= 2,	// GL_FUNC_REVERSE_SUBTRACT
	Min					= 3,	// GL_FUNC_MIN
	Max					= 4		// GL_FUNC_MAX
};

enum class BlendFactor : int32
{
	Zero				= 0,	// GL_ZERO
	One					= 1,	// GL_ONE
	SrcColor			= 2,	// GL_SRC_COLOR
	InvSrcColor			= 3,	// GL_ONE_MINUS_SRC_COLOR
	SrcAlpha			= 4,	// GL_SRC_ALPHA
	InvSrcAlpha			= 5,	// GL_ONE_MINUS_SRC_ALPHA
	DestAlpha			= 6,	// GL_DST_ALPHA
	InvDestAlpha		= 7,	// GL_ONE_MINUS_DST_ALPHA
	DestColor			= 8,	// GL_DST_COLOR
	InvDestColor		= 9,	// GL_ONE_MINUS_DST_COLOR
	SrcAlphaSat			= 10,	// GL_SRC_ALPHA_SATURATE
	ConstantColor		= 11,	// GL_CONSTANT_COLOR
	InvConstantColor	= 12,	// GL_ONE_MINUS_CONSTANT_COLOR
};

enum class ColorMask : int32
{
	None	= 0,
	Red		= 1,
	Green	= 2,
	Blue	= 4,
	Alpha	= 8,
	All		= (Red | Green | Blue | Alpha)
};
ENUM_FLAGS(ColorMask)

struct BlendProperties
{
	bool			enabled;
	BlendFactor		srcBlend;
	BlendFactor		dstBlend;
	BlendOperation	blendOp;
	BlendFactor		srcBlendAlpha;
	BlendFactor		dstBlendAlpha;
	BlendOperation	blendOpAlpha;
	uint8			colorMask;
};

class BlendState
{
public:

	BlendProperties properties;
};

// ==============================================

// ==============================================
// Depth/Stencil State
// ==============================================

enum class ComparisonFunc
{
	Never			= 0,
	Less			= 1,
	Equal			= 2,
	LessEqual		= 3,
	Greater			= 4,
	NotEqual		= 5,
	GreaterEqual	= 6,
	Always			= 7
};

enum class StencilOp
{
	Keep	= 0,
	Zero	= 1,
	Replace = 2,
	IncrSat = 3,
	DecrSat = 4,
	Invert	= 5,
	Incr	= 6,
	Decr	= 7
};

struct DepthStencilOp
{
	StencilOp stencilfailOp;
	StencilOp stencilPassOp;
	StencilOp depthFailOp;
	ComparisonFunc stencilFunc;
};

struct DepthStencilStateDesc
{
	DepthStencilStateDesc()
	{
		depthEnabled = true;
		depthWriteEnabled = true;
		depthFunc = ComparisonFunc::Less;

		stencilEnabled = true;
		stencilRead = 0xFF;
		stencilWrite = 0xFF;

		frontFace.stencilfailOp = StencilOp::Keep;
		frontFace.stencilPassOp = StencilOp::Keep;
		frontFace.depthFailOp	= StencilOp::Incr;
		frontFace.stencilFunc	= ComparisonFunc::Always;

		backFace.stencilfailOp	= StencilOp::Keep;
		backFace.stencilPassOp	= StencilOp::Keep;
		backFace.depthFailOp	= StencilOp::Decr;
		backFace.stencilFunc	= ComparisonFunc::Always;
	}

	bool depthEnabled;
	bool depthWriteEnabled;
	ComparisonFunc depthFunc;

	bool stencilEnabled;
	uint8 stencilRead;
	uint8 stencilWrite;

	DepthStencilOp frontFace;
	DepthStencilOp backFace;

	uint32 stateKey;

	uint32 CalculateKey()
	{
		stateKey = 0;



		return stateKey;
	}
};

class DepthStencilState {};

// ==============================================

// ==============================================
// Uniform Buffers
// ==============================================

class UniformBuffer
{

};

// ==============================================

#endif // _GRAPHICS_DEFINITIONS_H