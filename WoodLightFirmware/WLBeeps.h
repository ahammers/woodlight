#pragma once

#include "WLSystem.h"

namespace WoodLight
{
	class Song
	{
  public:
    Song(const Note*const, const int);
    
	public:
		const Note *const notes= nullptr;
		const int speed = 0;
	};
	
	Song getTestSong();
	Song getDoomSong();
	Song getImperialMarchSong();
	Song getMerryChristmasSong();
	Song getPacmanSong();
	Song getSuperMarioBrosSong();
	Song getTetrisThemeSong();
	Song getZeldaThemeSong();
	Song getNokiaSong();
}
