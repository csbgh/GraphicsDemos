#include "GL3Device.h"

#include <vector>

std::string GL3Device::GetAPIName()
{
	return "OpenGL 3.3";
}

bool GL3Device::Create(const RenderInfo& info)
{
	renderInfo = info;

	// check that a valid window pointer was passed
	if (!info.wndPointer)
	{
		LOG_ERROR("Could not create OpenGL context, window pointer was null");
		return false;
	}

	windowContext = *static_cast<HDC*>(info.wndPointer);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;
	
	int pixel_format = ChoosePixelFormat(windowContext, &pfd);
	if (pixel_format == 0)
	{
		DWORD error_code = GetLastError();
		LOG_ERROR("ChoosePixelFormat Failed : Error Code %u", error_code);
		return false;
	}

	// make that match the device contexts current pixel format 
	if (SetPixelFormat(windowContext, pixel_format, &pfd) == FALSE)
	{
		DWORD error_code = GetLastError();
		LOG_ERROR("SetPixelFormat Failed : Error Code %u", error_code);
		return false;
	}

	HGLRC tempContext;
	if ((tempContext = wglCreateContext(windowContext)) == NULL)
	{
		LOG_ERROR("Failed create temp GL context!");
		return false;
	}

	if (wglMakeCurrent(windowContext, tempContext) == FALSE)
	{
		LOG_ERROR("Failed making temp GL context current!");
		return false;
	}

	GLenum glew_err = glewInit();
	if (GLEW_OK != glew_err)
	{
		std::string error_string = reinterpret_cast<const char*>(glewGetErrorString(glew_err));
		LOG_ERROR("Unable to initliaze GLEW : %s", error_string.c_str());
	}
	else
	{
		LOG("Successfully Initialized GLEW!");
	}

#ifdef _DEBUG
	int contextType = WGL_CONTEXT_DEBUG_BIT_ARB;
#else
	int contextType = WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
#endif

	const int cca_list[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, GL3_MAJOR,
		WGL_CONTEXT_MINOR_VERSION_ARB, GL3_MINOR,
		WGL_CONTEXT_FLAGS_ARB, contextType,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0, 0
	};

	// TODO : allow to continue using temp context
	if ((glContextHandle = wglCreateContextAttribsARB(windowContext, 0, cca_list)) != NULL)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempContext);
		
		if (wglMakeCurrent(windowContext, glContextHandle) == FALSE)
		{
			LOG_ERROR("wglMakeCurrent Failed");
			return false;
		}
	}
	else
	{
		LOG_ERROR("wglCreateContextAttribsARB Failed");
		return false;
	}

	int major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	LOG("Created OpenGL Context = %i.%i", major, minor);

	return true;
}

void GL3Device::Initialize()
{
	// Create depth/stencil state
	DepthStencilStateDesc defaultDepthStencilDesc;
	defaultDepthStencilState = CreateDepthStencilState(defaultDepthStencilDesc);
	SetDepthStencilState(defaultDepthStencilState);

	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	//glCullFace(GL_FRONT);
	//glFrontFace(GL_CCW);
}

void GL3Device::Destroy()
{
	wglDeleteContext(glContextHandle);
}

void GL3Device::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GL3Device::Present()
{
	SwapBuffers(windowContext);
}

void GL3Device::SetVSync(bool enabled)
{
	if (enabled)
	{
		wglSwapIntervalEXT(1);
	}
	else
	{
		wglSwapIntervalEXT(0);
	}
}

void GL3Device::SetShader(Shader* shader)
{
	GL3Shader* gl3Shader = static_cast<GL3Shader*>(shader);

	if (gl3Shader != nullptr)
	{
		glUseProgram(gl3Shader->programID);
	}
}

void GL3Device::SetTexture(Texture* texture, uint32 slot)
{
	GL3Texture* gl3Texture = static_cast<GL3Texture*>(texture);

	if (texture)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, gl3Texture->textureID);
	}
}

void GL3Device::DrawMesh(Mesh* mesh)
{
	MeshGL3* glMesh = static_cast<MeshGL3*>(mesh);

	if (glMesh != nullptr)
	{
		glBindVertexArray(glMesh->vertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, glMesh->vertexCount);
	}
}

