#pragma once

#include "WLStr.h"

namespace WoodLight
{
	int httpGet(const char* url, const char* auth, String* contentTarget);

}