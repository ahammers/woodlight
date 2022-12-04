#include <Preferences.h>

#include "MAX7219.h"
#include "JSON.h"

#include "WLSystem.h"
#include "WLApp.h"
#include "WLApp_Menu.h"

#include <HTTPClient.h>
#include <HTTPUpdate.h>

static constexpr auto Version = 7;
static constexpr auto VersionDate = "2022-12-02";

static constexpr auto BUZZER_PIN = 21;
static constexpr auto BUZZER_CHANNEL = 0;

/**
 * Hinweis:
 * - Mit Board Library 2.0.5 treten HTTPClient Error Meldungen bei entsprechend eingesteller Warnstufe auf. Mit 2.0.1 nicht.
 * */
 
// Speichert Einstellungen
Preferences preferences;

WoodLight::System wlSystem;

struct TouchButton
{
  int pin;
  uint8_t key;
};

using namespace WoodLight;

static const auto TouchButtons = std::array<TouchButton, 4>{
  TouchButton{ 39, WoodLight::System::LowerLeftKeyCode },
  TouchButton{ 36, WoodLight::System::UpperLeftKeyCode },
  TouchButton{ 35, WoodLight::System::UpperRightKeyCode },
  TouchButton{ 34, WoodLight::System::LowerRightKeyCode }
};
  
void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  
  for(const auto& touchButton: TouchButtons)
    pinMode(touchButton.pin, INPUT);

  display_init();
  wlSystem.initialize();

  Serial.println("INITIALIZED");
}

void tryTimeSynchronization()
{
    const auto ntpsrv1 = wlSystem.getStringSetting("NTPSRV1", "europe.pool.ntp.org");
    const auto ntpsrv2 = wlSystem.getStringSetting("NTPSRV2", "ts1.univie.ac.at");
    const auto ntpsrv3 = wlSystem.getStringSetting("NTPSRV3", "time.metrologie.at");
    const auto gmtOffset = wlSystem.getIntSetting("GMTOFFSET", 3600);
    const auto dsOffset = wlSystem.getIntSetting("DSOFFSET", 3600);

    Serial.printf("Trying time synchronization with [%s, %s, %s] gmt+%ds, ds+%ds\n", ntpsrv1.c_str(), ntpsrv2.c_str(), ntpsrv3.c_str(), gmtOffset, dsOffset);

    configTime(gmtOffset, dsOffset, ntpsrv1.c_str(), ntpsrv2.c_str(), ntpsrv3.c_str());
}

void updateButtons()
{
 for(size_t buttonIndex = 0; buttonIndex < TouchButtons.size(); ++buttonIndex)
  {
    if (digitalRead(TouchButtons[buttonIndex].pin) == HIGH)
      wlSystem.keyPressed(TouchButtons[buttonIndex].key);
    else
      wlSystem.keyRelease(TouchButtons[buttonIndex].key);
  }
}

int noteDurationLeft = 0;

bool isDecDigit(char ch)
{
    return ch >= '0' && ch <= '9';
}

bool nameMatches(const char *name, const char *mask)
{
    if (strlen(name) != strlen(mask))
        return false;

    for(auto i = 0; name[i]; ++i)
        if (name[i] != mask[i] && (mask[i] != '#' || !isDecDigit(name[i])))
            return false;

    return true;
}