void GL3Device::DrawMeshIndexed(Mesh* mesh, uint32 elementCount, uint32 vertexOffset, uint16 indexOffset)
{
	MeshGL3* glMesh = static_cast<MeshGL3*>(mesh);

	if (glMesh != nullptr)
	{
		if (elementCount == 0)
		{
			elementCount = glMesh->indexCount;
		}

		glBindVertexArray(glMesh->vertexArrayID);

		glDrawElementsBaseVertex(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, (void*)(indexOffset * sizeof(GLushort)), vertexOffset);

		glBindVertexArray(0);
	}
}

Mesh* GL3Device::CreateMesh(const MeshData &meshData, VertexAttributes vertexAttributeFlags, BufferUsage usage)
{
	// clear binded vertex array object
	glBindVertexArray(0);

	// create and bind a new VAO
	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	CHECK_GL(glBindVertexArray(vertexArrayID));

	uint32 stride = GetAttributeMaskSize(vertexAttributeFlags);

	// Create vertex and index buffers
	BufferGL3* vertexBuffer = CreateBuffer(meshData.vertexData,
										   meshData.vertexCount * stride,
										   BufferTarget::Vertex, usage);

	BufferGL3* indexBuffer = CreateBuffer(meshData.indexData,
										  meshData.indexCount * sizeof(uint16),
										  BufferTarget::Index, usage);

	// define generic vertex attribute data
	SetVertexAttributes(vertexAttributeFlags, stride);

	// reset bound vao so that it is not modified
	glBindVertexArray(0);

	// reset
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return new MeshGL3(vertexArrayID, vertexBuffer, indexBuffer, meshData.vertexCount, meshData.indexCount, stride);
}

Mesh* GL3Device::CreateMesh(const MeshDataList &meshData, VertexAttributes vertexAttributeFlags, BufferUsage usage)
{
	// clear binded vertex array object
	glBindVertexArray(0);

	// create and bind a new VAO
	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	CHECK_GL(glBindVertexArray(vertexArrayID));

	uint32 stride = GetAttributeMaskSize(vertexAttributeFlags);

	// Create vertex and index buffers
	BufferGL3* vertexBuffer = CreateBuffer(meshData.vertices, meshData.dataCount,
										   meshData.vertexCount * stride,
										   BufferTarget::Vertex, usage);

	BufferGL3* indexBuffer = CreateBuffer(meshData.indices, meshData.dataCount,
										  meshData.indexCount * sizeof(uint16),
										  BufferTarget::Index, usage);

	// define generic vertex attribute data
	SetVertexAttributes(vertexAttributeFlags, stride);

	// reset bound vao so that it is not modified
	glBindVertexArray(0);

	// reset
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return new MeshGL3(vertexArrayID, vertexBuffer, indexBuffer, meshData.vertexCount, meshData.indexCount, stride);
}

void GL3Device::UpdateMesh(Mesh* mesh, const MeshData &meshData)
{
	MeshGL3* glMesh = static_cast<MeshGL3*>(mesh);

	UpdateBuffer(glMesh->vertexBuffer, meshData.vertexData, meshData.vertexCount * glMesh->stride);
	UpdateBuffer(glMesh->indexBuffer, meshData.indexData, meshData.indexCount * sizeof(uint16));

	glMesh->vertexCount = meshData.vertexCount;
	glMesh->indexCount = meshData.indexCount;
}

void GL3Device::UpdateMesh(Mesh* mesh, const MeshDataList &meshData)
{
	MeshGL3* glMesh = static_cast<MeshGL3*>(mesh);

	UpdateBuffer(glMesh->vertexBuffer, meshData.vertices, meshData.dataCount, meshData.vertexCount * glMesh->stride);
	UpdateBuffer(glMesh->indexBuffer, meshData.indices, meshData.dataCount, meshData.indexCount * sizeof(uint16));

	glMesh->vertexCount = meshData.vertexCount;
	glMesh->indexCount = meshData.indexCount;
}

