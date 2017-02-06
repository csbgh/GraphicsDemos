#ifndef _INPUT_H
#define _INPUT_H

#include "DemoTypes.h"
#include "InputDefinitions.h"

struct InputState
{
	bool down;		// the inputs current state, true = pressed
	bool pressed;	// true if the input was pressed down on this update
	bool released;	// true if the input was released on this update
};

class Input
{
public:

	Input();

	void ProcessInput(const SDL_Event &sdlEvent);

	// resets the pressed states of keys / mouse buttons
	void Reset(bool first = false);

	// returns true if key is pressed down
	bool GetKey(KeyCode key);

	// returns true if key was pressed down in this frame
	bool GetKeyPressed(KeyCode key);

	// returns true if key was released in this frame
	bool GetKeyReleased(KeyCode key);

	// return true if button is pressed down
	bool GetMouse(MouseButton button);

	// returns true if button was pressed down in this frame
	bool GetMousePressed(MouseButton button);

	// returns true if button was released in this frame
	bool GetMouseReleased(MouseButton button);

	// returns the amount the mouse has moved in x and y since the last frame
	void GetMouseDelta(int32 &x, int32 &y);

	// sets x and y to the current mouse position inside the window
	void GetMousePos(int32 &x, int32 &y);

private:

	void SetKeyState(const SDL_Event &keyEvent);
	void SetMouseState(const SDL_Event &mouseEvent);

	// keyboard state
	InputState keyState[KEY_COUNT];

	// mouse button state
	InputState mouseState[KEY_COUNT];

	// mouse position state
	int32 mouseX, mouseY;
};

#endif // _INPUT_H