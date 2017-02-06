#ifndef _DS_BIT_H
#define _DS_BIT_H

#include "DemoTypes.h"
#include <intrin.h>

#pragma intrinsic(_BitScanForward)

namespace Bit
{
	inline uint32 LeastSignifcantBit(uint32 mask)
	{
		unsigned long index;
		_BitScanForward(&index, mask);
		return index;
	}
}

#endif // _DS_BIT_H