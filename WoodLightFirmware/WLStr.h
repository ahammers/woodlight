#pragma once

#ifdef _WIN32
#include <string>
#else
#include <Arduino.h>
#endif

#ifdef _WIN32
class String : public std::string
{
public:
	using std::string::basic_string;

	String(int32_t);

	//bool equals(const String& b) const;
	//bool equalsIgnoreCase(const String& b) const;
	//size_t length(const) const;
	//size_t indexOf(char, size_t start = 0) const;
	//size_t lastIndexOf(char) const;
	//size_t lastIndexOf(char, size_t from = 2000000000) const;
	void remove(size_t index, size_t count = 1);
	//void replace(const String& substr1, const String& substr2);
	//void setCharAt(size_t index char);
	//bool startsWith(const String&) const;
	//bool endsWith(const String&) const;
	//String substring(size_t begin, size_t end) const;
};
#endif
