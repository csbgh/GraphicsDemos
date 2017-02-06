
#ifndef _DX11_DEFINITIONS_H
#define _DX11_DEFINITIONS_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <string>

#define DX11_SHADER_LOC "resources/shaders/D3D11/"

using namespace DirectX;

// maps TextureWrapMode to DirectX11 equivalent
static const D3D11_TEXTURE_ADDRESS_MODE DX11WrapMode[] = { D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_CLAMP };

// maps VertexAttribute values to there D3D Semantic Name
static const char* D3D11InputElementName[] =
{
	"POSITION",	// Position
	"POSITION",	// UIPosition
	"TEXCOORD",	// TexCoord
	"COLOR",	// Color32
	"NORMAL"	// Normal
};

static const DXGI_FORMAT const D3D11BaseTypeMap[8][4] =
{
	{ DXGI_FORMAT_R8_SINT, DXGI_FORMAT_R8G8_SINT, DXGI_FORMAT_R8G8B8A8_SINT, DXGI_FORMAT_R8G8B8A8_SINT},				// Int8
	{ DXGI_FORMAT_R8_UINT, DXGI_FORMAT_R8G8_UINT, DXGI_FORMAT_R8G8B8A8_UINT, DXGI_FORMAT_R8G8B8A8_UINT},				// UInt8

	{ DXGI_FORMAT_R16_SINT, DXGI_FORMAT_R16G16_SINT, DXGI_FORMAT_R16G16B16A16_SINT, DXGI_FORMAT_R16G16B16A16_SINT},		// Int16
	{ DXGI_FORMAT_R16_UINT, DXGI_FORMAT_R16G16_UINT, DXGI_FORMAT_R16G16B16A16_UINT, DXGI_FORMAT_R16G16B16A16_UINT},		// UInt16

	{ DXGI_FORMAT_R32_SINT, DXGI_FORMAT_R32G32_SINT, DXGI_FORMAT_R32G32B32_SINT, DXGI_FORMAT_R32G32B32A32_SINT},		// Int32
	{ DXGI_FORMAT_R32_UINT, DXGI_FORMAT_R32G32_UINT, DXGI_FORMAT_R32G32B32_UINT, DXGI_FORMAT_R32G32B32A32_UINT},		// UInt32

	{ DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT},	// Float
	{ DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT},	// Double
};

static const DXGI_FORMAT const D3D11NormalizedBaseTypeMap[4][4] =
{
	{ DXGI_FORMAT_R8_SNORM, DXGI_FORMAT_R8G8_SNORM, DXGI_FORMAT_R8G8B8A8_SNORM, DXGI_FORMAT_R8G8B8A8_SNORM },				// Int8
	{ DXGI_FORMAT_R8_UNORM, DXGI_FORMAT_R8G8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM },				// UInt8

	{ DXGI_FORMAT_R16_SNORM, DXGI_FORMAT_R16G16_SNORM, DXGI_FORMAT_R16G16B16A16_SNORM, DXGI_FORMAT_R16G16B16A16_SNORM },	// Int16
	{ DXGI_FORMAT_R16_UNORM, DXGI_FORMAT_R16G16_UNORM, DXGI_FORMAT_R16G16B16A16_SNORM, DXGI_FORMAT_R16G16B16A16_UINT },		// UInt16
};

// Get a DXGI format that the given layout will fit in to
static DXGI_FORMAT GetDXGIFormat(BaseType type, uint32 components, bool normalized)
{
	DS_ASSERT(components > 0 && components <= 4);	// must have at least 1 component and at most 4 components

	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

	uint32 typeIndex = static_cast<uint32>(type);
	uint32 componentIndex = components-1;

	// 8 and 16 bit int types do not support 3 components, produce warning 
	if (components == 3 && 
	    (type == BaseType::Int8 || type == BaseType::UInt8 ||
		type == BaseType::Int16 || type == BaseType::UInt16))
	{
		LOG_WARNING("GetDXGIFormat : No matching 3 component format for 8 and 16 bit int/uint types, returning 4 component format.");
	}

	if (!normalized)
	{
		format = D3D11BaseTypeMap[typeIndex][componentIndex];
	}
	else
	{
		if (type != BaseType::UInt32 && type != BaseType::Int32 &&
			type != BaseType::Float && type != BaseType::Double )
		{
			format = D3D11NormalizedBaseTypeMap[typeIndex][componentIndex];
		}
	}

	DS_ASSERT(format != DXGI_FORMAT_UNKNOWN);	// we should never get an unknown format

	return format;
}

// ==============================================
// Buffers
// ==============================================

// maps BufferTarget to DirectX11 equivalent
static const UINT DX11BindTargetMap[] = { D3D11_BIND_VERTEX_BUFFER, D3D11_BIND_INDEX_BUFFER, D3D11_BIND_CONSTANT_BUFFER };

// maps BufferUsage to DirectX11 equivalent
static const D3D11_USAGE DX11UsageMap[] = { D3D11_USAGE_IMMUTABLE, D3D11_USAGE_DEFAULT, D3D11_USAGE_DYNAMIC };

