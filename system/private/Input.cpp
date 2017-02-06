#include "Input.h"
#include "DemoCommon.h"

Input::Input()
{
	mouseX = 0;
	mouseY = 0;

	Reset(true);
}

void Input::ProcessInput(const SDL_Event &sdlEvent)
{
	switch (sdlEvent.type)
	{
		case SDL_KEYDOWN:
			SetKeyState(sdlEvent);
			break;

		case SDL_KEYUP:
			SetKeyState(sdlEvent);
			break;

		case SDL_MOUSEBUTTONDOWN:
			SetMouseState(sdlEvent);
			break;

		case SDL_MOUSEBUTTONUP:
			SetMouseState(sdlEvent);
			break;

		case SDL_MOUSEMOTION:

			mouseX = sdlEvent.motion.x;
			mouseY = sdlEvent.motion.y;

			break;
	}
}

void Input::Reset(bool first)
{
	for (int32 k = 0; k < KEY_COUNT; k++)
	{
		keyState[k].pressed = false;
		keyState[k].released = false;
		if (first) { keyState[k].down = false; }
	}

	for (int32 mb = 0; mb < MOUSE_BUTTONS; mb++)
	{
		mouseState[mb].pressed = false;
		mouseState[mb].released = false;
		if (first) { mouseState[mb].down = false; }
	}
}

bool Input::GetKey(KeyCode key)
{
	return keyState[(int32)key].down;
}

bool Input::GetKeyPressed(KeyCode key)
{
	return keyState[(int32)key].pressed;
}

bool Input::GetKeyReleased(KeyCode key)
{
	return keyState[(int32)key].released;
}

bool Input::GetMouse(MouseButton button)
{
	return mouseState[((int32)button) - 1].down;
}

bool Input::GetMousePressed(MouseButton button)
{
	return mouseState[((int32)button) - 1].pressed;
}

bool Input::GetMouseReleased(MouseButton button)
{
	return mouseState[((int32)button) - 1].released;
}

void Input::GetMouseDelta(int32 &x, int32 &y)
{

}

void Input::GetMousePos(int32 &x, int32 &y)
{
	x = mouseX;
	y = mouseY;
}

void Input::SetKeyState(const SDL_Event &keyEvent)
{
	// map the sdl key to our key enum, then get the array index its state is located at
	int32 keyIndex = static_cast<int32>(MapSDLKey(keyEvent.key.keysym.sym));

	// if the key was pressed down
	if (keyEvent.type == SDL_KEYDOWN)
	{
		keyState[keyIndex].down = true;
		keyState[keyIndex].pressed = true;
	}
	else // if key was not pressed down, only other event is the key was released
	{
		keyState[keyIndex].down = false;
		keyState[keyIndex].released = true;
	}
}

void Input::SetMouseState(const SDL_Event &mouseEvent)
{
	// get the mouse button array index
	int32 mouseButtonIndex = static_cast<int32>(mouseEvent.button.button) - 1;

	// if the mouse button was pressed down
	if (mouseEvent.type == SDL_MOUSEBUTTONDOWN)
	{
		mouseState[mouseButtonIndex].down = true;
		mouseState[mouseButtonIndex].pressed = true;
	}
	else // if button was not pressed down, only other event is the button was released
	{
		mouseState[mouseButtonIndex].down = false;
		mouseState[mouseButtonIndex].released = true;
	}
}