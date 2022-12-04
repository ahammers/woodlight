#include "WLApp_Clock.h"
#include "WLApp.h"
#include "WLSystem.h"
#include "WLButtons.h"

#include <ctime>
#include <cstring>
#include <stdio.h>

namespace WoodLight
{
	namespace {
		class ClockApp : public App
		{
		public:
			static constexpr auto LeftTimeMode = 0;
			static constexpr auto CenterTimeMode = 1;
			static constexpr auto RightTimeMode = 2;
			static constexpr auto LeftDateMode = 3;
			static constexpr auto CenterDateMode = 4;
			static constexpr auto RightDateMode = 5;
			static constexpr auto WideShortMode = 6;
			static constexpr auto WideShortModeSwapped = 7;
			static constexpr auto LongMode = 8;
			static constexpr auto LongModeSwapped = 9;
			static constexpr auto CenterShortMode = 10;
			static constexpr auto UnixMode = 11;
			static constexpr auto Modes = 12;


		public:
			virtual void initialize(System& system) override
			{
				mode = system.getIntSetting("CLK_MODE", WideShortMode) % Modes;
			}

			virtual void update(System& system) override
			{
				static constexpr auto ShortTimeMask = "%02d:%02d";
				static constexpr auto ShortTimeWidth = 4 * 6 + 3;
				static constexpr auto LongTimeMask = "%02d:%02d:%02d";
				static constexpr auto LongTimeWidth = 6 * 6 + 2 * 3;

				static constexpr auto ShortDateMask = "%02d.%02d.%02d";
				static constexpr auto ShortDateWidth = 6 * 6 + 2 * 3;
				static constexpr auto LongDateMask = "%02d.%02d.%04d";
				static constexpr auto LongDateWidth = 8 * 6 + 2 * 3;

				static constexpr auto CenteredPos = -2;
				static constexpr auto RightPos = -3;

				system.display.reset(false);

				if (!system.isTimeSynchronized())
				{
					system.display.print(0, 0, true, "Uhrzeit nicht synchronisiert");
					return;
				}

				const char* timeMask = nullptr;
				auto timePos = -1;
				const char* dateMask = nullptr;
				auto datePos = -1;
				auto yearMod = 100;
				char unixText[16];
				auto unixPos = -1;
				char buffer[12];

				if (mode == LeftTimeMode || mode == WideShortModeSwapped)
				{
					timeMask = ShortTimeMask;
					timePos = 0;

				} else if (mode == CenterTimeMode)
				{
					timeMask = ShortTimeMask;
					timePos = CenteredPos;

				} else if (mode == RightTimeMode || mode == WideShortMode)
				{
					timeMask = ShortTimeMask;
					timePos = RightPos;

				}

				if (mode == LeftDateMode || mode == WideShortMode)
				{
					dateMask = ShortDateMask;
					datePos = 0;

				} else if (mode == CenterDateMode)
				{
					dateMask = ShortDateMask;
					datePos = CenteredPos;

				} else if (mode == RightDateMode || mode == WideShortModeSwapped)
				{
					dateMask = ShortDateMask;
					datePos = RightPos;
				}

				if (mode == CenterShortMode)
				{
					dateMask = ShortDateMask;
					datePos = (Display::Columns - ShortDateWidth - ShortTimeWidth - 2) / 2;

					timeMask = ShortTimeMask;
					timePos = datePos + ShortDateWidth + 2;

				} else if (mode == LongMode)
				{
					dateMask = LongDateMask;
					datePos = 0;
					yearMod = 10000;
					timeMask = LongTimeMask;
					timePos = RightPos;

				} else if (mode == LongModeSwapped)
				{
					dateMask = LongDateMask;
					datePos = RightPos;
					yearMod = 10000;
					timeMask = LongTimeMask;
					timePos = 0;

				} else if (mode == UnixMode)
				{
					const auto t = time(NULL);
          #ifdef _WIN32
            snprintf(unixText, sizeof(unixText), "%lld", t);
          #else
					  snprintf(unixText, sizeof(unixText), "%d", t);
          #endif
					unixPos = (Display::Columns - Display::getTextWidth(unixText)) / 2;
				}

				if (unixPos >= 0)
					system.display.print(unixPos, 0, true, unixText);

				const auto t = system.getLocalTime();

				if (timeMask != nullptr)
				{
					snprintf(buffer, sizeof(buffer), timeMask, t.hour, t.minute, t.second);

					if (timePos == CenteredPos)
						timePos = (Display::Columns - Display::getTextWidth(buffer)) / 2;
					else if (timePos == RightPos)
						timePos = Display::Columns - Display::getTextWidth(buffer);

					system.display.print(timePos, 0, true, buffer);
				}

				if (dateMask != nullptr)
				{
					snprintf(buffer, sizeof(buffer), dateMask, t.day, t.month, t.year % yearMod);

					if (datePos == CenteredPos)
						datePos = (Display::Columns - Display::getTextWidth(buffer)) / 2;
					else if (datePos == RightPos)
						datePos = Display::Columns - Display::getTextWidth(buffer);

					system.display.print(datePos, 0, true, buffer);
				}

				while (auto key = system.getkey())
				{
					switch (key)
					{
					case System::UpperLeftKeyCode:
						mode = (mode + Modes - 1) % Modes;
						break;
					case System::LowerLeftKeyCode:
						mode = (mode + 1) % Modes;
						break;
					case System::UpperLeftKeyCode | System::LongKeyPress:
					case System::LowerLeftKeyCode | System::LongKeyPress:
						system.setIntSetting("CLK_MODE", mode);
						break;

					case System::UpperRightKeyCode:
						//TODO: Men� + Alarmkonfiguration
						break;
					case System::LowerRightKeyCode:
						system.nextApp = system.fallbackApp;
						break;
					}
				}

			}

		private:
			int mode = 0;
		};
	}

	App* createClockApp()
	{
		return new ClockApp;
	}
}
