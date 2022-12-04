#include "WLSystem.h"

#include "WLApp.h"

#include "WLApp_Menu.h"
#include "WLApp_Clock.h"
#include "WLApp_HomeAssistant.h"

#ifdef _WIN32
#include "SoundEffect.h"
#endif

#include <Preferences.h>
#include <WiFi.h>

#ifndef _WIN32
#include <HTTPClient.h>
#endif

#include "WLApp_Update.h"
#include "WLApp_Message.h"
#include "WLHttp.h"

namespace WoodLight
{
	namespace {
		int getKeyMask(int keyCode)
		{
			return (1 << keyCode);
		}
	}

	System::System()
	{
	}

	void System::initialize()
	{
		keyTones = getIntSetting("SYSKEYTONE", 1) != 0;
		updateHandling = getIntSetting("SYSUPDATE", AskForUpdates) != 0;

		wifiRestart();

		fallbackApp = WoodLight::createMenuApp();

		const auto startApp = getStringSetting("STARTAPP", "Menu");
		if (startApp == "Clock")
			currentApp = WoodLight::createClockApp();
		else if (startApp == "HomeAssistant")
			currentApp = WoodLight::createHomeAssistantApp();
		else
			currentApp = fallbackApp;

		currentApp->initialize(*this);

	}

	void System::keyPressed(uint8_t keyCode)
	{
		const auto keyMask = getKeyMask(keyCode);
		if ((keyState & keyMask) == 0)
		{
			keyState |= keyMask;
			if (!(currentApp && currentApp->supportsLongKeyPresses))
			{
				if (keyTones && toneBufferWriteIndex == toneBufferReadIndex)
					playTone({ NOTE_C5, 1 });
				putkey(keyCode);
			}
		}
	}

	void System::keyRelease(uint8_t keyCode)
	{
		const auto keyMask = getKeyMask(keyCode);
		if (keyState & keyMask)
		{
			keyState &= ~keyMask;
			if (currentApp && currentApp->supportsLongKeyPresses && keyHold[keyCode] != 255)
			{
				if (keyTones && toneBufferWriteIndex == toneBufferReadIndex)
					playTone({ NOTE_C5, 1 });
				putkey(keyCode);
			}
		}
	}

	void System::process()
	{
		++cycleCount;

		installUpdateFlag = false;

		if (currentApp != fallbackApp)
		{
			if ((keyState & (UpperLeftKeyMask | LowerLeftKeyMask)) &&
				(keyState & (UpperRightKeyMask | LowerRightKeyMask)))
			{
				std::fill(keyHold.begin(), keyHold.end(), 255);

				delete currentApp;
				currentApp = nullptr;
				nextApp = fallbackApp;

				if (pausedApp)
				{
					delete pausedApp;
					pausedApp = nullptr;
				}
			}
		}

		if (currentApp)
			currentApp->update(*this);

		if (nextApp)
		{
			if (currentApp != fallbackApp)
				delete currentApp;

			// Wenn eine App pausiert wurde dann reaktivieren wir diese anstatt der fallbackApp
			if (nextApp == fallbackApp && pausedApp != nullptr)
				nextApp = pausedApp;

			currentApp = nextApp;
			nextApp = 0;

			keyBufferWriteIndex = keyBufferReadIndex = 0;
			display.clearClippingRect();
			resetPlayback();

			if (keyTones)
				playTone({ NOTE_C4, 1 });

			currentApp->initialize(*this);
		}

		if (currentApp && currentApp->supportsLongKeyPresses)
		{
			for (auto keyCode = 1; keyCode < 5; ++keyCode)
			{
				const auto mask = getKeyMask(keyCode);
				if (keyState & mask)
				{
					if (keyHold[keyCode] < 255)
					{
						++keyHold[keyCode];
						if (keyHold[keyCode] >= LongKeyPressThreshold)
						{
							putkey(keyCode | LongKeyPress);
							keyHold[keyCode] = 255;

							if (keyTones && toneBufferWriteIndex == toneBufferReadIndex)
								playTone({ NOTE_G3, 3 });
						}
					}
				} else
					keyHold[keyCode] = 0;
			}
		}


	}

