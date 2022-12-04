#include "WLApp_HomeAssistant.h"
#include "WLApp.h"
#include "WLSystem.h"
#include "WLMenu.h"
#include "WLInput.h"
#include "WLAppExt.h"
#include "WLJson.h"
#include "WLScroller.h"

#include <ArduinoJson.h>

// Token for Tests: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJkZjcxYTk3YWM3YjQ0NzJiYTBlMzVkMDE2MDg1MTA2YyIsImlhdCI6MTY2OTkyNDU5OCwiZXhwIjoxOTg1Mjg0NTk4fQ.XtesXBKRQyCQvyrjB5H2FTXL7bebqNK2MJr4fZz16Sk

#ifdef _WIN32
	template<class T>
	T constrain(T x, T a, T b)
	{
		if (x < a)
			return a;
		else if (x > b)
			return b;
		else
			return x;
	}
#endif

namespace WoodLight
{
	namespace {
		static constexpr auto BaseUrlIndex = 0;
		static constexpr auto SecurityTokenIndex = 1;
		static constexpr auto UpdateIntervalIndex = 2;
		static constexpr auto SwitchingIndex = 3;
		static constexpr auto EntitiesIndex = 4;

		static constexpr const char* SetupMenuItems[] = {
			"Basis-URL",
			"SecurityToken",
			"Aktualisierungsintervall",
			"Werte durchschalten",
			"Anzeigewerte"
		};

		static constexpr const char* TenSecondItems[] = {
			"manuell",
			"1s",
			"2s",
			"3s",
			"4s",
			"5s",
			"6s",
			"7s",
			"8s",
			"9s",
			"10s"
		};

		static constexpr auto AllowedTokenCharacters = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ._0123456789+-*/~=";
		static constexpr auto AllowedEntityCharacters = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ._0123456789+-*/~=";
		static constexpr auto AllowedCaptionCharacters = "aAäÄbBcCdDeEfFgGhHiIjJkKlLmMnNoOöÖpPqQrRsSßtTuUüÜvVwWxXyYzZ .,:;_#0123456789+-*/%&$?~=!()[]";

		static constexpr auto MaxIdLength = 64;
		static constexpr auto MaxCaptionLength = 18;

		class HomeAssistantApp : public App, public EditorExt
		{
		public:
			class Entity
			{
			public:
				bool active = false;		// Konfiguriert?
				char caption[20];			// Beschriftung am Display (aus HE_CAP##)
				char id[80];				// Name der Entity im Home Assistant (aus HE_ENT##)
				uint32_t lastUpdate = 0;	// Zeitstempel des letzten Updates (MillisecondTimestamp; 0 wenn nicht geladen)
				bool haValid = false;		// Wird gesetzt wenn beim letzten Update ein gültiger Wert empfangen wurde
				double haState = 0.0;		// Wert der Entity
				char haUnit[8];				// Einheit
				char haName[40];			// Bezeichnung in HomeAssistant
			};

		public:
			HomeAssistantApp()
				: currentFunction(&HomeAssistantApp::displayFunction),
				setupMenu(SetupMenuItems, sizeof(SetupMenuItems) / sizeof(*SetupMenuItems), Buttons::OkBack)
			{}

		public: // App
			virtual void initialize(System& system) override
			{
				settingsChangedFlag = true;
				//_CrtDbgBreak(); // Zu Testzwecken damit ich das nicht eingeben muß!
				//system.setStringSetting("HA_URL", "http://server:8123/api/");
				//system.setStringSetting("HA_SEC", "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJkZjcxYTk3YWM3YjQ0NzJiYTBlMzVkMDE2MDg1MTA2YyIsImlhdCI6MTY2OTkyNDU5OCwiZXhwIjoxOTg1Mjg0NTk4fQ.XtesXBKRQyCQvyrjB5H2FTXL7bebqNK2MJr4fZz16Sk");
				//system.setIntSetting("HA_INT", 10);
				//system.setStringSetting("HA_ENT01", "sensor.terrassensteckdose_montiert_im_esszimmer_power"); // Speicherplatzverwendung am Server
				//system.setStringSetting("HA_CAP01", "Balkon PV"); // Stromverbrauch Fernseher Opa
				//system.setStringSetting("HA_ENT02", "sensor.netzwerkinfrastruktur_power"); // Speicherplatzverwendung am Server
				//system.setStringSetting("HA_ENT03", "sensor.fernseher_opa_power"); // Stromverbrauch Fernseher Opa
				//system.setStringSetting("HA_CAP03", "Opa TV"); // Stromverbrauch Fernseher Opa

				currentEntityIndex = constrain(system.getIntSetting("HA_ENT", 0), 0, static_cast<int>(entities.size() - 1));
			}

