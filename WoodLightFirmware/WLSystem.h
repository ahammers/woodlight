#pragma once

#include "WLDisplay.h"
#include "WLStr.h"

namespace WoodLight
{
	static constexpr auto PAUSE = int16_t{ 0 };
	static constexpr auto REST = int16_t{ PAUSE };

	static constexpr auto END_OF_NOTES = int16_t{ -32768 };

	static constexpr auto NOTE_B0 = int16_t{ 31 };
	static constexpr auto NOTE_C1 = int16_t{ 33 };
	static constexpr auto NOTE_CS1 = int16_t{ 35 };
	static constexpr auto NOTE_D1 = int16_t{ 37 };
	static constexpr auto NOTE_DS1 = int16_t{ 39 };
	static constexpr auto NOTE_E1 = int16_t{ 41 };
	static constexpr auto NOTE_F1 = int16_t{ 44 };
	static constexpr auto NOTE_FS1 = int16_t{ 46 };
	static constexpr auto NOTE_G1 = int16_t{ 49 };
	static constexpr auto NOTE_GS1 = int16_t{ 52 };
	static constexpr auto NOTE_A1 = int16_t{ 55 };
	static constexpr auto NOTE_AS1 = int16_t{ 58 };
	static constexpr auto NOTE_B1 = int16_t{ 62 };
	static constexpr auto NOTE_C2 = int16_t{ 65 };
	static constexpr auto NOTE_CS2 = int16_t{ 69 };
	static constexpr auto NOTE_D2 = int16_t{ 73 };
	static constexpr auto NOTE_DS2 = int16_t{ 78 };
	static constexpr auto NOTE_E2 = int16_t{ 82 };
	static constexpr auto NOTE_F2 = int16_t{ 87 };
	static constexpr auto NOTE_FS2 = int16_t{ 93 };
	static constexpr auto NOTE_G2 = int16_t{ 98 };
	static constexpr auto NOTE_GS2 = int16_t{ 104 };
	static constexpr auto NOTE_A2 = int16_t{ 110 };
	static constexpr auto NOTE_AS2 = int16_t{ 117 };
	static constexpr auto NOTE_B2 = int16_t{ 123 };
	static constexpr auto NOTE_C3 = int16_t{ 131 };
	static constexpr auto NOTE_CS3 = int16_t{ 139 };
	static constexpr auto NOTE_D3 = int16_t{ 147 };
	static constexpr auto NOTE_DS3 = int16_t{ 156 };
	static constexpr auto NOTE_E3 = int16_t{ 165 };
	static constexpr auto NOTE_F3 = int16_t{ 175 };
	static constexpr auto NOTE_FS3 = int16_t{ 185 };
	static constexpr auto NOTE_G3 = int16_t{ 196 };
	static constexpr auto NOTE_GS3 = int16_t{ 208 };
	static constexpr auto NOTE_A3 = int16_t{ 220 };
	static constexpr auto NOTE_AS3 = int16_t{ 233 };
	static constexpr auto NOTE_B3 = int16_t{ 247 };
	static constexpr auto NOTE_C4 = int16_t{ 262 };
	static constexpr auto NOTE_CS4 = int16_t{ 277 };
	static constexpr auto NOTE_D4 = int16_t{ 294 };
	static constexpr auto NOTE_DS4 = int16_t{ 311 };
	static constexpr auto NOTE_E4 = int16_t{ 330 };
	static constexpr auto NOTE_F4 = int16_t{ 349 };
	static constexpr auto NOTE_FS4 = int16_t{ 370 };
	static constexpr auto NOTE_G4 = int16_t{ 392 };
	static constexpr auto NOTE_GS4 = int16_t{ 415 };
	static constexpr auto NOTE_A4 = int16_t{ 440 };
	static constexpr auto NOTE_AS4 = int16_t{ 466 };
	static constexpr auto NOTE_B4 = int16_t{ 494 };
	static constexpr auto NOTE_C5 = int16_t{ 523 };
	static constexpr auto NOTE_CS5 = int16_t{ 554 };
	static constexpr auto NOTE_D5 = int16_t{ 587 };
	static constexpr auto NOTE_DS5 = int16_t{ 622 };
	static constexpr auto NOTE_E5 = int16_t{ 659 };
	static constexpr auto NOTE_F5 = int16_t{ 698 };
	static constexpr auto NOTE_FS5 = int16_t{ 740 };
	static constexpr auto NOTE_G5 = int16_t{ 784 };
	static constexpr auto NOTE_GS5 = int16_t{ 831 };
	static constexpr auto NOTE_A5 = int16_t{ 880 };
	static constexpr auto NOTE_AS5 = int16_t{ 932 };
	static constexpr auto NOTE_B5 = int16_t{ 988 };
	static constexpr auto NOTE_C6 = int16_t{ 1047 };
	static constexpr auto NOTE_CS6 = int16_t{ 1109 };
	static constexpr auto NOTE_D6 = int16_t{ 1175 };
	static constexpr auto NOTE_DS6 = int16_t{ 1245 };
	static constexpr auto NOTE_E6 = int16_t{ 1319 };
	static constexpr auto NOTE_F6 = int16_t{ 1397 };
	static constexpr auto NOTE_FS6 = int16_t{ 1480 };
	static constexpr auto NOTE_G6 = int16_t{ 1568 };
	static constexpr auto NOTE_GS6 = int16_t{ 1661 };
	static constexpr auto NOTE_A6 = int16_t{ 1760 };
	static constexpr auto NOTE_AS6 = int16_t{ 1865 };
	static constexpr auto NOTE_B6 = int16_t{ 1976 };
	static constexpr auto NOTE_C7 = int16_t{ 2093 };
	static constexpr auto NOTE_CS7 = int16_t{ 2217 };
	static constexpr auto NOTE_D7 = int16_t{ 2349 };
	static constexpr auto NOTE_DS7 = int16_t{ 2489 };
	static constexpr auto NOTE_E7 = int16_t{ 2637 };
	static constexpr auto NOTE_F7 = int16_t{ 2794 };
	static constexpr auto NOTE_FS7 = int16_t{ 2960 };
	static constexpr auto NOTE_G7 = int16_t{ 3136 };
	static constexpr auto NOTE_GS7 = int16_t{ 3322 };
	static constexpr auto NOTE_A7 = int16_t{ 3520 };
	static constexpr auto NOTE_AS7 = int16_t{ 3729 };
	static constexpr auto NOTE_B7 = int16_t{ 3951 };
	static constexpr auto NOTE_C8 = int16_t{ 4186 };
	static constexpr auto NOTE_CS8 = int16_t{ 4435 };
	static constexpr auto NOTE_D8 = int16_t{ 4699 };
	static constexpr auto NOTE_DS8 = int16_t{ 4978 };