void serialControl(WoodLight::System& wlSystem)
{
  static char buffer[256];
  static auto bufferBytes = 0;
  static const char *const KnownSettings[] = {
    // System
    "S" "NTPSRV1",
    "S" "NTPSRV2",
    "S" "NTPSRV3",
    "I" "GMTOFFSET",
    "I" "DSOFFSET",
    "I" "SYSKEYTONE",
    "I" "SYSUPDATE",

    // Basic Setup
    "S" "WIFISSID",
    "S" "WIFIPASSWORD",

    // Clock
    "I" "CLK_MODE",

    // HomeAssistant
    "S" "HA_URL",
    "S" "HA_SEC",
    "I" "HA_INT",
    "I" "HA_ASW",
    "I" "HA_ENT",
    "S" "HA_CAP##",
    "S" "HA_ENT##",

    nullptr
  };

  while(Serial.available())
  {
    const auto ch = Serial.read();
     
    // Zeilenende empfangen
    if (ch == '\n')
    {
    
      // Wenn der Puffer bereits voll war melden wir ein Problem
      if (bufferBytes < 5 || bufferBytes >= 255)
      {
        Serial.println("ERROR Invalid command size");
  
      // Auswertbares Kommando
      } else
      {
        buffer[bufferBytes] = 0;
  
        if (!strcmp(buffer, "HELP?") || !strcmp(buffer, "HELP") || !strcmp(buffer, "help") || !strcmp(buffer, "?"))
        {
          Serial.println("Commands available:");
          Serial.println("INFO? ................ show version number and a list of available settings");
          Serial.println("TIME={NTP|<UTC>} ..... sets time by NTP service or unix timestamp");
          Serial.println("<Setting>=<VALUE> .... change the value of a setting");
          Serial.println("<Setting>? ........... query the value of a setting");
          Serial.println("Settings available:");
          Serial.println("WIFISSID ....... WiFi SSID");
          Serial.println("WIFIPASSWORD ... WiFi Password");
          Serial.println("...and many more (use INFO? to get a list)");
  
        } else if (!strcmp(buffer, "INFO?") || !strcmp(buffer, "VERSION?"))
        {
          Serial.printf("INFO WoodLight v%d (%s)\n", Version, VersionDate);

          for(auto ptr = KnownSettings; *ptr != nullptr; ++ptr)
            Serial.println(*ptr + 1);

        } else if (!strncmp(buffer, "TIME=", 5))
        {
          if (!strcmp(buffer+5, "NTP"))
            tryTimeSynchronization();
          else
            Serial.println("ERROR: Only NTP is allowed as an argument for TIME=");

        } else if (!strcmp(buffer, "TIME?"))
        {
          Serial.printf("TIME=%d\n", time(NULL));

        } else 
        {
          auto delimiter = 0;
          while(buffer[delimiter] && buffer[delimiter] != '=' && buffer[delimiter] != '?')
            ++delimiter;

          if (buffer[delimiter])
          {
            const auto isUpdate = buffer[delimiter] == '=';
            const auto value = buffer+delimiter+1;
            const auto name = buffer;
            buffer[delimiter] = 0;
            //Serial.print("name="); Serial.println(name);
            //Serial.print("value="); Serial.println(value);

            size_t settingIndex = 0;
            while(KnownSettings[settingIndex] && !nameMatches(name, KnownSettings[settingIndex] + 1))
              ++settingIndex;

            if (KnownSettings[settingIndex])
            {
              //Serial.print("KnownSetting=");
              //Serial.println(KnownSettings[settingIndex]);
              
              if (isUpdate)
              {
                switch(KnownSettings[settingIndex][0])
                {
                  case 'S':
                    wlSystem.setStringSetting(name, value);
                    Serial.println("OK");
                    if (String(name).startsWith("WIFI"))
                      wlSystem.wifiRestart();
                    break;
                  case 'I':
                  {
                    const auto v = String(value).toInt();
                    if (v == 0 && strcmp(value, "0"))
                      Serial.println("ERROR: Integer value expected");
                    else
                    {
                      wlSystem.setIntSetting(name, v);
                      Serial.println("OK");
                    }
                  } break;
                  default: 
                  Serial.println("ERROR: Unsupported data type "); break;
                }                
              } else
              {
                //Serial.println(KnownSettings[settingIndex][0]);
                Serial.print(name);
                Serial.print('=');

                switch(KnownSettings[settingIndex][0])
                {
                  case 'S': Serial.println(wlSystem.getStringSetting(name)); break;
                  case 'I': Serial.println(wlSystem.getIntSetting(name)); break;
                  default: Serial.println("*ERROR: Unsupported data type*"); break;
                }
              }
              
              


            } else
            {
              Serial.println("ERROR: Unknown setting");
            }
            
          } else
          {
            Serial.println("ERROR: Invalid command (Try HELP?)");
          }
        }
      }
  
      bufferBytes = 0;
      
    // Puffern so lange noch Platz ist
    } else if (bufferBytes < 255)
    {
      buffer[bufferBytes++] = ch;
    }
  }
}

