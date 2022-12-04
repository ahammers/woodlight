#include "WLApp_Pacman.h"
#include "WLApp.h"
#include "WLSystem.h"

#include <cstdint>

namespace WoodLight
{
	namespace {
		static const uint8_t Pacman0[] = { 8, 0, 56, 124, 238, 198, 130, 0, 0 };
		static const uint8_t Pacman1[] = { 8, 0, 56, 124, 238, 238, 198, 68, 0 };
		static const uint8_t Pacman2[] = { 8, 0, 56, 124, 238, 238, 238, 108, 40 };
		static const uint8_t Pacman3[] = { 8, 0, 56, 124, 254, 254, 254, 124, 56 };

		static const auto PacmanStates = 7;
		static const uint8_t* PacmanAnimation[PacmanStates] = { Pacman0, Pacman1, Pacman2, Pacman3, Pacman2, Pacman1, Pacman0 };

		class PacmanApp : public App
		{
		public:
			virtual void initialize(System& system) override
			{
				system.display.clear(false);
				for (int x = 3; x < system.display.Columns; x += 10)
				{
					system.display.setPixel(x, system.display.Rows / 2, true);
				}
			}

			virtual void update(System& system) override
			{
				++c;
				if (c % 2 == 0)
				{
					++pacmanX;
					system.display.vline(pacmanX - 1, 0, system.display.Rows, false);
					drawAnimatedPacman(system.display, pacmanX);
				}

				if (pacmanX > system.display.Columns)
					system.nextApp = system.fallbackApp;
			}

		private:
			void drawAnimatedPacman(Display& display, int x)
			{
				pacmanState = (pacmanState + 1) % PacmanStates;

				drawFrame(display, x, 0, PacmanAnimation[pacmanState]);
			}

			void drawFrame(Display& display, int x, int y, const uint8_t* data)
			{
				for (auto w = *(data++); w > 0; --w)
				{
					for (int ry = 0; ry < 8; ++ry)
						display.setPixel(x, y + ry, *data & (1 << ry));

					++data;
					x += 1;
				}
			}

		private:
			int c = 0;
			int pacmanX = 0;
			int pacmanState = 0;
		};
	}

	App* createPacmanApp()
	{
		return new PacmanApp;
	}
}