	struct Tone
	{
		int16_t freq;
		int16_t duration;
	};

	struct Note
	{
		int16_t freq;
		int16_t type; // 1, 2, 4, 8 f�r Ganze, Halbe, Viertel ..... Negativ f�r gepunktete Noten (50% l�nger)
	};

	class System
	{
	public:
		static constexpr uint8_t UpperLeftKeyCode = 1;
		static constexpr uint8_t LowerLeftKeyCode = 2;
		static constexpr uint8_t UpperRightKeyCode = 3;
		static constexpr uint8_t LowerRightKeyCode = 4;

		static constexpr uint8_t FirstUnusedKeyCode = 5;

		static constexpr uint8_t LongKeyPress = 0x80;

		static constexpr uint8_t UpperLeftKeyMask = 2;
		static constexpr uint8_t LowerLeftKeyMask = 4;
		static constexpr uint8_t UpperRightKeyMask = 8;
		static constexpr uint8_t LowerRightKeyMask = 16;

		static constexpr auto LongKeyPressThreshold = 1000 / 40;

		static constexpr auto IgnoreUpdates = 0;
		static constexpr auto AskForUpdates = 1;
		static constexpr auto InstallUpdates = 2;

		struct UpdateInfo
		{
		public:
			static constexpr auto UndefinedStatus = 0; // unbekannter Status
			static constexpr auto CheckFailedStatus = 1; // Fehler beim Prüfen. Fehlermeldung in stringBuffer und evtl. Fehlercode in errorCode
			static constexpr auto UpToDateStatus = 2; // Installierte Firmware ist aktuell
			static constexpr auto UpdateAvailableStatus = 3;  // Neuere Firmware verfügbar. URL in stringBuffer.

		public:
			int status = UndefinedStatus;
			int errorCode = 0;
			int newVersion = 0;
			char newDate[11] = { 0 };
			char stringBuffer[200] = { 0 };
		};

