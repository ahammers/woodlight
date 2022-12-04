#include "WLStr.h"

#ifdef _WIN32

String::String(int32_t v)
	: std::string(std::to_string(v))
{}

void String::remove(size_t index, size_t count)
{
	erase(index, count);
}
#endif
