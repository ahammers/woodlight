#include "WLAppExt.h"

namespace WoodLight
{
    void EditorExt::editStringSetting(System& system, const char* caption, const char* settingName, const char* allowedCharacters, const char *defaultValue, int minLength, int maxLength)
    {
        currentSettingName = settingName;
        input.initialize(caption, system.getStringSetting(settingName, defaultValue).c_str(), allowedCharacters, minLength, maxLength);
        setCurrentFunction(&EditorExt::editStringFunction);
    }

    void EditorExt::editStringFunction(System& system)
    {
        switch (input.exec(system))
        {
        case Input::Accept:
            system.setStringSetting(currentSettingName, input.getValue());

            // [[fallthrough]];
        case Input::Cancel:
            resetCurrentFunction();
            break;
        }
    }

    void EditorExt::editIntSetting(System& system, const char* label, const char* settingName, const int defaultValue, const int minValue, const int maxValue)
    {
        currentSettingName = settingName;
        input.initialize(label, String(system.getIntSetting(settingName, defaultValue)).c_str(), Input::AllowedDecimalCharacters, 1, 10);
        setCurrentFunction(&EditorExt::editIntFunction);
    }

    void EditorExt::editIntFunction(System& system)
    {
        switch (input.exec(system))
        {
        case Input::Accept:
        {
            const auto value = atoi(input.getValue());
            const auto valid = value != 0 || !strcmp(input.getValue(), "0");
            if (valid)
                system.setIntSetting(currentSettingName, value);
        }
            // [[fallthrough]];
        case Input::Cancel:
            resetCurrentFunction();
            break;
        }
    }

    void EditorExt::editIntEnumSetting(System& system, const char* label, const char* settingName, const char* const items[], const size_t itemCount, const int defaultValue)
    {
        currentSettingName = settingName;
        menu.reset(items, itemCount, Buttons::OkBack, system.getIntSetting(settingName, defaultValue));
        setCurrentFunction(&EditorExt::editIntEnumFunction);
    }

    void EditorExt::editIntEnumFunction(System& system)
    {
        const auto result = menu.exec(system);

        if (result >= 0)
        {
            if (result != Menu::BackIndex)
                system.setIntSetting(currentSettingName, result);
            resetCurrentFunction();
        }
    }

    void EditorExt::editStringEnumSetting(System& system, const char* label, const char* settingName, const char* const items[], const size_t itemCount, const char *const defaultValue)
    {
        currentSettingName = settingName;
        currentItems = items;

        const auto startValue = system.getStringSetting(settingName, defaultValue);

        auto startIndex = 0;
        for (size_t i = 0; i < itemCount; ++i)
            if (startValue == items[i])
                startIndex = i;

        menu.reset(items, itemCount, Buttons::OkBack, startIndex);
        setCurrentFunction(&EditorExt::editStringEnumFunction);
    }

    void EditorExt::editStringEnumFunction(System& system)
    {
        const auto result = menu.exec(system);

        if (result >= 0)
        {
            if (result != Menu::BackIndex)
                system.setStringSetting(currentSettingName, currentItems[result]);
            resetCurrentFunction();
        }
    }

}