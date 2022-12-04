#include "WLScroller.h"

#include "WLSystem.h"

#include <cstring>

namespace WoodLight
{
	static constexpr auto TextWaitBeginning = 25;
	static constexpr auto TextWaitEnd = 25;

	void Scroller::setTextPtr(const char* np)
	{
		if (np != textPtr || Display::getTextWidth(textPtr) != textLen)
		{
			textPtr = np;
			textLen = Display::getTextWidth(textPtr);
			textStart = 0;
			textDir = 1;
			textWait = TextWaitBeginning;
		}
	}

	void Scroller::update(System& system)
	{
		const auto& rect = system.display.getClippingRect();

		const auto disabled = (rect.right - rect.left >= textLen);

		system.display.print(rect.left + (disabled?0:textStart), 0, true, textPtr);

		if (!disabled)
		{
			if (textWait > 0)
			{
				--textWait;
			} else if (textDir > 0)
			{
				textStart -= 0.5;
				const auto textStartLimit = (rect.right - rect.left) - textLen;

				if (textStart <= textStartLimit)
				{
					textStart = textStartLimit;
					textDir = -textDir;
					textWait = TextWaitEnd;
				}
			} else
			{
				const auto step = (textLen - (rect.right - rect.left)) / 25;
				textStart += (step < 3) ? 3 : step;

				const auto textStartLimit = 0;

				if (textStart >= textStartLimit)
				{
					textStart = textStartLimit;
					textDir = -textDir;
					textWait = TextWaitBeginning;
				}
			}
		}
	}
}
