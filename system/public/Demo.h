#ifndef _DEMO_H
#define _DEMO_H

#include "DemoCommon.h"

class DemoSystem;
class IGraphicsDevice;

class Demo
{
public:

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Destroy() = 0;

	// called when the render api is first initialized or when a new api is swapped in
	// TODO : CreateResources(GraphicsDevice* device);
	virtual void CreateGraphics(IGraphicsDevice* gDevice) = 0;

	// virtual void InitializeGraphics();

	// called each frame for main rendering logic
	// TODO : Render
	virtual void Draw(IGraphicsDevice* gDevice) = 0;

	// cleanup graphics resources when a rendering api is swapped or the program is exiting
	// TODO : ReleaseResources
	virtual void ReleaseGraphics(IGraphicsDevice* gDevice) = 0;

	void SetDemoSystem(DemoSystem *sys) { demoSystem = sys; }

protected:

	DemoSystem *demoSystem;

};

#endif // _DEMO_H