void checkForUpdates()
{
  static constexpr auto CheckIntervalMS = 300000;  // Alle fünf Minuten auf Updates prüfen 
  static auto lastCheck = wlSystem.getMillisecondTimestamp();
  
  if (wlSystem.getUpdateHandling() != System::IgnoreUpdates && wlSystem.getMillisecondDelay(lastCheck) >= CheckIntervalMS)
  {
    lastCheck = wlSystem.getMillisecondTimestamp();
    auto updateInfo = WoodLight::System::UpdateInfo{};
    
    if (!wlSystem.wifiAvailable())
    {
      updateInfo.status = WoodLight::System::UpdateInfo::CheckFailedStatus;
      strcpy(updateInfo.stringBuffer, "WiFi not available");
      Serial.println("UPDATE: WiFi not available");

    } else
    {
      HTTPClient http;
    
      const auto url = "http://woodlight.hammerschmidt.at/ota.php?version=" + String(Version) + "&device=" + preferences.getString("deviceid", "none");
      Serial.print("Checking for updates firmware at ");
      Serial.println(url);
      
      http.begin(url.c_str());
     
      auto response = http.GET();

      if (response < 200 || response >= 300)
      {
        updateInfo.status = WoodLight::System::UpdateInfo::CheckFailedStatus;
        strcpy(updateInfo.stringBuffer, "Failed to query update information");
        updateInfo.errorCode = response;
        
        Serial.printf("UPDATE: Failed to query update information. Server returned %d\n", static_cast<int>(response));
        
      } else if ( deserializeJson(doc, http.getString()) != DeserializationError::Ok)
      {
        updateInfo.status = WoodLight::System::UpdateInfo::CheckFailedStatus;
        strcpy(updateInfo.stringBuffer, "Failed to parse update information");
        
        Serial.printf("UPDATE: Failed to parse update information. JSON decoder returned an error.");
        
      } else
      {
        try {
          const int currentMinVersion = doc["current-min-version"];
          const int currentMaxVersion = doc["current-max-version"];
          const int newVersion = doc["new-version"];
          const char *newDate = doc["new-date"];
          const char *newUrl = doc["new-url"];

          if (strlen(newDate) + 1 > sizeof(updateInfo.newDate) || strlen(newUrl) + 1 > sizeof(updateInfo.stringBuffer))
          {
            updateInfo.status = WoodLight::System::UpdateInfo::CheckFailedStatus;
            strcpy(updateInfo.stringBuffer, "Update information to big");

          } else if (Version >= currentMinVersion && Version <= currentMaxVersion && Version != newVersion)
          {
            updateInfo.status = WoodLight::System::UpdateInfo::UpdateAvailableStatus;
            updateInfo.newVersion = newVersion;
            strcpy(updateInfo.newDate, newDate);
            strcpy(updateInfo.stringBuffer, newUrl);
          } else
          {
            updateInfo.status = WoodLight::System::UpdateInfo::UpToDateStatus;
          }
        } catch(...)
        {
          updateInfo.status = WoodLight::System::UpdateInfo::CheckFailedStatus;
          strcpy(updateInfo.stringBuffer, "Invalid or incomplete update information received");
        }
      }
    }

    wlSystem.reportUpdateInfo(updateInfo);

    if (wlSystem.shouldInstallUpdate())
      Serial.println("Update should install without user interaction");
  }
}