	void System::putkey(uint8_t newKeyCode)
	{
		if ((keyBufferWriteIndex + 1) % keyBuffer.size() != keyBufferReadIndex)
		{
			keyBuffer[keyBufferWriteIndex] = newKeyCode;
			keyBufferWriteIndex = (keyBufferWriteIndex + 1) % keyBuffer.size();
			lastInputTimestamp = getMillisecondTimestamp();
		}
	}

	uint8_t System::getkey()
	{
		auto result = 0;

		if (keyBufferReadIndex != keyBufferWriteIndex)
		{
			result = keyBuffer[keyBufferReadIndex];
			keyBufferReadIndex = (keyBufferReadIndex + 1) % keyBuffer.size();
		}

		return result;
	}

	bool System::getKeyTones() const
	{
		return keyTones;
	}

	uint32_t System::getSecondsSinceLastInput()
	{
		return getMillisecondDelay(lastInputTimestamp) / 1000;
	}

	int System::getUpdateHandling() const
	{
		return updateHandling;
	}

	void System::playTone(const Tone& tone)
	{
		playTones(&tone, 1);
	}

	void System::playTones(const Tone* tones, size_t count)
	{
		for (size_t i = 0; i < count && tones[i].freq != END_OF_NOTES; ++i)
		{
			// Abbruch wenn der Puffer voll ist
			if ((toneBufferWriteIndex + 1) % toneBuffer.size() == toneBufferReadIndex)
				return;

			toneBuffer[toneBufferWriteIndex] = tones[i];
			toneBufferWriteIndex = (toneBufferWriteIndex + 1) % toneBuffer.size();
		}
	}

	void System::playNote(const Note& note, int wholeNoteDuration)
	{
		if ((toneBufferWriteIndex + 1) % toneBuffer.size() != toneBufferReadIndex)
		{
			const auto duration = (note.type > 0)
				? wholeNoteDuration / note.type
				: wholeNoteDuration * -3 / note.type / 2;

			toneBuffer[toneBufferWriteIndex].freq = note.freq;
			toneBuffer[toneBufferWriteIndex].duration = duration;
			toneBufferWriteIndex = (toneBufferWriteIndex + 1) % toneBuffer.size();
		}
	}

	void System::playNotes(const Note* notes, int wholeNoteDuration)
	{
		for (; notes && notes->freq != END_OF_NOTES; ++notes)
			playNote(*notes, wholeNoteDuration);
	}

	void System::resetPlayback()
	{
		toneBufferReadIndex = toneBufferWriteIndex;
	}

	Tone System::getTone()
	{
		auto result = Tone{ 0, 0 };
		if (toneBufferReadIndex != toneBufferWriteIndex)
		{
			result = toneBuffer[toneBufferReadIndex];
			toneBufferReadIndex = (toneBufferReadIndex + 1) % toneBuffer.size();
		}
		return result;
	}

	String System::getStringSetting(const char* name, const char* defaultValue) const
	{
		Preferences preferences;
		preferences.begin("settings", true);
		const auto result = preferences.getString(name, defaultValue);
		preferences.end();

		return result;
	}

	void System::setStringSetting(const char* name, const char* value)
	{
		Preferences preferences;
		preferences.begin("settings", false);
		preferences.putString(name, value);
		preferences.end();

		if (!strncmp(name,"WIFI", 4))
			wifiRestart();

		if (currentApp)
			currentApp->settingsChanged(*this, name);
	}

	int System::getIntSetting(const char* name, int defaultValue) const
	{
		Preferences preferences;
		preferences.begin("settings", true);
		const auto result = preferences.getInt(name, defaultValue);
		preferences.end();
		return result;
	}

	void System::setIntSetting(const char* name, int value)
	{
		Preferences preferences;
		preferences.begin("settings", false);
		preferences.putInt(name, value);
		preferences.end();

		if (!strcmp(name, "SYSUPDATE"))
			updateHandling = value;
		else if (!strcmp(name, "SYSKEYTONE"))
			keyTones = value;

		if (currentApp)
			currentApp->settingsChanged(*this, name);
	}

