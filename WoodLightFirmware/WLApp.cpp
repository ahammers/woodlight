#include "WLApp.h"

namespace WoodLight
{
	App::App(int flags_)
		: flags(flags_)
	{}

	void App::initialize(class System&)
	{}

	// Wird aufgerufen wenn Settings extern geändert werden (zB durch über serielle Kommandos)
	void App::settingsChanged(class System&, const char* const settingName)
	{}

}
