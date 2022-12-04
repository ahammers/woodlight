#include "WLApp.h"

#include <array>

#include "WLSystem.h"
#include "WLMenu.h"
#include "WLButtons.h"
#include "WLInput.h"
#include "WLAppExt.h"

#include "WLApp_Clock.h"
#include "WLApp_HomeAssistant.h"
#include "WLApp_Pacman.h"
#include "WLApp_Snake.h"

#include <WiFi.h>

namespace WoodLight
{
    namespace {

        static constexpr auto WiFiSSIDAllowedCharacters = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz !#%&'()*,-./0123456789:;<=>@^_`{|}~";
        static constexpr auto WiFiPasswordAllowedCharacters = WiFiSSIDAllowedCharacters;

        static constexpr auto WiFiSSIDIndex = 0;
        static constexpr auto WiFiPasswordIndex = 1;
        static constexpr auto WiFiStatusIndex = 2;
        static constexpr auto NTPServerIndex = 3;
        static constexpr auto GMTOffsetIndex = 4;
        static constexpr auto DSOffsetIndex = 5;
        static constexpr auto StartAnimationIndex = 6;
        static constexpr auto StartApplicationIndex = 7;
        static constexpr auto KeyTonesIndex = 8;
        static constexpr auto SearchForUpdatesIndex = 9;

        static const char* const MenuItems[] = {
          "WiFi SSID",
          "WiFi Kennwort",
          "WiFi Status",
          "NTP Zeitserver",
          "GMT Offset in Sek.",
          "Sommerzeitoffset",
          "Startanimation",
          "Startapplikation",
          "Tastentöne",
          "Nach Updates suchen"
        };

        static const char* const StartAnimationItems[] = {
            "None"
        };

        static const char* const StartAppItems[] = {
            "Menu",
            "Clock",
            "HomeAssistant",
        };

        static const char* const OnOffItems[] = {
            "Aus",
            "Ein"
        };

        static const char *const UpdateItems[] = { 
            "Manuell", // Ignore Updates
            "Suchen & Fragen",  // AskForUpdates
            "Autom. Installieren"   // InstallUpdates
        };

        class SetupApp : public App, public EditorExt
        {
        public:
            SetupApp()
                : menu(MenuItems, sizeof(MenuItems) / sizeof(*MenuItems), Buttons::OkHome),
                currentFunction(&SetupApp::menuFunction)
            {}

        public: // App
            virtual void initialize(System& system) override
            {
                system.display.clear(false);
            }

            virtual void update(System& system) override
            {
                ++currentFunctionCallCount;
                (this->*currentFunction)(system);
            }

        public: // EditorExt
            virtual void setCurrentFunction(void (EditorExt::* f)(System& system)) override
            {
                currentFunction = f;
            }

            virtual void resetCurrentFunction() override
            {
                currentFunction = &SetupApp::menuFunction;
            }


        private:
            void menuFunction(System& system)
            {
                switch (menu.exec(system))
                {
                case WiFiSSIDIndex:         editStringSetting(system, "SSID:", "WIFISSID", WiFiSSIDAllowedCharacters, "", 1, 32); break;
                case WiFiPasswordIndex:     editStringSetting(system, "PWD:", "WIFIPASSWORD", WiFiPasswordAllowedCharacters, "", 1, 64); break;
                case WiFiStatusIndex:       currentFunction = &SetupApp::WiFiStatusFunction; break;

                case NTPServerIndex:        editStringSetting(system, "NTP:", "NTPSRV1", Input::AllowedUrlCharacters, "europe.pool.ntp.org", 1, 64); break;
                case GMTOffsetIndex:        editIntSetting(system, "Zeitoffset [s]:", "GMTOFFSET", 3600, -12 * 3600, 12 * 3600); break;
                case DSOffsetIndex:         editIntSetting(system, "Sommerzeitoffset:", "DSOFFSET", 3600, -7200, 7200); break;
                case StartAnimationIndex:   editIntEnumSetting(system, "Start: ", "STARTANI", StartAnimationItems, sizeof(StartAnimationItems)/sizeof(*StartAnimationItems), 0); break;
                case StartApplicationIndex: editStringEnumSetting(system, "App: ", "STARTAPP", StartAppItems, sizeof(StartAppItems) / sizeof(*StartAppItems), "Menu"); break;

                case KeyTonesIndex:         editIntEnumSetting(system, "Tastentöne: ", "SYSKEYTONE", OnOffItems, sizeof(OnOffItems) / sizeof(*OnOffItems), 1); break;
                case SearchForUpdatesIndex: editIntEnumSetting(system, "Akt.:", "SYSUPDATE", UpdateItems, sizeof(UpdateItems) / sizeof(*UpdateItems), System::AskForUpdates); break;

                case Menu::HomeIndex:       system.nextApp = system.fallbackApp; break;
                }

                if (currentFunction != &SetupApp::menuFunction)
                    currentFunctionCallCount = 0;
            }

            void WiFiStatusFunction(System& system)
            {
                const char* statusText = "Unknown status";

                switch (system.wifiStatus())
                {
                case WL_IDLE_STATUS: statusText = "im Leerlauf"; break;
                case WL_NO_SSID_AVAIL:  statusText = "keine ssid verfügbar"; break;
                case WL_SCAN_COMPLETED: statusText = "Scan abgeschlossen"; break;
                case WL_CONNECTED: statusText = "verbunden"; break;
                case WL_CONNECT_FAILED:  statusText = "fehlgeschlagen"; break;
                case WL_CONNECTION_LOST: statusText = "abgebrochen"; break;
                case WL_DISCONNECTED: statusText = "getrennt"; break;
                }

                drawButtons(system, Buttons::None, Buttons::Ok);
                system.display.print(system.display.getClippingRect().left, 0, true, statusText);

                if (system.getkey() > 0)
                    currentFunction = &SetupApp::menuFunction;
            }

        private:
            int currentFunctionCallCount = 0;
            void (SetupApp::* currentFunction)(System& system);
            Menu menu, subMenu;
            Input input;
        };
    }

    App* createSetupApp()
    {
        return new SetupApp;
    }
}