#include "WLApp_Update.h"
#include "WLApp.h"
#include "WLSystem.h"
#include "WLButtons.h"
#include "WLScroller.h"

#include <WiFi.h>

namespace WoodLight
{
	namespace 
	{
		class UpdateApp : public App
		{

		public:
			UpdateApp()
				: App(App::DoesUpdateHandlingFlag)
			{
			}

			virtual void initialize(System& system) override
			{
				testCycle = (system.cycleCount + 1) % 1024;
			}

			virtual void update(System& system) override
			{
				system.display.clear(false);

				const auto& info = system.getUpdateInfo();
				auto canUpdate = false;

				switch (info.status)
				{
				case System::UpdateInfo::UndefinedStatus:
					if (system.getUpdateHandling() == System::IgnoreUpdates)
						scroller.setTextPtr("Keine Informationen da Updates in den Einstellungen auf \"Ignorieren\" gesetzt sind.");
					else
						scroller.setTextPtr("Noch keine Update Informationen verfügbar");
					break;

				case System::UpdateInfo::CheckFailedStatus:
					snprintf(scrollText, sizeof(scrollText), "Prüfung fehlgeschlagen: %s (code %d)", info.stringBuffer, info.errorCode);
					scroller.setTextPtr(scrollText);
					break;

				case System::UpdateInfo::UpToDateStatus:
					scroller.setTextPtr("Keine neue Software verfügbar");
					break;

				case System::UpdateInfo::UpdateAvailableStatus:
					snprintf(scrollText, sizeof(scrollText), "Software Version %d vom %s verfügbar. Aktualisieren?", info.newVersion, info.newDate);
					scroller.setTextPtr(scrollText);
					canUpdate = true;
					break;
				}

				if (canUpdate)
					drawButtons(system, Buttons::Cancel, Buttons::OkCancel);

				while (auto ch = system.getkey())
				{
					if (ch == System::UpperRightKeyCode && canUpdate)
						system.installUpdate();
					else if (ch == System::UpperRightKeyCode || ch == System::LowerRightKeyCode || ch == System::UpperLeftKeyCode || ch == System::LowerLeftKeyCode)
						system.nextApp = system.fallbackApp;
				}

				scroller.update(system);
			}

		private:
			char scrollText[128];

			Scroller scroller;
			int testCycle = -1;
			int updateVersionNumber = 0;
		};
	}

	App* createUpdateApp()
	{
		return new UpdateApp;
	}


}

