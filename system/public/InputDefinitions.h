#ifndef _INPUT_DEFINITIONS_H
#define _INPUT_DEFINITIONS_H

#include <SDL.h>

enum class KeyCode : int32
{
	// letters
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P,
	Q, R, S, T, U, V, W, X, Y, Z,

	// numeric
	NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9, NUM_0,

	PLUS, MINUS, ENTER, LSHIFT, ESC, DELETE_KEY, LEFT, RIGHT, UP, DOWN, TAB, BACKSPACE, SPACE,

	EXCLAM, AT, HASH, DOLLAR, PERCENT, CARET, AMPERSAND, ASTERISK, LEFTPAREN, RIGHTPAREN,
	UNDERSCORE, GREATER, LESS, QUESTION, QUOTEDBL, COLON,

	// function
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	NONE,
};

const int32 KEY_COUNT = (int32)KeyCode::NONE + 1;

enum class MouseButton : int32
{
	LEFT = 1,
	MIDDLE = 2,
	RIGHT = 3
};

const int32 MOUSE_BUTTONS = 3;

static KeyCode MapSDLKey(unsigned int key)
{
	switch (key)
	{
		case SDLK_a: return KeyCode::A;
		case SDLK_b: return KeyCode::B;
		case SDLK_c: return KeyCode::C;
		case SDLK_d: return KeyCode::D;
		case SDLK_e: return KeyCode::E;
		case SDLK_f: return KeyCode::F;
		case SDLK_g: return KeyCode::G;
		case SDLK_h: return KeyCode::H;
		case SDLK_i: return KeyCode::I;
		case SDLK_j: return KeyCode::J;
		case SDLK_k: return KeyCode::K;
		case SDLK_l: return KeyCode::L;
		case SDLK_m: return KeyCode::M;
		case SDLK_n: return KeyCode::N;
		case SDLK_o: return KeyCode::O;
		case SDLK_p: return KeyCode::P;
		case SDLK_q: return KeyCode::Q;
		case SDLK_r: return KeyCode::R;
		case SDLK_s: return KeyCode::S;
		case SDLK_t: return KeyCode::T;
		case SDLK_u: return KeyCode::U;
		case SDLK_v: return KeyCode::V;
		case SDLK_w: return KeyCode::W;
		case SDLK_x: return KeyCode::X;
		case SDLK_y: return KeyCode::Y;
		case SDLK_z: return KeyCode::Z;
		case SDLK_1: return KeyCode::NUM_1;
		case SDLK_2: return KeyCode::NUM_2;
		case SDLK_3: return KeyCode::NUM_3;
		case SDLK_4: return KeyCode::NUM_4;
		case SDLK_5: return KeyCode::NUM_5;
		case SDLK_6: return KeyCode::NUM_6;
		case SDLK_7: return KeyCode::NUM_7;
		case SDLK_8: return KeyCode::NUM_8;
		case SDLK_9: return KeyCode::NUM_9;
		case SDLK_0: return KeyCode::NUM_0;
		case SDLK_LSHIFT:     return KeyCode::LSHIFT;
		case SDLK_RETURN:     return KeyCode::ENTER;
		case SDLK_ESCAPE:     return KeyCode::ESC;
		case SDLK_DELETE:	  return KeyCode::DELETE_KEY;
		case SDLK_SPACE:      return KeyCode::SPACE;
		case SDLK_KP_PLUS:    return KeyCode::PLUS;
		case SDLK_KP_MINUS:   return KeyCode::MINUS;
		case SDLK_LEFT:       return KeyCode::LEFT;
		case SDLK_RIGHT:      return KeyCode::RIGHT;
		case SDLK_UP:         return KeyCode::UP;
		case SDLK_DOWN:       return KeyCode::DOWN;
		case SDLK_TAB:        return KeyCode::TAB;
		case SDLK_BACKSPACE:  return KeyCode::BACKSPACE;
		case SDLK_EXCLAIM:    return KeyCode::EXCLAM;
		case SDLK_KP_AT:      return KeyCode::AT;
		case SDLK_HASH:       return KeyCode::HASH;
		case SDLK_DOLLAR:     return KeyCode::DOLLAR;
		case SDLK_PERCENT:    return KeyCode::PERCENT;
		case SDLK_CARET:      return KeyCode::CARET;
		case SDLK_AMPERSAND:  return KeyCode::AMPERSAND;
		case SDLK_ASTERISK:   return KeyCode::ASTERISK;
		case SDLK_LEFTPAREN:  return KeyCode::LEFTPAREN;
		case SDLK_RIGHTPAREN: return KeyCode::RIGHTPAREN;
		case SDLK_UNDERSCORE: return KeyCode::UNDERSCORE;
		case SDLK_GREATER:    return KeyCode::GREATER;
		case SDLK_LESS:       return KeyCode::LESS;
		case SDLK_QUESTION:   return KeyCode::QUESTION;
		case SDLK_QUOTEDBL:   return KeyCode::QUOTEDBL;
		case SDLK_COLON:      return KeyCode::COLON;
		case SDLK_F1:         return KeyCode::F1;
		case SDLK_F2:         return KeyCode::F2;
		case SDLK_F3:         return KeyCode::F3;
		case SDLK_F4:         return KeyCode::F4;
		case SDLK_F5:         return KeyCode::F5;
		case SDLK_F6:         return KeyCode::F6;
		case SDLK_F7:         return KeyCode::F7;
		case SDLK_F8:         return KeyCode::F8;
		case SDLK_F9:         return KeyCode::F9;
		case SDLK_F10:        return KeyCode::F10;
		case SDLK_F11:        return KeyCode::F11;
		case SDLK_F12:        return KeyCode::F12;

		default: return KeyCode::NONE;
	}
}

#endif // _INPUT_DEFINITIONS_H