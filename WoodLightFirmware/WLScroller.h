#pragma once

namespace WoodLight
{
	class System;

	class Scroller
	{
	public:
		void setTextPtr(const char*);

		void update(System&);

	private:
		const char* textPtr = nullptr;
		int textLen = 0;
		float textStart = 0;
		int textDir = 1;
		int textWait = 0;
	};
}
