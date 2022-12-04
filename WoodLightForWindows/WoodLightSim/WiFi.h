#pragma once

static constexpr auto WL_IDLE_STATUS = 0;		// temporary status assigned when WiFi.begin() is called
static constexpr auto WL_NO_SSID_AVAIL = 1;		// when no SSID are available
static constexpr auto WL_SCAN_COMPLETED = 2;	// scan networks is completed
static constexpr auto WL_CONNECTED = 3;			// when connected to a WiFi network
static constexpr auto WL_CONNECT_FAILED = 4;	// when the connection fails for all the attempts
static constexpr auto WL_CONNECTION_LOST = 5;	// when the connection is lost
static constexpr auto WL_DISCONNECTED = 6;		// when disconnected from a network

static constexpr auto WIFI_STA = 0; //	station mode : the ESP32 connects to an access point
static constexpr auto WIFI_AP = 1; //	access point mode : stations can connect to the ESP32
static constexpr auto WIFI_STA_AP = 2; //	access pointand a station connected to another access point

class WiFiClass
{
public:
	inline void setHostName(const char*)
	{}

	inline void mode(int)
	{}

	inline void begin(const char* ssid, const char* password)
	{
		s = WL_CONNECTED;
	}

	inline void disconnect()
	{
		s = WL_DISCONNECTED;
	}
	inline int status() const
	{
		return s;
	}


private:
	int s = WL_DISCONNECTED;
};

extern WiFiClass WiFi;