			virtual void update(System& system) override
			{
				if (settingsChangedFlag)
					this->reloadSettings(system);

				(this->*currentFunction)(system);
			}

			virtual void settingsChanged(class System&, const char* const settingName) override
			{
				if (!strncmp(settingName, "HA_", 3))
					settingsChangedFlag = true;
			}

		public: // EditorExt
			virtual void setCurrentFunction(void (EditorExt::* f)(System& system)) override
			{
				currentFunction = f;
			}

			virtual void resetCurrentFunction() override
			{
				currentFunction = entitySetupActive
					? &HomeAssistantApp::entitySetupFunction
					: &HomeAssistantApp::setupFunction;
			}

		private:
			void displayFunction(System& system)
			{
				updateEntities(system);

				// Entities zählen
				auto acitveEntityCount = 0;
				for (auto& entity : entities)
					if (entity.active)
						++acitveEntityCount;

				system.display.reset(false);

				// Nicht eingerichtet - Jede Taste startet das Setup
				if (acitveEntityCount == 0)
				{
					static constexpr auto Msg = "Drücke eine Touch Taste";
					const auto MsgWidth = Display::getTextWidth(Msg);

					system.display.reset(false);

					system.display.print((Display::Columns - MsgWidth) / 2, 0, true, Msg);

					if (system.getkey())
						currentFunction = &HomeAssistantApp::setupFunction;

				// Eingerichtet
				} else
				{
					// Automatisch weiterschalten
					if (autoSwitchingIntervalS > 0 && (lastAutoSwitch == 0 || system.getMillisecondDelay(lastAutoSwitch) > 1000 * autoSwitchingIntervalS))
					{
						do {
							currentEntityIndex = (currentEntityIndex + 1) % entities.size();
						} while (!entities[currentEntityIndex].active);
						lastAutoSwitch = system.getMillisecondTimestamp();
						if (!lastAutoSwitch)
							lastAutoSwitch = 1;
						
					}

					if (acitveEntityCount > 1)
						drawButtons(system, Buttons::UpDown, Buttons::SetupBack, DrawButtons::HideAfterTimeout);

					for (size_t i = 0; i < entities.size() & !entities[currentEntityIndex].active; ++i)
						currentEntityIndex = (currentEntityIndex + 1) % entities.size();

					//auto cr = system.display.getClippingRect();
					//system.display.rect(cr.left, -1, 7, 9, true);
					//system.display.printSmall(cr.left + 2, 1, true, String(currentEntityIndex).c_str());
					//cr.left += 8;
					//system.display.setClippingRect(cr);
					
					auto& entity = entities[currentEntityIndex];

					char buffer[64];
					if (entity.haValid)
						snprintf(buffer, sizeof(buffer), "%.1f %s", entity.haState, entity.haUnit);
					else
						snprintf(buffer, sizeof(buffer), "n.v.");

					const auto w = Display::getTextWidth(buffer);
					auto r = system.display.getClippingRect();
					system.display.print(r.right - w, 0, true, buffer);
					r.right -= w + 4;
					system.display.setClippingRect(r);

					if (*entity.caption)
						scroller.setTextPtr(entity.caption);
					else if (*entity.haName)
						scroller.setTextPtr(entity.haName);
					else
						scroller.setTextPtr(entity.id);
					scroller.update(system);

					while (auto key = system.getkey())
					{
						switch (key)
						{
						case System::UpperLeftKeyCode:
							do {
								currentEntityIndex = (currentEntityIndex + entities.size() - 1) % entities.size();
							} while (!entities[currentEntityIndex].active);
							break;

						case System::LowerLeftKeyCode:
							do {
								currentEntityIndex = (currentEntityIndex + 1) % entities.size();
							} while (!entities[currentEntityIndex].active);
							break;

						case System::UpperLeftKeyCode | System::LongKeyPress:
						case System::LowerLeftKeyCode | System::LongKeyPress:
							system.setIntSetting("HA_ENT", currentEntityIndex);
							break;

						case System::UpperRightKeyCode:
							currentFunction = &HomeAssistantApp::setupFunction;
							break;

						case System::LowerRightKeyCode:
							system.nextApp = system.fallbackApp;
							break;
						}
					}
				}

			}

