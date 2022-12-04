#include "WLHttp.h"

#ifdef _WIN32
#include <Windows.h>
#include <wininet.h>
#include <vector>
#include <array>
#pragma comment(lib, "Wininet.lib")
#else
#include <HTTPClient.h>
#endif

namespace WoodLight
{
    int httpGet(const char* url, const char* auth, String* contentTarget)
    {
        auto result = 600;

#ifdef _WIN32
        if (auto hInternet = InternetOpenA("WoodLightSim", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0))
        {
            auto headers = auth
                ? std::string("Authorization: ") + auth
                : std::string("");

            DWORD err;
            auto buffer = std::vector<char>(65536);
            DWORD bufferLen = buffer.size();

            if (auto hUrl = InternetOpenUrlA(hInternet, url, headers.c_str(), -1, INTERNET_FLAG_NO_UI | INTERNET_FLAG_RELOAD, 0))
            {
                bufferLen = buffer.size();
                if (HttpQueryInfoA(hUrl, HTTP_QUERY_STATUS_CODE, buffer.data(), &bufferLen, NULL))
                {
                    const auto s = std::string(buffer.data(), buffer.data() + bufferLen);
                    result = atoi(s.c_str());
                }

                //bufferLen = buffer.size();
                //if (HttpQueryInfoA(hUrl, HTTP_QUERY_RAW_HEADERS_CRLF, buffer.data(), &bufferLen, NULL))
                //{
                //    _CrtDbgBreak();
                //}

                if (result >= 200 && result <= 299)
                {
                    auto buffer = std::vector<char>(65536);
                    DWORD bytesRead = 0;
                    if (InternetReadFile(hUrl, buffer.data(), buffer.size(), &bytesRead) == TRUE)
                    {
                        contentTarget->resize(bytesRead);
                        memcpy(contentTarget->data(), buffer.data(), bytesRead);
                    } else
                    {
                        result = 604;
                    }
                }

                InternetCloseHandle(hUrl);
            } else
            {
                result = 602;

                //if (HttpQueryInfo(hInternet, HTTP_QUERY_RAW_HEADERS_CRLF, buffer.data(), &bufferLen, NULL))
                //{
                //    _CrtDbgBreak();
                //}

                if (InternetGetLastResponseInfoA(&err, buffer.data(), &bufferLen) == TRUE)
                {
                    _CrtDbgBreak();
                }
               
            }

            InternetCloseHandle(hInternet);

        } else
        {
            result = 601;
        }

        return result;
#else
        HTTPClient http;
        http.begin(url);
        if (auth)
            http.addHeader("Authorization", auth);

        auto response = http.GET();

        Serial.print("GET ");
        Serial.print(url);
        Serial.print(" resulted in ");
        Serial.println(response);
        if (auth)
        {
            Serial.print("auth is ");
            Serial.println(auth);
        }
        if (contentTarget)
        {
            *contentTarget = std::move(http.getString());
            Serial.println(*contentTarget);
        }

        return response;

#endif
	}

}
