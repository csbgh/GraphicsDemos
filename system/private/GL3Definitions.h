#ifndef _GL3_DEFINITIONS_H
#define _GL3_DEFINITIONS_H

#include <GL\glew.h>
#include <GL\wglew.h>

#include "DemoCommon.h"

#define GL3_MAJOR 3
#define GL3_MINOR 3

#define GL3_SHADER_LOC "resources/shaders/GL3/"

#define LOG_GL_ERROR(_msg) logGLError(_msg);
#define CHECK_GL_ERROR(_msg) CheckGLError(_msg)

// maps ZPFaceCullMode to OpenGL equivalent
//static const GLenum GL3CullModeMap[] = { GL_BACK, GL_FRONT, GL_BACK, GL_FRONT_AND_BACK }; // CULL_NONE is mapped to GL_BACK but it should never be actually used

// maps ZPFillMode to OpenGL equivalent
//static const GLenum GL3FillModeMap[] = { GL_FILL, GL_LINE, GL_POINT };

// maps TextureWrapMode to OpenGL equivalent
static const GLint GL3WrapMode[] = { GL_REPEAT, GL_CLAMP_TO_EDGE };

// maps ZPDepthFunc to OpenGL
//static const GLenum GL3DepthFuncMap[] = { GL_LESS, GL_EQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS };

// ==============================================
// GL3 Buffers
// ==============================================

// maps BufferTarget to OpenGL equivalent
static const GLenum GL3TargetMap[] = { GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_UNIFORM_BUFFER };

// maps BufferUsage to OpenGL equivalent
static const GLenum GL3UsageMap[] = { GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW };

class BufferGL3 : public Buffer
{
public:

	BufferGL3(GLuint glID, GLenum glUsage, GLenum glTarget, uint32 size, BufferUsage usage) : 
		glID(glID),
		glUsage(glUsage),
		glTarget(glTarget),
		size(size),
		usage(usage)
	{}

	GLuint glID;
	GLenum glUsage;
	GLenum glTarget;

	uint32 size;
	BufferUsage usage;
};

// ==============================================

// ==============================================
// Depth/Stencil State
// ==============================================

static const GLenum GL3ComparisonFuncMap[] =
{
	GL_NEVER,
	GL_LESS,
	GL_EQUAL,
	GL_LEQUAL,
	GL_GREATER,
	GL_NOTEQUAL,
	GL_GEQUAL,
	GL_ALWAYS
};

static const GLenum GL3StencilOpMap[] =
{
	GL_KEEP,
	GL_ZERO,
	GL_REPLACE,
	GL_INCR_WRAP,
	GL_DECR_WRAP,
	GL_INVERT,
	GL_INCR,
	GL_DECR
};

struct StencilFuncGL3
{
	GLenum glStencilFailOp;
	GLenum glDepthFailOp;
	GLenum glStencilPassOp;
	GLenum glFunc;
};

class DepthStencilStateGL3 : public DepthStencilState
{
public:

	bool depthEnabled;
	GLboolean glDepthWrite;
	GLenum glDepthComparison;

	bool stencilEnabled;
	GLuint glStencilReadMask;
	GLuint glStencilWriteMask;

	StencilFuncGL3 frontFace;
	StencilFuncGL3 backFace;

	DepthStencilStateDesc desc;
};

// helper macros
#define GET_GL3_COMPARISON(comparison) GL3ComparisonFuncMap[static_cast<int32>(comparison)]
#define GET_GL3_STENCIL_OP(stencilOp) GL3StencilOpMap[static_cast<int32>(stencilOp)]

// ==============================================

static const GLenum GLBaseTypes[] =
{
	GL_BYTE,
	GL_UNSIGNED_BYTE,
	GL_SHORT,
	GL_UNSIGNED_SHORT,
	GL_INT,
	GL_UNSIGNED_INT,
	GL_FLOAT,
	GL_DOUBLE
};

static const char *getGLErrorString(GLenum err)
{
	switch (err)
	{
		case GL_NO_ERROR: return "GL_NO_ERROR";
		case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
		case GL_STACK_UNDERFLOW: return"GL_STACK_UNDERFLOW";
		case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
		default:
			return "UNKNOWN_ERROR";
	}
}

static void logGLError(const char *msg)
{
	GLenum err = glGetError();

	if (err == GL_NO_ERROR)
	{
		LOG_ERROR(msg);
		return;
	}

	LOG_ERROR("GL3 : %s : [%s]", msg, getGLErrorString(err));
}

static bool CheckGLError(const char *msg)
{
	GLenum err = glGetError();

	if (err == GL_NO_ERROR)
	{
		return false;
	}

	LOG_ERROR("GL3 %s : [%s]", msg, getGLErrorString(err));
	return true;
}

#if defined(_DEBUG)

#define CHECK_GL(_call)															\
_call;																			\
{																				\
	GLenum err = glGetError();													\
	if(err != GL_NO_ERROR)														\
	{																			\
		Log::WriteError("GLError on line %i of file %s", __LINE__, __FILE__);	\
		Log::WriteError(getGLErrorString(err));									\
	}																			\
}																				\

#else
	#define CHECK_GL(_call) (_call)
#endif

#endif // _GL3_DEFINITIONS_H