void GL3Device::ReleaseMesh(Mesh* mesh)
{
	MeshGL3* glMesh = static_cast<MeshGL3*>(mesh);

	if (glMesh == nullptr)
		return;

	ReleaseBuffer(glMesh->vertexBuffer);
	ReleaseBuffer(glMesh->indexBuffer);

	delete glMesh;
}

Shader* GL3Device::CreateShader(const std::string &name)
{
	GL3Shader* newShader = new GL3Shader();

	// create vertex shader
	GLuint vertexShader = CompileShaderObject(GL3_SHADER_LOC + name + ".vert", GL_VERTEX_SHADER);

	if(vertexShader == 0)
	{
		LOG_GL_ERROR("Unable to create shader, failed creating vertex shader");
		return nullptr;
	}

	// create fragment shader
	GLuint fragmentShader = CompileShaderObject(GL3_SHADER_LOC +  name + ".frag", GL_FRAGMENT_SHADER);

	if (fragmentShader == 0)
	{
		LOG_GL_ERROR("Unable to create shader, failed creating fragment shader");
		return nullptr;
	}

	// create shader program
	GLuint programID = glCreateProgram();

	if (programID == 0)
	{
		LOG_GL_ERROR("Error creating shader program, glCreateProgram returned 0");
		return nullptr;
	}

	// attach vertex and fragment shaders
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);

	// link program
	glLinkProgram(programID);

	// check shader program linked succesfully and is ready to use
	GLint linkStatus = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == GL_FALSE)
	{
		GLint info_length;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &info_length);

		std::vector<char> errorMsg(glm::max(info_length, int(1)));
		glGetProgramInfoLog(programID, info_length, NULL, &errorMsg[0]);

		LOG_GL_ERROR("Failed linking shader program");
		LOG_ERROR(&errorMsg[0]);

		return nullptr;
	}

	newShader->vertexShader = vertexShader;
	newShader->fragmentShader = fragmentShader;
	newShader->programID = programID;

	return newShader;
}

void GL3Device::ReleaseShader(Shader* shader)
{

}

Texture* GL3Device::CreateTexture(uint8 *data, const TextureSettings &settings)
{
	CHECK_GL_ERROR("No Texture Error");
	GL3Texture* newTexture = new GL3Texture(settings);

	// create and bind GL texture
	glGenTextures(1, &newTexture->textureID);

	// save bound texture before modifying it
	GLint lastSampler = 0;
	glGetIntegerv(GL_SAMPLER_BINDING, &lastSampler);

	glBindTexture(GL_TEXTURE_2D, newTexture->textureID);
	CHECK_GL_ERROR("Failed creating texture a");
	// set texture wrapping mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, newTexture->glWrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, newTexture->glWrapMode);

	// set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, newTexture->glMinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, newTexture->glMagFilter);
	CHECK_GL_ERROR("Failed creating textureb");
	// anisotropic filtering
	if (settings.anisoLevel > 0.0f)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, settings.anisoLevel);
	}
	CHECK_GL_ERROR("Failed tex params");
	// upload initial texture data
	/*glTexImage2D(GL_TEXTURE_2D, 0, newTexture->glInternalFormat,
				 settings.width, settings.height, 0,
			     newTexture->glFormat, newTexture->glType, data);*/

	// determine number of mip map levels and upload initial texture data
	int32 numLevels = !settings.mipMaps ? 1 : static_cast<int32>(ceil(log2(glm::max(settings.width, settings.height)))) + 1;

	glTexStorage2D(GL_TEXTURE_2D, numLevels, newTexture->glInternalFormat, settings.width, settings.height);
	CHECK_GL_ERROR("Failed storage");
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, settings.width, settings.height, newTexture->glFormat, newTexture->glType, data);
	CHECK_GL_ERROR("Failed sub image");
	// TODO : refactor so that GL3Texture is only created after succesfully making a GL texture
	if (CHECK_GL_ERROR("Failed creating texture"))
	{
		delete newTexture;
		newTexture = nullptr;

		// restore state
		glBindTexture(GL_TEXTURE_2D, lastSampler);

		return nullptr;
	}

	// generate mip maps if they are enabled in the texture settings
	if (settings.mipMaps)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// restore state
	glBindTexture(GL_TEXTURE_2D, lastSampler);

	return newTexture;
}

