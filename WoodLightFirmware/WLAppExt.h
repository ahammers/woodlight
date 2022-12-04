#pragma once

#include "WLInput.h"
#include "WLSystem.h"
#include "WLMenu.h"

namespace WoodLight
{
    class WLSystem;

    class EditorExt
    {
    public:
        virtual void setCurrentFunction(void (EditorExt::* f)(System& system)) = 0;
        virtual void resetCurrentFunction() = 0;

    public:
        void editStringSetting(System& system, const char* caption, const char* settingName, const char* allowedCharacters, const char *defaultValue, int minLength, int maxLength);
        void editStringFunction(System& system);

        void editIntSetting(System& system, const char* label, const char* settingName, const int defaultValue, const int minValue, const int maxValue);
        void editIntFunction(System& system);

        void editIntEnumSetting(System& system, const char* label, const char* settingName, const char *const items[], const size_t itemCount, const int defaultValue);
        void editIntEnumFunction(System& system);

        void editStringEnumSetting(System& system, const char* label, const char* settingName, const char* const items[], const size_t itemCount, const char *const defaultValue);
        void editStringEnumFunction(System& system);

    public:
        const char* currentSettingName = nullptr;
        const char* const * currentItems = nullptr;
        Input input;
        Menu menu;
    };

}