			void setupFunction(System& system)
			{
				switch (setupMenu.exec(system))

				{
				case BaseUrlIndex: editStringSetting(system, "URL:", "HA_URL", Input::AllowedUrlCharacters, "http://host:8123/app/", 5, 128); break;
				case SecurityTokenIndex: editStringSetting(system, "Token:",  "HA_SEC", AllowedTokenCharacters, "", 1, 128); break;
				case UpdateIntervalIndex: editIntSetting(system, "Int[s]:", "HA_INT", 30, 1, 24 * 3600); break;
				case SwitchingIndex: editIntEnumSetting(system, "Durchschalten:", "HA_ASW", TenSecondItems, sizeof(TenSecondItems) / sizeof(*TenSecondItems), 0); break;

				case EntitiesIndex: currentFunction = &HomeAssistantApp::entitiesFunction; break;
				
				case Menu::BackIndex:
					currentFunction = &HomeAssistantApp::displayFunction;
					break;
				}
			}

			void entitiesFunction(System& system)
			{
				drawButtons(system, Buttons::UpDown, Buttons::SetupBack);

				if (currentEntityIndex < 0)
					currentEntityIndex = 0;
				else if (currentEntityIndex >= entities.size())
					currentEntityIndex = entities.size() - 1;

				printEntityNumber(system);

				// Name und ID
				auto& entity = entities[currentEntityIndex];
				if (!entity.active)
					snprintf(scrollBuffer, sizeof(scrollBuffer), "frei");
				else if (*entity.caption)
					snprintf(scrollBuffer, sizeof(scrollBuffer), "%s: %s", entity.caption, entity.id);
				else
					snprintf(scrollBuffer, sizeof(scrollBuffer), "%s", entity.id);

				scroller.setTextPtr(scrollBuffer);
				scroller.update(system);

				while (auto key = system.getkey())
				{
					switch (key)
					{
					case System::UpperLeftKeyCode:
						currentEntityIndex = (currentEntityIndex + entities.size() - 1) % entities.size();
						break;
					case System::LowerLeftKeyCode:
						currentEntityIndex = (currentEntityIndex + 1) % entities.size();
						break;
					case System::UpperRightKeyCode:
						currentFunction = &HomeAssistantApp::entitySetupFunction;
						break;
					case System::LowerRightKeyCode:
					case System::LowerRightKeyCode | System::LongKeyPress:
						currentFunction = &HomeAssistantApp::setupFunction;
						break;
					}
				}
			}

			void entitySetupFunction(System& system)
			{
				static constexpr auto SubFunctionCount = 2;
				static char entityNameBuffer[10];

				subFunctionIndex = constrain(subFunctionIndex, 0, SubFunctionCount-1);

				entitySetupActive = true;
					
				const char* caption = "";

				drawButtons(system, Buttons::UpDown, Buttons::SetupBack);
				printEntityNumber(system);

				switch (subFunctionIndex)
				{
				case 0: // ID
					caption = "ID:";
					snprintf(scrollBuffer, sizeof(scrollBuffer), "%s", entities[currentEntityIndex].id);
					break;

				case 1: // Caption
					caption = "Titel:";
					snprintf(scrollBuffer, sizeof(scrollBuffer), "%s", entities[currentEntityIndex].caption);
					break;
				}

				const auto captionWidth = Display::getTextWidth(caption);
				auto r = system.display.getClippingRect();
				system.display.print(r.left, 0, true, caption);
				r.left += captionWidth + 2;
				system.display.setClippingRect(r);

				scroller.setTextPtr(scrollBuffer);
				scroller.update(system);

				while (auto key = system.getkey())
				{
					switch (key)
					{
					case System::UpperLeftKeyCode:
						subFunctionIndex = (subFunctionIndex + SubFunctionCount - 1) % SubFunctionCount;
						break;
					case System::LowerLeftKeyCode:
						subFunctionIndex = (subFunctionIndex + 1) % SubFunctionCount;
						break;
					case System::UpperRightKeyCode:
						switch (subFunctionIndex)
						{
						case 0:
						{
							snprintf(entityNameBuffer, sizeof(entityNameBuffer), "HA_ENT%02d", currentEntityIndex);
							editStringSetting(system, caption, entityNameBuffer, AllowedEntityCharacters, "", 1, MaxIdLength);
						} break;
						case 1:
						{
							snprintf(entityNameBuffer, sizeof(entityNameBuffer), "HA_CAP%02d", currentEntityIndex);
							editStringSetting(system, caption, entityNameBuffer, AllowedCaptionCharacters, "", 1, MaxCaptionLength);
						} break;
						}
						break;

					case System::LowerRightKeyCode:
						entitySetupActive = false;
						currentFunction = &HomeAssistantApp::entitiesFunction;
						break;
					}
				}
			}

