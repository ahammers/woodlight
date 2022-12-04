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


/**

   httpUpdate.ino

	Created on: 27.11.2015


#include <Arduino.h>

#include <WiFi.h>
//#include <WiFiMulti.h>

#include <HTTPClient.h>
#include <HTTPUpdate.h>

const char* ssid = "L42 OG 24";
const char* password = "3280056957989284";

void setup_wifi()
{
	Serial.print("Running HTTPUpdate version ");

	delay(10);
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.print("Connected as ");
	Serial.println(WiFi.localIP());
}

void setup() {
	Serial.begin(115200);
	setup_wifi();
}

void update_started() {
	Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
	Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
	Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
	Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void loop() {

	// wait for WiFi connection
	if (WiFi.status() == WL_CONNECTED)
	{
		WiFiClient client;

		// The line below is optional. It can be used to blink the LED on the board during flashing
		// The LED will be on during download of one buffer of data from the network. The LED will
		// be off during writing that buffer to flash
		// On a good connection the LED should flash regularly. On a bad connection the LED will be
		// on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
		// value is used to put the LED on. If the LED is on with HIGH, that value should be passed
		// httpUpdate.setLedPin(LED_BUILTIN, LOW);

		httpUpdate.onStart(update_started);
		httpUpdate.onEnd(update_finished);
		httpUpdate.onProgress(update_progress);
		httpUpdate.onError(update_error);

		t_httpUpdate_return ret = httpUpdate.update(client, "http://www.ecblink.de:80/fota/bin/esp32fota_test_221128.bin");

		switch (ret) {
		case HTTP_UPDATE_FAILED:
			Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
			break;

		case HTTP_UPDATE_NO_UPDATES:
			Serial.println("HTTP_UPDATE_NO_UPDATES");
			break;

		case HTTP_UPDATE_OK:
			Serial.println("HTTP_UPDATE_OK");
			break;
		}
	}
}

**/
