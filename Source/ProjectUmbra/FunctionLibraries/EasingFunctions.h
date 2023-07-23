#pragma once

#include "CoreMinimal.h"

class EasingFunctions
{
public:
	static float EaseInSine(float _fProgress)
	{
		return 1 - cos((_fProgress * PI) / 2);
	}
};
