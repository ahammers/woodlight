#include "WLApp_Beep.h"
#include "WLApp.h"
#include "WLSystem.h"
#include "WLMenu.h"
#include "WLBeeps.h"

namespace WoodLight
{
	namespace {
		static constexpr auto TestSongIndex = 0;
		static constexpr auto DoomSongIndex = 1;
		static constexpr auto ImperialMarchSongIndex = 2;
		static constexpr auto MerryChristmasSongIndex = 3;
		static constexpr auto PacmanSongIndex = 4;
		static constexpr auto SuperMarioBrosSongIndex = 5;
		static constexpr auto TetrisThemeSongIndex = 6;
		static constexpr auto ZeldaThemeSongIndex = 7;
		static constexpr auto NokiaSongIndex = 8;

		static const char *const MenuItems[] = {
		  "Test",
		  "Imperial March",
		  "Merry Christmas",
		  "Pacman",
		  "Super Mario Bros",
		  "Tetris",
		  "Zelda",
		  "Nokia"
		};

		class BeepApp : public App
		{
		public:
			BeepApp()
				: menu(MenuItems, sizeof(MenuItems) / sizeof(*MenuItems), Buttons::OkBack)
			{}

			virtual void initialize(System& system) override
			{
				
			}

			virtual void update(System& system) override
			{
				switch (menu.exec(system))
				{
				case TestSongIndex: play(system, getTestSong()); break;
				case DoomSongIndex: play(system, getDoomSong()); break;
				case ImperialMarchSongIndex: play(system, getImperialMarchSong()); break;
				case MerryChristmasSongIndex: play(system, getMerryChristmasSong()); break;
				case PacmanSongIndex: play(system, getPacmanSong()); break;
				case SuperMarioBrosSongIndex: play(system, getSuperMarioBrosSong()); break;
				case TetrisThemeSongIndex: play(system, getTetrisThemeSong()); break;
				case ZeldaThemeSongIndex: play(system, getZeldaThemeSong()); break;
				case NokiaSongIndex: play(system, getNokiaSong()); break;

				case Menu::BackIndex:
					system.nextApp = system.fallbackApp;
					break;
				}
			}

		private:
			void play(System& system, Song song)
			{
				system.playNotes(song.notes, song.speed);
			}

		private:
			Menu menu;
		};
	}

	App* createBeepApp()
	{
		return new BeepApp;
	}
}