			// Aktualisiert die Werte der Entities wenn diese veraltet sind
			void updateEntities(System& system)
			{
				char urlBuffer[200];

				int intervallDelay = 0;
				for (auto& entity : entities)
				{
					intervallDelay += 10;
					if (entity.active && (entity.lastUpdate == 0 || system.getMillisecondDelay(entity.lastUpdate) >= updateIntervalMs + intervallDelay))
					{
						entity.lastUpdate = system.getMillisecondTimestamp();

						if (entity.lastUpdate == 0)
							entity.lastUpdate = 1;
						entity.haValid = false;

						snprintf(urlBuffer, sizeof(urlBuffer), homeAssistantUrlMask, entity.id);

						auto content = String{};
						const auto response = system.httpGet(urlBuffer,	homeAssistantAuth, &content);

						if (response >= 200 && response < 300)
						{
							const auto err = deserializeJson(jsonDoc, content);

							if (err == DeserializationError::Ok)
							{
								entity.haState = jsonDoc["state"];

								const char* unit = jsonDoc["attributes"]["unit_of_measurement"];
								snprintf(entity.haUnit, sizeof(entity.haUnit), "%s", unit);
								entity.haUnit[sizeof(entity.haUnit) - 1] = 0;

								const char* name = jsonDoc["attributes"]["friendly_name"];
								snprintf(entity.haName, sizeof(entity.haName), "%s", name);
								entity.haName[sizeof(entity.haName) - 1] = 0;

								entity.haValid = true;
							}

						}

					}
				}
			}

			void reloadSettings(System& system)
			{
				// Basiseinstellungen
				updateIntervalMs = system.getIntSetting("HA_INT", 30) * 1000;
				autoSwitchingIntervalS = system.getIntSetting("HA_ASW", 0);

				snprintf(homeAssistantUrlMask, sizeof(homeAssistantUrlMask), "%sstates/%%s", system.getStringSetting("HA_URL", "").c_str());
				snprintf(homeAssistantAuth, sizeof(homeAssistantAuth), "Bearer %s", system.getStringSetting("HA_SEC", "").c_str());

				// Aktualisiert die Entites aus den Settings (muß beim Start und bei jeder Konfigurationsänderung aufgerufen werden)
				for (size_t i = 0; i < entities.size(); ++i)
				{
					auto& entity = entities[i];

					char settingName[9];
					snprintf(settingName, sizeof(settingName), "HA_CAP%02d", static_cast<int>(i));
					auto caption = system.getStringSetting(settingName);
					snprintf(settingName, sizeof(settingName), "HA_ENT%02d", static_cast<int>(i));
					auto id = system.getStringSetting(settingName);

					if (id.length() < 1)
					{
						memset(&entity, 0, sizeof(entity));
						entity.active = false;
					} else
					{
						// Wenn der Eintrag nicht aktiv ist, oder schon aktiv aber die Quelle wird geändert dann setzen wir lastUpdate zurück damit neu geladen wird
						if (!entity.active || (entity.lastUpdate && strcmp(entity.id, id.c_str())))
							entity.lastUpdate = 0;

						// Alles zurücksetzen
						if (entity.lastUpdate == 0)
						{
							entity.haValid = false;
							entity.haState = 0.0;
							memset(entity.haUnit, 0, sizeof(entity.haUnit));
							memset(entity.haName, 0, sizeof(entity.haName));
						}

						// Beschriftung
						snprintf(entity.caption, sizeof(entity.caption), "%s", caption.c_str());

						// ID
						snprintf(entity.id, sizeof(entity.id), "%s", id.c_str());

						// Und jetzt auf jeden Fall aktiv
						entity.active = true;
					}
				}

				settingsChangedFlag = false;
			}

			void printEntityNumber(System& system)
			{
				char buffer[8];
				// Eintragsnummer
				snprintf(buffer, sizeof(buffer), "%d", currentEntityIndex);
				const auto numWidth = Display::getSmallTextWidth(buffer);
				auto r = system.display.getClippingRect();
				for (int i = 0; i < numWidth + 3; ++i)
					system.display.setCol(r.left + i, 0xFF);
				system.display.printSmall(r.left + 2, 1, false, buffer);
				r.left += numWidth + 4;
				system.display.setClippingRect(r);
			}
		private:
			void (HomeAssistantApp::* currentFunction)(System& system);
			bool settingsChangedFlag = true;
			int updateIntervalMs = 30000;
			int autoSwitchingIntervalS = 0;
			char homeAssistantUrlMask[200];
			char homeAssistantAuth[250];
			int currentEntityIndex = 0;
			Menu setupMenu;
			Scroller scroller;
			char scrollBuffer[100];
			uint32_t lastAutoSwitch = 0;
			bool entitySetupActive = false;
			int subFunctionIndex = 0;
			std::array<Entity, 10> entities;
		};
	}

	App* createHomeAssistantApp()
	{
		return new HomeAssistantApp;
	}
}