void GL3Device::ReleaseTexture(Texture* pTexture)
{
	GL3Texture* glTexture = static_cast<GL3Texture*>(pTexture);

	if (glTexture == nullptr)
		return;

	glDeleteTextures(1, &glTexture->textureID);

	delete glTexture;
}

void GL3Device::SetUniformBuffer(uint32 slot, Buffer* buffer, ShaderStage stage)
{
	BufferGL3* gl3Buffer = static_cast<BufferGL3*>(buffer);

	DS_ASSERT(gl3Buffer);									// gl3Buffer must not be null
	DS_ASSERT(gl3Buffer->glTarget == GL_UNIFORM_BUFFER);	// must be uniform buffer

	CHECK_GL(glBindBufferRange(GL_UNIFORM_BUFFER, slot, gl3Buffer->glID, 0, gl3Buffer->size));
}

BufferGL3* GL3Device::CreateBuffer(const void* data, uint32 size, BufferTarget target, BufferUsage usage)
{
	// generate new buffer
	GLuint glBuffer;
	glGenBuffers(1, &glBuffer);

	// get target and usage GL values
	GLenum glUsage = GL3UsageMap[static_cast<int32>(usage)];
	GLenum glTarget = GL3TargetMap[static_cast<int32>(target)];

	CHECK_GL(glBindBuffer(glTarget, glBuffer));

	// check if passed data is null
	if (data)
	{
		// create the new buffer with initial data
		CHECK_GL(glBufferData(glTarget, size, data, glUsage));
	}
	else
	{
		// if no data was passed create uninitialized buffer with the given size
		CHECK_GL(glBufferData(glTarget, size, NULL, glUsage));
	}

	// glBindBuffer(glTarget, 0);

	return new BufferGL3(glBuffer, glUsage, glTarget, size, usage);
}

BufferGL3* GL3Device::CreateBuffer(const std::vector<BufferData> &data, uint32 dataCount, uint32 bufferSize, BufferTarget target, BufferUsage usage)
{
	// generate new buffer
	GLuint glBuffer;
	glGenBuffers(1, &glBuffer);

	// get target and usage GL values
	GLenum glUsage = GL3UsageMap[static_cast<int32>(usage)];
	GLenum glTarget = GL3TargetMap[static_cast<int32>(target)];

	CHECK_GL(glBindBuffer(glTarget, glBuffer));

	// if we only have one BufferData, or if the mesh is static copy one chunk of data
	if (usage == BufferUsage::Static || dataCount == 1)
	{
		CHECK_GL(glBufferData(glTarget, bufferSize, data[0].pData, glUsage));
	}
	else
	{
		// Create buffer with unitialized data with the required size
		CHECK_GL(glBufferData(glTarget, bufferSize, NULL, glUsage));

		// copy data from each BufferData object in to our new buffer
		uint32 bufferOffset = 0;
		for (uint32 md = 0; md < dataCount; md++)
		{
			CHECK_GL(glBufferSubData(glTarget, bufferOffset, data[md].sizeBytes, data[md].pData));
			bufferOffset += data[md].sizeBytes;
		}
	}

	// glBindBuffer(glTarget, 0);

	return new BufferGL3(glBuffer, glUsage, glTarget, bufferSize, usage);
}

void GL3Device::UpdateBuffer(Buffer* buffer, const void* data, uint32 size)
{
	BufferGL3* gl3Buffer = static_cast<BufferGL3*>(buffer);

	DS_ASSERT(gl3Buffer);								// gl3Buffer must not be null
	DS_ASSERT(gl3Buffer->usage != BufferUsage::Static);	// Static buffers should not be modified

	CHECK_GL(glBindBuffer(gl3Buffer->glTarget, gl3Buffer->glID));

	// if the buffer is too small to fit the new data reallocate the buffer to the required size
	// otherwise copy data in to current buffer
	if (size > gl3Buffer->size)
	{
		CHECK_GL(glBufferData(gl3Buffer->glTarget, size, data, gl3Buffer->glUsage));
		gl3Buffer->size = size;
	}
	else
	{
		CHECK_GL(glBufferSubData(gl3Buffer->glTarget, 0, size, data));
	}

	glBindBuffer(gl3Buffer->glTarget, 0);
}