		class Message : public String
		{
		public:
			using String::String;
		};

		class Time
		{
		public:
			int year = 0;
			int month = 0;
			int day = 0;

			int hour = 0;
			int minute = 0;
			int second = 0;
		};

	public:
		System();

		void initialize();

		void keyPressed(uint8_t);
		void keyRelease(uint8_t);

		void process();

		void putkey(uint8_t);
		uint8_t getkey();

		bool getKeyTones() const;

		uint32_t getSecondsSinceLastInput();

		int getUpdateHandling() const;

		void playTone(const Tone&);
		void playTones(const Tone* tones, size_t count);

		void playNote(const Note&, int wholeNoteDuration);
		void playNotes(const Note* notes, int wholeNoteDuration);

		template<int Count>
		void playTones(const std::array<Tone, Count>& tones)
		{
			playTones(tones.data(), tones.size());
		}

		void resetPlayback();

		// Liefert den nächsten Ton und löscht ihn aus der Liste
		// (Liefert einen Ton der Frequenz 0 und Länge 0 falls keine Töne in der Warteschlange sind)
		Tone getTone();

		String getStringSetting(const char* name, const char* defaultValue = "") const;
		void setStringSetting(const char* name, const char* value);
		int getIntSetting(const char* name, int defaultValue = -1) const;
		void setIntSetting(const char* name, int value);

		// Startet die WiFi Verbindung (neu)
		void wifiRestart() const;

		// Liefert true wenn WLAN verbunden ist
		bool wifiAvailable() const;

		// Liefert den WiFi Status wie WiFi.status()
		int wifiStatus() const;

		// Liefert zurück ob time() eine gültige Zeit liefert (d.h. ob setTime() innerhalb der letzten 24h zumindest einmal aufgerufen wurde)
		bool isTimeSynchronized() const;

		// Liefert die aktuelle Lokalzeit
		Time getLocalTime() const;

		// Wird vom Basiscode benutzt um Informationen über die Firmwareprüfung zu veröffentlichen
		void reportUpdateInfo(const UpdateInfo&);

		// Erlaubt den Zugriff auf die aktuelle UpdateInfo
		const UpdateInfo& getUpdateInfo() const;

		// Erlaubt dem Basiscode das aktuelle Update zu installieren (wird in System::process() zurückgesetzt und gilt nur einen Zyklus)
		void installUpdate();

		// Liefert true wenn das aktuelle Update installiert werden darf
		bool shouldInstallUpdate() const;

		// Fügt eine Meldung in den Meldungspuffer ein (und bringt das System dazu die erste Meldung anzuzeigen)
		void addMessage(Message);

		// Liefert einen Zeiger auf die älteste Meldung oder nullptr wenn keine vorliegt
		const Message* getMessage() const;

		// Entfernt die älteste Meldung aus der Liste
		void dropMessage();

		// Liefert einen Millisekundenzeitstempel (aus millis())
		static uint32_t getMillisecondTimestamp();

		// Liefert die Zeit in Millisekunden die seit dem angegebenen Zeitstempel vergangen ist
		static uint32_t getMillisecondDelay(uint32_t millisecondTimestamp);

		int httpGet(const char* url, const char* auth, String* contentTarget);

	public:
		uint8_t keyState = 0;

		uint16_t cycleCount = 0;

		Display display;

		class App* fallbackApp = nullptr;
		class App* currentApp = nullptr;
		class App* nextApp = nullptr;
		class App* pausedApp = nullptr;

		std::array<uint8_t, FirstUnusedKeyCode> keyHold;	// Wird f�r gehaltene Tasten bei jedem process() hochgez�hlt bis 255

		uint32_t lastInputTimestamp = 0;
		std::array<char, 16> keyBuffer;
		int keyBufferWriteIndex = 0;
		int keyBufferReadIndex = 0;

		std::array<Tone, 1024> toneBuffer;
		int toneBufferWriteIndex = 0;
		int toneBufferReadIndex = 0;

		bool keyTones = true;
		int updateHandling = AskForUpdates;

		uint32_t timeTimestamp = 0;

		UpdateInfo updateInfo;
		bool installUpdateFlag = false;

		std::array<Message, 8> messageBuffer;
		int messageWriteIndex = 0;
		int messageReadIndex = 0;
	};
}