	// Startet die WiFi Verbindung (neu)
	void System::wifiRestart() const
	{
		WiFi.disconnect();
		WiFi.mode(WIFI_STA);

		const auto ssid = getStringSetting("WIFISSID", "");
		const auto pwd = getStringSetting("WIFIPASSWORD", "");

		if (ssid.length() > 0)
			WiFi.begin(ssid.c_str(), pwd.c_str());
	}

	bool System::wifiAvailable() const
	{
		return wifiStatus() == WL_CONNECTED;
	}

	// Liefert den WiFi Status wie WiFi.status()
	int System::wifiStatus() const
	{
		return WiFi.status();
	}

	// Liefert zurück ob time() eine gültige Zeit liefert
	bool System::isTimeSynchronized() const
	{
		return time(NULL) >= 660069179;
	}

	System::Time System::getLocalTime() const
	{
#ifdef _WIN32
		struct tm tm;
		const auto t = time(NULL);
		localtime_s(&tm, &t);
#else
		struct tm tm;
 		::getLocalTime(&tm);
#endif
		auto result = System::Time{};
		result.year = tm.tm_year + 1900;
		result.month = tm.tm_mon + 1;
		result.day = tm.tm_mday;
		result.hour = tm.tm_hour;
		result.minute = tm.tm_min;
		result.second = tm.tm_sec;
		return result;
	}

	// Wird vom Basiscode benutzt um Informationen über die Firmwareprüfung zu veröffentlichen
	void System::reportUpdateInfo(const UpdateInfo& newUpdateInfo)
	{
		updateInfo = newUpdateInfo;

		if (updateInfo.status == UpdateInfo::UpdateAvailableStatus && getUpdateHandling() == AskForUpdates && (currentApp->flags & App::DoesUpdateHandlingFlag) == 0)
			nextApp = createUpdateApp();
	}

	const System::UpdateInfo& System::getUpdateInfo() const
	{
		return updateInfo;
	}

	void System::installUpdate()
	{
		installUpdateFlag = true;
	}

	bool System::shouldInstallUpdate() const
	{
		return installUpdateFlag || (updateInfo.status == UpdateInfo::UpdateAvailableStatus && getUpdateHandling() == System::InstallUpdates);
	}

	// Fügt eine Meldung in den Meldungspuffer ein (und bringt das System dazu die erste Meldung anzuzeigen)
	void System::addMessage(Message msg)
	{
		if ((messageWriteIndex + 1) % messageBuffer.size() != messageReadIndex)
		{
			// Wenn die erste Meldung erzeugt wird und gerade keine App pausiert ist, dann starten wir die Meldungausgabeapp
			if (messageWriteIndex == messageReadIndex && pausedApp == nullptr && (currentApp == nullptr || (currentApp->flags & App::DoesMessageHandlingFlag) == 0))
			{
				pausedApp = currentApp;
				currentApp = nullptr;
				nextApp = createMessageApp();
			}

			messageBuffer[messageWriteIndex] = std::move(msg);
			messageWriteIndex = (messageWriteIndex + 1) % messageBuffer.size();
		}
	}

	// Liefert einen Zeiger auf die älteste Meldung oder nullptr wenn keine vorliegt
	const System::Message* System::getMessage() const
	{
		return (messageReadIndex != messageWriteIndex)
			? &messageBuffer[messageReadIndex]
			: nullptr;
	}

	// Entfernt die älteste Meldung aus der Liste
	void System::dropMessage()
	{
		if (messageReadIndex != messageWriteIndex)
		{
			messageReadIndex = (messageReadIndex + 1) % messageBuffer.size();
		}
	}

	// Liefert einen Millisekundenzeitstempel (aus millis())
	uint32_t System::getMillisecondTimestamp()
	{
#ifdef _WIN32
		static_assert(CLOCKS_PER_SEC == 1000);
		return static_cast<uint32_t>(clock());
#else
		return uint32_t{ millis() };
#endif
	}

	// Liefert die Zeit in Millisekunden die seit dem angegebenen Zeitstempel vergangen ist
	uint32_t System::getMillisecondDelay(uint32_t millisecondTimestamp)
	{
		return static_cast<uint32_t>(getMillisecondTimestamp() - millisecondTimestamp);

	}

	int System::httpGet(const char* url, const char* auth, String* contentTarget)
	{
		return WoodLight::httpGet(url, auth, contentTarget);
	}

}
