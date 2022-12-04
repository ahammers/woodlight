#include "WLApp.h"

#include "WLSystem.h"
#include "WLMenu.h"
#include "WLButtons.h"

#include "WLApp_Setup.h"
#include "WLApp_Update.h"
#include "WLApp_Clock.h"
#include "WLApp_HomeAssistant.h"
#include "WLApp_Pacman.h"
#include "WLApp_Snake.h"
#include "WLApp_Beep.h"

#include <array>

namespace WoodLight
{
    namespace {
        static constexpr auto SetupIndex = 0;
        static constexpr auto UpdateIndex = 1;
        static constexpr auto ClockIndex = 2;
        static constexpr auto HomeAssistantIndex = 3;
        static constexpr auto PacmanIndex = 4;
        static constexpr auto SnakeIndex = 5;
        static constexpr auto BeepIndex = 6;

        static const char *const MenuItems[] = {
          "Einstellungen",
          "Aktualisierung",
          "Zeit & Datum",
          "Home Assistant",
          "Pacman",
          "Snake",
          "Beep"
        };

        class MenuApp : public App
        {
        public:
            MenuApp()
                : menu(MenuItems, sizeof(MenuItems) / sizeof(*MenuItems), Buttons::Enter)
            {}

            virtual void initialize(System& system) override
            {
            }

            virtual void update(System& system) override
            {
                switch (menu.exec(system))
                {
                case SetupIndex: system.nextApp = createSetupApp(); break;
                case UpdateIndex: system.nextApp = createUpdateApp(); break;
                case ClockIndex: system.nextApp = createClockApp(); break;
                case HomeAssistantIndex: system.nextApp = createHomeAssistantApp(); break;
                case PacmanIndex: system.nextApp = createPacmanApp(); break;
                case SnakeIndex: system.nextApp = createSnakeApp(); break;
                case BeepIndex: system.nextApp = createBeepApp(); break;
                }
            }

        private:
            Menu menu;
            //int currentPos = 0; // Die Position ist itemIndex * 8 + Scanzeile
            //int targetPos = 0;
        };
    }

    App* createMenuApp()
    {
        return new MenuApp;
    }
}