void GL3Device::UpdateBuffer(Buffer* buffer, const std::vector<BufferData> &data, uint32 dataCount, uint32 bufferSize)
{
	BufferGL3* gl3Buffer = static_cast<BufferGL3*>(buffer);

	DS_ASSERT(gl3Buffer);								// gl3Buffer must not be null
	DS_ASSERT(gl3Buffer->usage != BufferUsage::Static);	// Static buffers should not be modified

	CHECK_GL(glBindBuffer(gl3Buffer->glTarget, gl3Buffer->glID));

	// Expand buffer if it is not big enough to fit the new data
	if (bufferSize > gl3Buffer->size)
	{
		CHECK_GL(glBufferData(gl3Buffer->glTarget, bufferSize, NULL, gl3Buffer->glUsage));
		gl3Buffer->size = bufferSize;
	}

	// copy data from each BufferData object in to our new buffer
	uint32 bufferOffset = 0;
	for (uint32 md = 0; md < dataCount; md++)
	{
		CHECK_GL(glBufferSubData(gl3Buffer->glTarget, bufferOffset, data[md].sizeBytes, data[md].pData));
		bufferOffset += data[md].sizeBytes;
	}

	glBindBuffer(gl3Buffer->glTarget, 0);
}

void GL3Device::ReleaseBuffer(Buffer* buffer)
{
	BufferGL3* gl3Buffer = static_cast<BufferGL3*>(buffer);

	DS_ASSERT(gl3Buffer); // gl3Buffer must not be null

	CHECK_GL(glDeleteBuffers(1, &gl3Buffer->glID));
	delete buffer;
}

void GL3Device::SetScissorRects(uint32 numRects, const DSRect* pRects)
{
	if (pRects)
	{
		glScissor(pRects[0].left,
				  renderInfo.resolutionY - pRects[0].bottom,
				  pRects[0].right - pRects[0].left,
				  pRects[0].bottom - pRects[0].top);
	}
}

void GL3Device::GetScissorRects(uint32* pNumRects, DSRect* pRects)
{
	GLint box[4];
	glGetIntegerv(GL_SCISSOR_BOX, box);

	GLint x = box[0];
	GLint y = box[1];
	GLint width = box[2];
	GLint height = box[3];

	if (pNumRects)
		*pNumRects = 1;

	if (pRects)
	{
		pRects[0] = DSRect(x, y, x + width, y + height);
	}
}

BlendState* GL3Device::CreateBlendState(BlendProperties properties)
{
	return nullptr;
}

void GL3Device::SetBlendState(BlendState* state)
{

}

DepthStencilStateGL3* GL3Device::CreateDepthStencilState(DepthStencilStateDesc& desc)
{
	DepthStencilStateGL3* state = new DepthStencilStateGL3();

	state->depthEnabled = desc.depthEnabled;
	state->glDepthWrite = static_cast<GLboolean>(desc.depthWriteEnabled);
	state->glDepthComparison = GET_GL3_COMPARISON(desc.depthFunc);

	state->stencilEnabled = desc.stencilEnabled;
	state->glStencilReadMask = desc.stencilRead;
	state->glStencilWriteMask = desc.stencilWrite;

	state->frontFace.glStencilFailOp = GET_GL3_STENCIL_OP(desc.frontFace.stencilfailOp);
	state->frontFace.glStencilPassOp = GET_GL3_STENCIL_OP(desc.frontFace.stencilPassOp);
	state->frontFace.glDepthFailOp = GET_GL3_STENCIL_OP(desc.frontFace.depthFailOp);
	state->frontFace.glFunc = GET_GL3_COMPARISON(desc.frontFace.stencilFunc);

	state->backFace.glStencilFailOp = GET_GL3_STENCIL_OP(desc.backFace.stencilfailOp);
	state->backFace.glStencilPassOp = GET_GL3_STENCIL_OP(desc.backFace.stencilPassOp);
	state->backFace.glDepthFailOp = GET_GL3_STENCIL_OP(desc.backFace.depthFailOp);
	state->backFace.glFunc = GET_GL3_COMPARISON(desc.backFace.stencilFunc);

	return state;
}