void update_started() {
 
    wlSystem.display.clear(false);
    wlSystem.display.print(0, 0, true, "Update startet...");
    display_output(wlSystem.display);
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
  wlSystem.addMessage("Aktualisierung erfolgreich");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);


  if (cur == total)
  {
    wlSystem.display.clear(false);
    Serial.println("Switched to black display for flashing");
    
  } else
  {
    const auto lastColumn = Display::Columns * cur / total;
    auto x = 0; 
    wlSystem.display.setCol(x, 0b00111100);
    for(++x; x < lastColumn; ++x)
      wlSystem.display.setCol(x, (x <= lastColumn) ? 0b00100100 : 0);
    wlSystem.display.setCol(x, 0b00111100);
    for(++x; x < Display::Columns; ++x)
      wlSystem.display.setCol(x, 0);
  }
   
  display_output(wlSystem.display);
}

void update_error(int err) {
  char buffer[64];
  snprintf(buffer, sizeof(buffer), "Update fehlgeschlagen. Der Fehlercode ist %d", err);
  wlSystem.addMessage(buffer);
}

void installUpdate(const char *url)
{
    Serial.printf("Starting update from %s\n", url);

    // Audio deaktivieren
    ledcDetachPin(BUZZER_PIN);

    WiFiClient client;

    // The line below is optional. It can be used to blink the LED on the board during flashing
    // The LED will be on during download of one buffer of data from the network. The LED will
    // be off during writing that buffer to flash
    // On a good connection the LED should flash regularly. On a bad connection the LED will be
    // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
    // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
    // httpUpdate.setLedPin(LED_BUILTIN, LOW);
    wlSystem.display.reset(false);
    wlSystem.display.print(0, 0, true, "Update vorbereiten...");
    display_output(wlSystem.display);

    httpUpdate.onStart(update_started);
    httpUpdate.onEnd(update_finished);
    httpUpdate.onProgress(update_progress);
    httpUpdate.onError(update_error);

    t_httpUpdate_return ret = httpUpdate.update(client, url);

    switch (ret) 
    {
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

void loop()
{
  // Zeitsynchronisation ausführen wenn noch nie synchronisiert wurde oder die Daten zu alt sind und WLAN verfügbar ist (aber maximal ein Versuch alle 10 Sekunden)
  static auto firstTimeSync = true;
  if (firstTimeSync || (time(NULL) < 660069179 && (millis() % 2000) == 0))
  {
    firstTimeSync = false;
    tryTimeSynchronization();
  }

  // Auf Updates prüfen wir quasi im Leerlauf
  if (millis() % 40 == 4)
  {
    checkForUpdates();
  
    if (wlSystem.shouldInstallUpdate() && wlSystem.getUpdateInfo().status == WoodLight::System::UpdateInfo::UpdateAvailableStatus)
      installUpdate(wlSystem.getUpdateInfo().stringBuffer);
  
  }
  
  if (millis() % 40 == 0)
  {
    serialControl(wlSystem);
    
    updateButtons();
    
    wlSystem.process();

    if (wlSystem.currentApp)
        wlSystem.currentApp->update(wlSystem);

    if (wlSystem.nextApp)
    {
        if (wlSystem.currentApp != wlSystem.fallbackApp)
            delete wlSystem.currentApp;

        wlSystem.currentApp = wlSystem.nextApp;
        wlSystem.nextApp = 0;

        wlSystem.currentApp->initialize(wlSystem);
    }

    if (wlSystem.display.isChanged())
    {
        wlSystem.display.clearChanged();
        display_output(wlSystem.display);
    }

    if (noteDurationLeft <= 0)
    {
        auto tone = wlSystem.getTone();
        noteDurationLeft = tone.duration;
        if (tone.freq > 0)
        {
          Serial.print("Playing a ");
          Serial.print(tone.freq);
          Serial.print("Hz tone for ");
          Serial.print(tone.duration * 40);
          Serial.println("ms");
          ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
          ledcWriteTone(BUZZER_CHANNEL, tone.freq);
        } else
        {
          ledcDetachPin(BUZZER_PIN);
        }
    }

    if (noteDurationLeft > 0)
        --noteDurationLeft;

    if (millis() % 40 == 0)
      delay(1);
  }
  
}
