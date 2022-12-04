#pragma once

#ifdef _WIN32
	#include "WLStr.h"
#else
	#include <Arduino.h>
#endif

#include <array>

namespace WoodLight
{
	class System;

	class Input
	{
	public:
		static constexpr auto NoOp = 0;
		static constexpr auto Accept = 1;
		static constexpr auto Cancel = 2;

		static constexpr auto LengthLimit = 128;

		static constexpr auto AllowedUrlCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.:|/%&=?-*+$_~[](),;#'!";
		static constexpr auto AllowedDecimalCharacters = "0123456789";

	public:
		bool isInitialized() const;

		void initialize(const char *label, const char *initialValue, const char* characters, int minLength, int maxLength);

		const char *getValue() const;

		int exec(System&);


	private:
		void updateOffsets();

	private:
		const char * label = nullptr;
		int valueLength = 0;
		std::array<char, LengthLimit + 1> value;
		std::array<uint16_t, LengthLimit + 1> offsets;
		const char* characters = nullptr;
		int minLength = 0;
		int maxLength = 0;
		bool posMode = true;
		
		int firstPos = 0;
		int posIndex = 0;
		
		int firstChar = 0;
		int charIndex = 0;

	};
}