DepthStencilState* GL3Device::GetCurrentDepthStencilState()
{
	return curDepthStencilState;
}

void GL3Device::SetDepthStencilState(DepthStencilState* state)
{
	DepthStencilStateGL3* gl3State = static_cast<DepthStencilStateGL3*>(state);

	if (state == nullptr)
	{
		SetDepthStencilState(defaultDepthStencilState);
		return;
	}

	// TODO : think of a better way of doing this
	if (curDepthStencilState == nullptr)
	{
		if (gl3State->depthEnabled)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		glDepthMask(gl3State->glDepthWrite);
		CHECK_GL(glDepthFunc(gl3State->glDepthComparison));

		if (gl3State->stencilEnabled)
		{
			glEnable(GL_STENCIL_TEST);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
		}

		glStencilMask(gl3State->glStencilReadMask);
		CHECK_GL(glStencilOpSeparate(GL_FRONT, gl3State->frontFace.glStencilFailOp, gl3State->frontFace.glDepthFailOp, gl3State->frontFace.glStencilPassOp));
		CHECK_GL(glStencilOpSeparate(GL_BACK, gl3State->backFace.glStencilFailOp, gl3State->backFace.glDepthFailOp, gl3State->backFace.glStencilPassOp));
		CHECK_GL(glStencilFuncSeparate(GL_FRONT, gl3State->frontFace.glFunc, 0, gl3State->glStencilReadMask));
		CHECK_GL(glStencilFuncSeparate(GL_FRONT, gl3State->backFace.glFunc, 0, gl3State->glStencilReadMask));
	}
	else if (curDepthStencilState != gl3State)
	{
		// Depth State
		// check if depth testing state has changed
		if (curDepthStencilState->depthEnabled != gl3State->depthEnabled)
		{
			if (gl3State->depthEnabled)
			{
				glEnable(GL_DEPTH_TEST);
			}
			else
			{
				glDisable(GL_DEPTH_TEST);
			}
		}

		// check if depth writing state has changed
		if (curDepthStencilState->glDepthWrite != gl3State->glDepthWrite)
		{
			glDepthMask(gl3State->glDepthWrite);
		}

		// check if the depth test function has changed
		if (curDepthStencilState->glDepthComparison != gl3State->glDepthComparison)
		{
			CHECK_GL(glDepthFunc(gl3State->glDepthComparison));
		}

		// Stencil State
		// check if stencil testing state has changed
		if (curDepthStencilState->stencilEnabled != gl3State->stencilEnabled)
		{
			if (gl3State->stencilEnabled)
			{
				glEnable(GL_STENCIL_TEST);
			}
			else
			{
				glDisable(GL_STENCIL_TEST);
			}
		}

		// check if stencil write mask has changed
		if (curDepthStencilState->glStencilWriteMask != gl3State->glStencilWriteMask)
		{
			glStencilMask(gl3State->glStencilReadMask);
		}

		// check if the stencil operation has changed for the front face
		if (curDepthStencilState->frontFace.glStencilFailOp != gl3State->frontFace.glStencilFailOp	||
			curDepthStencilState->frontFace.glDepthFailOp	!= gl3State->frontFace.glDepthFailOp	|| 
			curDepthStencilState->frontFace.glStencilPassOp != gl3State->frontFace.glStencilPassOp)
		{
			CHECK_GL(glStencilOpSeparate(GL_FRONT, gl3State->frontFace.glStencilFailOp,
										 gl3State->frontFace.glDepthFailOp, gl3State->frontFace.glStencilPassOp));
		}

		// check if the stencil operation has changed for the front face
		if (curDepthStencilState->backFace.glStencilFailOp != gl3State->backFace.glStencilFailOp	||
			curDepthStencilState->backFace.glDepthFailOp	!= gl3State->backFace.glDepthFailOp	||
			curDepthStencilState->backFace.glStencilPassOp != gl3State->backFace.glStencilPassOp)
		{
			CHECK_GL(glStencilOpSeparate(GL_BACK, gl3State->backFace.glStencilFailOp,
										 gl3State->backFace.glDepthFailOp, gl3State->backFace.glStencilPassOp));
		}


		// check if the stencil function or the stencil read mask has changed for the front face
		if (curDepthStencilState->glStencilReadMask != gl3State->glStencilReadMask ||
			curDepthStencilState->frontFace.glFunc != gl3State->frontFace.glFunc)
		{
			CHECK_GL(glStencilFuncSeparate(GL_FRONT, gl3State->frontFace.glFunc, 0, gl3State->glStencilReadMask));
		}

		// check if the stencil function or the stencil read mask has changed for the back face
		if (curDepthStencilState->glStencilReadMask != gl3State->glStencilReadMask ||
			curDepthStencilState->backFace.glFunc != gl3State->backFace.glFunc)
		{
			CHECK_GL(glStencilFuncSeparate(GL_BACK, gl3State->backFace.glFunc, 0, gl3State->glStencilReadMask));
		}
	}

	curDepthStencilState = gl3State;
}

