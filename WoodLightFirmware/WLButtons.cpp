#include "WLButtons.h"

#include "WLSystem.h"

namespace WoodLight
{
	namespace {
		int getWidth(Buttons b)
		{
			switch (b)
			{
			case Buttons::None: return 0;
			case Buttons::UpDown: return 3;
			case Buttons::LeftRight: return 2;
			case Buttons::OkBack: return 4;
			case Buttons::SetupBack: return 4;
			case Buttons::OkHome: return 4;
			case Buttons::Enter: return 4;
			case Buttons::EnterDelete: return 4;
			case Buttons::Ok: return 5;
			case Buttons::Cancel: return 5;
			case Buttons::OkCancel: return 4;
			default:
        #ifdef _WIN32
				_CrtDbgBreak();
        #endif
				return 5;
			}
		}

		void drawButtons(Display& display, int x, Buttons b)
		{
			switch (b)
			{
			case Buttons::None: break;

			case Buttons::UpDown:
				display.setCol(x+0, 0b01100110);
				display.setCol(x+1, 0b10000001);
				display.setCol(x+2, 0b01100110);
				break;

			case Buttons::LeftRight:
				display.setCol(x + 0, 0b10100010);
				display.setCol(x + 1, 0b01000101);
				break;

			case Buttons::SetupBack:
				display.setCol(x + 0, 0b01000000);
				display.setCol(x + 1, 0b11100010);
				display.setCol(x + 2, 0b01000101);
				display.setCol(x + 3, 0b01000010);
				break;

			case Buttons::OkBack:
				display.setCol(x + 0, 0b01000010);
				display.setCol(x + 1, 0b11100100);
				display.setCol(x + 2, 0b01000010);
				display.setCol(x + 3, 0b01000001);
				break;

			case Buttons::OkHome:
				display.setCol(x + 0, 0b00000010);
				display.setCol(x + 1, 0b11000100);
				display.setCol(x + 2, 0b10100010);
				display.setCol(x + 3, 0b11000001);
				break;

			case Buttons::Enter:
				display.setCol(x + 0, 0b00010000);
				display.setCol(x + 1, 0b00111000);
				display.setCol(x + 2, 0b00010000);
				display.setCol(x + 3, 0b00011111);
				break;

			case Buttons::EnterDelete:
				display.setCol(x + 0, 0b00000000);
				display.setCol(x + 1, 0b01000010);
				display.setCol(x + 2, 0b10100111);
				display.setCol(x + 3, 0b11100010);
				break;

			case Buttons::Ok: 
				display.setCol(x + 0, 0b00010000);
				display.setCol(x + 1, 0b00100000);
				display.setCol(x + 2, 0b00010000);
				display.setCol(x + 3, 0b00001000);
				display.setCol(x + 4, 0b00000100);
				break;

			case Buttons::Cancel: 
				display.setCol(x + 0, 0b01000100);
				display.setCol(x + 1, 0b00101000);
				display.setCol(x + 2, 0b00010000);
				display.setCol(x + 3, 0b00101000);
				display.setCol(x + 4, 0b01000100);
				break;

			case Buttons::OkCancel:
				display.setCol(x + 0, 0b00000010);
				display.setCol(x + 1, 0b10100100);
				display.setCol(x + 2, 0b01000010);
				display.setCol(x + 3, 0b10100001);
				break;


			}
		}
	}

	void drawButtons(System& system, Buttons left, Buttons right, int flags)
	{
		auto& display = system.display;

		auto leftWidth = getWidth(left);
		auto rightWidth = getWidth(right);

		display.clearClippingRect();
		if ((flags & (DrawButtons::HideAfterTimeout | DrawButtons::BlackAfterTimeout)) == 0 || system.getSecondsSinceLastInput() < 10)
		{
			drawButtons(display, 0, left);
			drawButtons(display, Display::Columns - rightWidth, right);
		} else if (flags & DrawButtons::BlackAfterTimeout)
		{
			for (auto x = 0; x < leftWidth; ++x)
				display.setCol(x, 0);
			for (auto x = Display::Columns - rightWidth; x < Display::Columns; ++x)
				display.setCol(x, 0);
		} else
		{
			leftWidth = rightWidth = 0;
		}

		if ((flags & DrawButtons::KeepCenter) == 0)
			for (int x = leftWidth; x < Display::Columns - rightWidth; ++x)
				display.setCol(x, 0);

		display.setClippingRect(leftWidth + 1, 0, Display::Columns - rightWidth - leftWidth - 2, Display::Rows);

		
	}
}
