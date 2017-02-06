#ifndef _SIMPLE_DEMO_H
#define _SIMPLE_DEMO_H

#include <Demo.h>

class Shader;
class Mesh;
class Texture;

class SimpleDemo : public Demo
{
public:

	void Initialize();
	void Update();
	void Destroy();

	void CreateGraphics(IGraphicsDevice* gDevice);
	void Draw(IGraphicsDevice* gDevice);
	void ReleaseGraphics(IGraphicsDevice* gDevice);

private:

	void SetFace(uint32 offset, uint32 i1, uint32 i2, uint32 i3, uint32 i4);

	Shader* myShader = nullptr;
	Texture* myTexture = nullptr;
	Mesh* myMesh = nullptr;
	Mesh* myMesh2 = nullptr;
};

#endif // _SIMPLE_DEMO_H