#include "WLMenu.h"

#include "WLSystem.h"
#include "WLButtons.h"

namespace WoodLight
{
	Menu::Menu(const char*const* itemText_, size_t itemCount_, Buttons rightButtons_, size_t initialItem_)
		: itemText(itemText_),
		itemCount(itemCount_),
        rightButtons(rightButtons_),
        currentPos(initialItem_ * 8),
        targetPos(initialItem_ * 8)
	{}

    void Menu::reset(int initialItem)
    {
        currentPos = targetPos = initialItem * 8;
    }

    void Menu::reset(const char* const* itemText_, size_t itemCount_, Buttons rightButtons_, size_t initialItem_)
    {
        itemText = itemText_;
        itemCount = itemCount_;
        rightButtons = rightButtons_;
        targetPos = currentPos = initialItem_ * 8;
    }

	// Verarbeitet das Men� und liefert den Index des gew�hlten Men�punktes 
	// oder einen Wert wenn kein Punkt gew�hlt wurde
	int Menu::exec(System& system)
	{
        auto result = -1;

        auto targetIndex = targetPos / 8;

        while (auto key = system.getkey())
        {
            switch (key)
            {
            case System::UpperLeftKeyCode:
                targetIndex = (targetIndex + itemCount - 1) % itemCount;
                break;
            case System::UpperLeftKeyCode | System::LongKeyPress:
                targetIndex = 0;
                break;
            case System::LowerLeftKeyCode:
                targetIndex = (targetIndex + 1) % itemCount;
                break;
            case System::LowerLeftKeyCode | System::LongKeyPress:
                targetIndex = itemCount - 1;
                break;

            case System::UpperRightKeyCode:
            case System::UpperRightKeyCode | System::LongKeyPress:
                switch (rightButtons)
                {
                case Buttons::Enter:
                case Buttons::OkBack:
                case Buttons::OkHome:
                case Buttons::OkCancel:
                    result = targetIndex;
                    break;
                }
                break;

            case System::LowerRightKeyCode:
            case System::LowerRightKeyCode | System::LongKeyPress:
                switch (rightButtons)
                {
                case Buttons::Enter:
                    result = targetIndex;
                    break;
                case Buttons::OkBack:
                    result = BackIndex;
                    break;
                case Buttons::OkHome:
                    result = HomeIndex;
                    break;
                }
                break;
            }
        }

        targetPos = targetIndex * 8;

        if (currentPos != targetPos)
        {
            auto step = (targetPos - currentPos) / 2;
            if (step == 0)
                step = (targetPos > currentPos) ? 1 : -1;

            currentPos += step;

            mustUpdate = true;

        }

        if (mustUpdate)
        {
            mustUpdate = false;

            system.display.clear(false);

            drawButtons(system, Buttons::UpDown, rightButtons);

            system.display.print(10, -(currentPos % 8), true, itemText[currentPos / 8]);
            if (currentPos % 8)
                system.display.print(10, 8 - (currentPos % 8), true, itemText[currentPos / 8 + 1]);
        }

        // Wurde eine Funktion gew�hlt dann merken wir uns da� das Men�
        // bei der n�chsten Verwendung auf jeden Fall neu ausgegeben wird.
        if (result >= 0)
            mustUpdate = true;

        return result;
	}

    int Menu::getTargetIndex() const
    {
        return targetPos / 8;
    }

    // Setzt das mustUpdate Flag
    void Menu::update()
    {
        mustUpdate = true;
    }


};