class DX11Buffer : public Buffer
{
public:

	DX11Buffer(D3D11_USAGE dxUsage, ID3D11Buffer* pBuffer, const D3D11_BUFFER_DESC &desc, uint32 size) :
		dxUsage(dxUsage),
		pBuffer(pBuffer),
		desc(desc),
		size(size)
	{}

	D3D11_USAGE dxUsage;
	ID3D11Buffer* pBuffer;
	D3D11_BUFFER_DESC desc;
	uint32 size;
};

// ==============================================

// ==============================================
// Blend State
// ==============================================

static const D3D11_BLEND D3D11BlendMap[] =
{
	D3D11_BLEND_ZERO,
	D3D11_BLEND_ONE,
	D3D11_BLEND_SRC_COLOR,
	D3D11_BLEND_INV_SRC_COLOR,
	D3D11_BLEND_SRC_ALPHA,
	D3D11_BLEND_INV_SRC_ALPHA,
	D3D11_BLEND_DEST_ALPHA,
	D3D11_BLEND_INV_DEST_ALPHA,
	D3D11_BLEND_DEST_COLOR,
	D3D11_BLEND_SRC_ALPHA_SAT,
	D3D11_BLEND_BLEND_FACTOR,
	D3D11_BLEND_INV_BLEND_FACTOR
};

static const D3D11_BLEND_OP D3D11BlendOpMap[] =
{
	D3D11_BLEND_OP_ADD, D3D11_BLEND_OP_SUBTRACT, D3D11_BLEND_OP_REV_SUBTRACT, D3D11_BLEND_OP_MIN, D3D11_BLEND_OP_MAX
};

class DX11BlendState : public BlendState
{
public:

	ID3D11BlendState* blendState;
};

// helper macros
#define GET_D3D11_BLEND(blend) D3D11BlendMap[static_cast<int32>(blend)]
#define GET_D3D11_BLEND_OP(blendOp) D3D11BlendOpMap[static_cast<int32>(blendOp)]

// ==============================================

// ==============================================
// Depth/Stencil State
// ==============================================

static const D3D11_COMPARISON_FUNC D3D11ComparisonFuncMap[] =
{
	D3D11_COMPARISON_NEVER,
	D3D11_COMPARISON_LESS,
	D3D11_COMPARISON_EQUAL,
	D3D11_COMPARISON_LESS_EQUAL,
	D3D11_COMPARISON_GREATER,
	D3D11_COMPARISON_NOT_EQUAL,
	D3D11_COMPARISON_GREATER_EQUAL,
	D3D11_COMPARISON_ALWAYS
};

static const D3D11_STENCIL_OP D3D11StencilOpMap[] =
{
	D3D11_STENCIL_OP_KEEP,
	D3D11_STENCIL_OP_ZERO,
	D3D11_STENCIL_OP_REPLACE,
	D3D11_STENCIL_OP_INCR_SAT,
	D3D11_STENCIL_OP_DECR_SAT,
	D3D11_STENCIL_OP_INVERT,
	D3D11_STENCIL_OP_INCR,
	D3D11_STENCIL_OP_DECR
};

class DepthStencilStateD3D11 : public DepthStencilState
{
public:

	DepthStencilStateD3D11(ID3D11DepthStencilState* pDepthStencilState) :
		pDepthStencilState(pDepthStencilState)
	{}

	ID3D11DepthStencilState* pDepthStencilState;
};

// helper macros
#define GET_D3D11_COMPARISON(comparison) D3D11ComparisonFuncMap[static_cast<int32>(comparison)]
#define GET_D3D11_STENCIL_OP(stencilOp) D3D11StencilOpMap[static_cast<int32>(stencilOp)]

// ==============================================

// ==============================================
// DirectX Debugging
// ==============================================

#define DX_ERROR_STR_BUFFER_SIZE 256

static std::string GetDXErrorString(HRESULT hr)
{
	HLOCAL pBuffer;

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
				  FORMAT_MESSAGE_IGNORE_INSERTS |
				  FORMAT_MESSAGE_ALLOCATE_BUFFER,
				  NULL,
				  hr,
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				  (LPTSTR)&pBuffer,
				  DX_ERROR_STR_BUFFER_SIZE,
				  NULL);

	return std::string((LPTSTR)pBuffer);
}

#define LOG_DX_ERROR(msgFormat, hr)								\
	LOG_ERROR(msgFormat " : %s", GetDXErrorString(hr).c_str())	\


#if defined(_DEBUG)

#define HR(_call)																\
{																				\
	HRESULT hr = _call;															\
	if(FAILED(hr))																\
	{																			\
		HRESULT hr = _call;														\
		Log::WriteError("DXError on line %i of file %s", __LINE__, __FILE__);	\
		Log::WriteError(GetDXErrorString(hr).c_str());							\
	}																			\
}																				\

#else
	#define HR(_call) (_call)
#endif

// ==============================================

#endif // _DX11_DEFINITIONS_H