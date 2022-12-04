#pragma once

namespace WoodLight
{
	class System;

	enum class Buttons
	{
		None,
		UpDown,
		LeftRight,
		EnterDelete,
		SetupBack,
		OkBack,
		OkHome,
		Enter,
		Ok,
		Cancel,
		OkCancel
	};

	namespace DrawButtons
	{
		static constexpr int KeepCenter = 1;
		static constexpr int BlackAfterTimeout = 2;
		static constexpr int HideAfterTimeout = 4;

	}
	void drawButtons(System& system, Buttons left, Buttons right, int flags = 0);
}
