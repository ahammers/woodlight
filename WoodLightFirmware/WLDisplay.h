#pragma once

#include <array>

namespace WoodLight
{
	class Display
	{
	public:
		class Rect
		{
		public:
			int left, top, right, bottom;
		};

	public:
		static constexpr auto Columns = 8 * 15;
		static constexpr auto Rows = 8;

		//static_assert(Rows <= 8);

	public:
		Display();

		bool isChanged() const;
		void clearChanged();

		void setClippingRect(int x, int y, int w, int h);
		void setClippingRect(Rect);
		const Rect& getClippingRect() const;
		void clearClippingRect();

		void reset(bool value);

		bool getPixel(int x, int y) const;
		void setPixel(int x, int y, bool value);

		uint8_t getCol(int x) const;
		void setCol(int x, uint8_t);

		void clear(bool value);
		void clear(int firstCol, int w, bool value);

		void hline(int x, int y, int w, bool value);
		void vline(int x, int y, int h, bool value);
		void rect(int x, int y, int w, int h, bool value);

		// Zeichnet ein Icon oder ein Zeichen
		void paintPattern(int x, int y, const uint8_t* data, bool value);

		int print(int x, int y, bool value, const char* text);
		static int getTextWidth(char);
		static int getTextWidth(const char* text, size_t l = 2000000000) ;

		int printSmall(int x, int y, bool value, const char* text);
		static int getSmallTextWidth(char);
		static int getSmallTextWidth(const char* text, size_t l = 2000000000);

	public:
		bool changed = true;
		std::array<uint8_t, Columns> cols;
		Rect clippingRect{ 0, 0, Columns - 1, Rows - 1 };
		uint8_t rect_col_mask = 0xff;
	};
}