void GL3Device::SetClearColor(const vec4 &color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void GL3Device::SetViewport(int32 x, int32 y, int32 width, int32 height)
{
	glViewport(x, y, width, height);
	glScissor(x, y, width, height);
}

void GL3Device::OnResolutionChanged(uint32 width, uint32 height)
{
	
	SetViewport(0, 0, width, height);
}

GLuint GL3Device::CompileShaderObject(const std::string &fileName, GLenum shaderType)
{
	// create new shader object of the given type
	GLuint shaderID = glCreateShader(shaderType);

	// check that we succesfully created a new shader object
	if (shaderID == 0)
	{
		LOG_ERROR("Failed creating shader %s", fileName.c_str());
		return 0;
	}

	// load shader from file
	int32 dataSize = 0;
	char* fileData = FileReadAll(fileName, dataSize);

	if (fileData == nullptr)
	{
		LOG_ERROR("Unable to compile shader [%s] could not open source file.", fileName.c_str());
		return 0;
	}

	std::string shaderStr = std::string(fileData, dataSize);

	// set shader object source code
	const GLchar* shaderSource = (const GLchar*)shaderStr.c_str();
	glShaderSource(shaderID, 1, &shaderSource, nullptr);

	// compile the shader object
	glCompileShader(shaderID);

	// check the result of the shaders compilation
	GLint result = GL_FALSE;
	GLint infoLength = 0;

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLength);

	// output information from the compilation of the shader if there is any
	if (infoLength > 0)
	{
		GLchar *infoLog = new GLchar[infoLength];
		glGetShaderInfoLog(shaderID, infoLength, NULL, infoLog);

		LOG("============== Shader Object Log =============");
		LOG("Shader [%s] : \n%s", fileName.c_str(), infoLog);
		LOG("==============================================");

		delete [] infoLog;
	}

	// if the compilation failed
	if (result == GL_FALSE)
	{
		LOG_ERROR("Failed compiling shader [%s]", fileName.c_str());
		return 0;
	}

	return shaderID;
}

void GL3Device::SetVertexAttributes(VertexAttributes vertexAttributeFlags, uint32 stride)
{
	// find each set bit in the mask, get the index of the bit and then clear it, continue until all bits cleared
	// each active bit is an active vertex attribute, retrieve and setup the values for that attribute
	uint32 offset = 0;
	uint32 attribIndex = 0;
	for (uint32 mask = ToIntegral(vertexAttributeFlags); mask; mask &= mask - 1)
	{
		uint32 index = Bit::LeastSignifcantBit(mask);

		const AttributeProperties &properties = attributeProperties[index];
		GLenum glType = GLBaseTypes[static_cast<int32>(properties.type)];

		CHECK_GL(glEnableVertexAttribArray(attribIndex));
		CHECK_GL(glVertexAttribPointer(attribIndex, properties.components, glType, properties.normalized, stride, (GLvoid*)offset));

		offset += properties.components * properties.typeSizeBytes;
		attribIndex++;
	}
}