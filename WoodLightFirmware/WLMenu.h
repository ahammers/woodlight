#pragma once

#include <array>

#include "WLButtons.h"

namespace WoodLight
{
	class System;

	class Menu
	{
	public:
		static constexpr auto BackIndex = 999;
		static constexpr auto HomeIndex = BackIndex;

	public:
		Menu() = default;
		Menu(const char *const* itemText, size_t itemCount, Buttons rightButtons, size_t initialItem = 0);

		template<int ItemCount>
		Menu(const std::array<const char*, ItemCount>& source, Buttons rightButtons, size_t initialItem = 0)
			: Menu(source.data(), source.size(), rightButtons, initialItem)
		{}

		void reset(int initialItem = 0);
		void reset(const char* const* itemText_, size_t itemCount_, Buttons rightButtons_, size_t initialItem_ = 0);

		template<int ItemCount>
		void reset(const std::array<const char*, ItemCount>& source, Buttons rightButtons, size_t initialItem = 0)
		{
			reset(source.data(), source.size(), rightButtons, initialItem);
		}

		// Verarbeitet das Men� und liefert den Index des gew�hlten Men�punktes 
		// oder einen negativen Wert wenn kein Punkt gew�hlt wurde.
		// Wurde auf "Back" getippt liefert die Methode fix 'BackIndex'
		int exec(System&);

		// Liefert den Index des aktuellen Ziels (auch wenn noch nicht so weit gescrollt wurde)
		int getTargetIndex() const;

		// Setzt das mustUpdate Flag
		void update();

	private:
		const char*const* itemText = nullptr;
		size_t itemCount = 0;
		Buttons rightButtons = Buttons::None;
		int currentPos = -1; // Die Position ist itemIndex * 8 + Scanzeile
		int targetPos = 0;
		bool mustUpdate = true;
	};
};
