#include "WLInput.h"

#include "WLSystem.h"
#include "WLButtons.h"

#include <cassert>

namespace WoodLight
{
	bool Input::isInitialized() const
	{
		return characters != nullptr;
	}

	void Input::initialize(const char* label_, const char* initialValue_, const char* characters_, int minLength_, int maxLength_)
	{
		assert(maxLength_ <= LengthLimit);

		label = label_;
		
		valueLength = initialValue_ ? std::min(maxLength_, static_cast<int>(strlen(initialValue_))) : 0;
		memcpy(value.data(), initialValue_, valueLength);
		value[valueLength] = 0;
		// offsets
		characters = characters_;
		minLength = minLength_;
		maxLength = (maxLength_ > LengthLimit) ? LengthLimit : maxLength_;
		firstPos = 0;
		posIndex = 0;
		charIndex = 0;

		updateOffsets();
	}

	const char * Input::getValue() const
	{
		return value.data();
	}

	int Input::exec(System& system)
	{
		auto result = NoOp;

		auto& display = system.display;

		while (auto ch = system.getkey())
		{
			// Positionsauswahl
			if (posMode)
			{
				switch (ch)
				{
				case System::UpperLeftKeyCode: // Voriges Zeichen
					if (posIndex > 0)
						--posIndex;
					break;
				case System::UpperLeftKeyCode | System::LongKeyPress: // Erstes Zeichen
					posIndex = 0;
					break;
				case System::LowerLeftKeyCode: // N�chstes Zeichen
					if (posIndex < valueLength)
						++posIndex;
					break;
				case System::LowerLeftKeyCode | System::LongKeyPress: // Letztes Zeichen
					posIndex = valueLength;
					break;

				case System::UpperRightKeyCode: // Zeichen w�hlen
					if (valueLength < maxLength)
					{
						posMode = false;
						updateOffsets();
					}
					break;
				case System::UpperRightKeyCode | System::LongKeyPress: // Best�tigen
					result = Accept;
					break;

				case System::LowerRightKeyCode: // Zeichen links vom Cursor l�schen
					if (posIndex > 0)
					{
						memmove(value.data() + posIndex - 1, value.data() + posIndex, valueLength - posIndex + 1);
						--valueLength;
						//value[valueLength] = 0;
						posIndex -= 1;
						updateOffsets();
					}
					break;

				case System::LowerRightKeyCode | System::LongKeyPress: // Abbrechen
					result = Cancel;
					break;
				}

			// Zeichenauswahl
			} else
			{
				switch (ch)
				{
				case System::UpperLeftKeyCode: // Voriges Zeichen
					if (charIndex > 0)
						--charIndex;
					break;
				case System::UpperLeftKeyCode | System::LongKeyPress: // Erstes Zeichen
					charIndex = 0;
					break;
				case System::LowerLeftKeyCode: // N�chstes Zeichen
					if (charIndex + 1 < strlen(characters))
						++charIndex;
					break;
				case System::LowerLeftKeyCode | System::LongKeyPress: // Letztes Zeichen
					charIndex = strlen(characters)-1;
					break;

				case System::UpperRightKeyCode: // Zeichen w�hlen
					memmove(value.data() + posIndex + 1, value.data() + posIndex, valueLength - posIndex + 1);
					value[posIndex] = characters[charIndex];
					valueLength = valueLength + 1;
					posIndex = posIndex + 1;
					posMode = true;
					updateOffsets();

					break;
				//case System::UpperRightKeyCode | System::LongKeyPress: // Best�tigen

				case System::LowerRightKeyCode: // Abbrechen
					posMode = true;
					updateOffsets();
					break;

				//case System::LowerRightKeyCode | System::LongKeyPress: // Abbrechen
				}
			}
		}

		// Schnelle Navigation
		{
			if (system.keyHold[System::UpperLeftKeyCode] >= 8)
			{
				system.keyHold[System::UpperLeftKeyCode] = 7;
				if (posMode)
				{
					if (posIndex > 0)
						--posIndex;
				} else
				{
					if (charIndex > 0)
						--charIndex;
				}
			}

			if (system.keyHold[System::LowerLeftKeyCode] >= 8)
			{
				system.keyHold[System::LowerLeftKeyCode] = 7;
				if (posMode)
				{
					if (posIndex < valueLength)
						++posIndex;
				} else
				{
					if (charIndex + 1 < strlen(characters))
						++charIndex;
				}
			}
		}

		// Positionsauswahl
		if (posMode)
		{
			display.reset(false);
			drawButtons(system, Buttons::LeftRight, Buttons::EnterDelete);

			const auto nextX = display.print(display.getClippingRect().left, 0, true, label);

			// links mehr einblenden
			if (posIndex < firstPos)
				firstPos = posIndex;

			// rechts mehr einblenden
			else if (nextX + 1 + offsets[posIndex] - offsets[firstPos] > display.getClippingRect().right)
				while (nextX + 1 + offsets[posIndex] - offsets[firstPos] > display.getClippingRect().right)
					++firstPos;

			//const auto cursorPos = display.getTextWidth(value.data(), posIndex);
			display.print(nextX + 1, 0, true, value.data() + firstPos);

			if (system.cycleCount % 16 < 8)
				display.hline(nextX + 1 + offsets[posIndex] - offsets[firstPos] - 1, 7, offsets[posIndex + 1] - offsets[posIndex] + 1, true);

		// Zeichenauswahl
		} else
		{
			display.reset(false);
			drawButtons(system, Buttons::LeftRight, Buttons::OkBack);

			const auto nextX = display.getClippingRect().left;

			// links mehr einblenden
			if (nextX + 1 + offsets[charIndex] - offsets[firstChar] < (display.getClippingRect().left + display.getClippingRect().right) / 2 - 5)
				while (firstChar > 0 && nextX + 1 + offsets[charIndex] - offsets[firstChar] < (display.getClippingRect().left + display.getClippingRect().right) / 2 - 5)
					--firstChar;

			// rechts mehr einblenden
			else if (nextX + 1 + offsets[charIndex] - offsets[firstChar] > (display.getClippingRect().left + display.getClippingRect().right) / 2 + 5)
				while (nextX + 1 + offsets[charIndex] - offsets[firstChar] > (display.getClippingRect().left + display.getClippingRect().right) / 2 + 5)
					++firstChar;

			//const auto cursorChar = display.getTextWidth(value.data(), charIndex);
			display.print(nextX + 1, 0, true, characters + firstChar);

			if (system.cycleCount % 16 < 8)
				display.hline(nextX + 1 + offsets[charIndex] - offsets[firstChar] - 1, 7, offsets[charIndex + 1] - offsets[charIndex] + 1, true);

		}

		// Bei einem Ergebnis setzen wir die Instanz wieder auf "nicht initialisiert"
		if (result != NoOp)
			characters = nullptr;

		return result;
	}

	void Input::updateOffsets()
	{
		offsets[0] = 0;
		if (posMode)
			for (auto i = 0; i < valueLength; ++i)
				offsets[i + 1] = offsets[i] + Display::getTextWidth(value[i]);
		else
			for (auto i = 0; characters[i]; ++i)
				offsets[i + 1] = offsets[i] + Display::getTextWidth(characters[i]);
	}

}
