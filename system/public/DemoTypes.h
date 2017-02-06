#ifndef _DEMO_TYPES_H
#define _DEMO_TYPES_H

//#include <stdint.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Types

typedef int8_t		int8;
typedef uint8_t		uint8;
typedef int16_t		int16;
typedef uint16_t	uint16;
typedef int32_t		int32;
typedef uint32_t	uint32;
typedef int64_t		int64;
typedef uint64_t	uint64;

// GLM 

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::quat;

struct DSRect
{
	DSRect() :
		left(0), top(0), right(0), bottom(0)
	{}

	DSRect(int32 left, int32 top, int32 right, int32 bottom) :
		left(left), top(top), right(right), bottom(bottom)
	{}

	int32 left, right, top, bottom;
};

#endif // _DEMO_TYPES_H