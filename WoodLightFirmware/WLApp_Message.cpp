#include "WLApp_Message.h"
#include "WLApp.h"
#include "WLSystem.h"
#include "WLScroller.h"
#include "WLButtons.h"

#include <ctime>
#include <cstring>
#include <stdio.h>

namespace WoodLight
{
	namespace {
		class MessageApp : public App
		{
		public:
			MessageApp()
				: App(App::DoesMessageHandlingFlag)
			{}

			virtual void initialize(System& system) override
			{

			}

			virtual void update(System& system) override
			{
				if (auto msg = system.getMessage())
				{
					scroller.setTextPtr(msg->c_str());

					drawButtons(system, Buttons::None, Buttons::Ok);

					auto drop = false;
					while (auto key = system.getkey())
					{
						if (key == System::UpperRightKeyCode || key == System::LowerRightKeyCode)
							drop = true;
					}

					if (drop)
						system.dropMessage();

				} else
				{
					system.nextApp = system.fallbackApp;
				}
			}

		private:
			Scroller scroller;
		};
	}

	App* createMessageApp()
	{
		return new MessageApp;
	}
}
