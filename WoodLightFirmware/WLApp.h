#pragma once

#include "WLInput.h"

namespace WoodLight
{
	class App
	{
	public:
		static constexpr auto DoesUpdateHandlingFlag = 1;
		static constexpr auto DoesMessageHandlingFlag = 2;

	public:
		App(int flags = 0);

		// Wird beim Start der App aufgerufen
		virtual void initialize(class System&);

		// Wird bei jedem 40ms Zyklus aufgerufen
		virtual void update(class System&) = 0;

		// Wird aufgerufen wenn Settings extern geändert werden (zB durch über serielle Kommandos)
		virtual void settingsChanged(class System&, const char* const settingName);

	public:
		typedef void (App::*currentFunction)(System& system);
		const int flags;
		bool supportsLongKeyPresses = true;
	};

}
