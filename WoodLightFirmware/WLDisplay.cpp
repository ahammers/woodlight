#include "WLDisplay.h"

#include "MD_MAX72xx_lib.h"

namespace WoodLight
{
	namespace {
		static const uint8_t SmallFont0[] = { 0x53, 14, 17, 14 };
		static const uint8_t SmallFont1[] = { 0x52, 2, 31 };
		static const uint8_t SmallFont2[] = { 0x53, 29, 21, 23 };
		static const uint8_t SmallFont3[] = { 0x53, 21, 21, 10 };
		static const uint8_t SmallFont4[] = { 0x53, 0b111, 0b100, 0b11111 };
		static const uint8_t SmallFont5[] = { 0x53, 23, 21, 29 };
		static const uint8_t SmallFont6[] = { 0x53, 0b1110, 0b10101, 0b1001 };
		static const uint8_t SmallFont7[] = { 0x53, 0b1, 0b1, 0b11111 };
		static const uint8_t SmallFont8[] = { 0x53, 0b11111, 0b10101, 0b11111 };
		static const uint8_t SmallFont9[] = { 0x53, 0b10, 0b10101, 0b1110 };

		static const uint8_t* SmallFontDigits[10] = {
			SmallFont0,
			SmallFont1,
			SmallFont2,
			SmallFont3,
			SmallFont4,
			SmallFont5,
			SmallFont6,
			SmallFont7,
			SmallFont8,
			SmallFont9
		};

		static const uint8_t* getFontChar(uint8_t ch)
		{
			if (ch > _sysfont[5])
				return nullptr;

			auto ptr = _sysfont + 7;

			while (ch--)
				ptr = ptr + 1 + *ptr;

			return ptr;
		}

		static void setColPixel(uint8_t* col, int y, bool value)
		{
			if (value)
				*col |= (1 << y);
			else
				*col &= ~(1 << y);
		}
	}

	Display::Display()
	{
		clear(false);
	}

	bool Display::isChanged() const
	{
		return changed;
	}

	void Display::clearChanged()
	{
		changed = false;
	}

	void Display::setClippingRect(int x, int y, int w, int h)
	{
		setClippingRect(Rect{ x, y, x + w - 1, y + h - 1 });
	}

	void Display::setClippingRect(Rect r)
	{
		clippingRect.left = std::max(0, r.left);
		clippingRect.top = std::max(0, r.top);
		clippingRect.right = std::min(Columns - 1, r.right);
		clippingRect.bottom = std::min(Rows - 1, r.bottom);

		rect_col_mask = 0;
		for (auto y = clippingRect.top; y <= clippingRect.bottom; ++y)
			rect_col_mask |= (1 << y);
	}

	const Display::Rect& Display::getClippingRect() const
	{
		return clippingRect;
	}

	void Display::clearClippingRect()
	{
		clippingRect.left = 0;
		clippingRect.top = 0;
		clippingRect.right = Columns - 1;
		clippingRect.bottom = Rows - 1;

		rect_col_mask = 0xFF;
	}

	void Display::reset(bool value)
	{
		clearClippingRect();
		clear(value);
	}

	bool Display::getPixel(int x, int y) const
	{
		return (x >= 0 && x < Columns && y >= 0 && y < Rows)
			? (cols[x] & (1 << y))
			: false;
	}

	void Display::setPixel(int x, int y, bool value)
	{
		if (x >= clippingRect.left && x <= clippingRect.right && y >= clippingRect.top && y <= clippingRect.bottom)
		{
			if (value)
				cols[x] |= (1 << y);
			else
				cols[x] &= ~(1 << y);

			changed = true;
		}
	}

	uint8_t Display::getCol(int x) const
	{
		return (x >= 0 && x < Columns)
			? cols[x]
			: 0;
	}

	void Display::setCol(int x, uint8_t data)
	{
		if (x >= clippingRect.left && x <= clippingRect.right)
		{
			if (rect_col_mask != 0xff)
				cols[x] = (cols[x] & ~rect_col_mask) | (data & rect_col_mask);
			else
				cols[x] = data;

			changed = true;
		}
	}

	void Display::clear(bool value)
	{
		const auto pattern = value ? rect_col_mask : uint8_t(0);
		
		for (auto x = clippingRect.left; x <= clippingRect.right; ++x)
			cols[x] = (cols[x] & ~rect_col_mask) | pattern;

		changed = true;
	}

	void Display::clear(int firstCol, int w, bool value)
	{
		if (firstCol < clippingRect.left)
		{
			w += (clippingRect.left - firstCol);
			firstCol = clippingRect.left;
		}

		if (firstCol + w > clippingRect.right + 1)
			w = clippingRect.right + 1 - firstCol;

		if (w > 0)
		{
			std::fill(cols.begin() + firstCol, cols.begin() + firstCol + w, value ? 0xFF : 0x00);
			changed = true;
		}
	}

	void Display::hline(int x, int y, int w, bool value)
	{
		if (x < clippingRect.left)
		{
			w += x - clippingRect.left;
			x = clippingRect.left;
		}
		if (x + w > clippingRect.right + 1)
			w = clippingRect.right + 1 - x;

		if (y >= clippingRect.top && y <= clippingRect.bottom && w > 0)
		{
			for (int xr = 0; xr < w; ++xr)
				setPixel(x + xr, y, value);
			changed = true;
		}
	}
	void Display::vline(int x, int y, int h, bool value)
	{
		if (y < clippingRect.top)
		{
			h += y - clippingRect.top;
			y = clippingRect.top;
		}
		if (y + h > clippingRect.bottom + 1)
			h = clippingRect.bottom + 1 - y;
		if (x >= clippingRect.left && x <= clippingRect.right && h > 0)
		{
			auto& col = cols[x];
			
			if (value)
			{
				auto bit = (1 << y);
				for (int i = 0; i < h; ++i)
				{
					col |= bit;
					bit <<= 1;
				}
			} else
			{
				for (int i = 0; i < h; ++i)
					col &= ~(1 << (y + i));
			}

			changed = true;
		}
	}

	void Display::rect(int x, int y, int w, int h, bool value)
	{
		hline(x+1, y, w-2, value);
		hline(x+1, y + h - 1, w-2, value);
		vline(x, y, h, value);
		vline(x + w - 1, y, h, value);
	}

	void Display::paintPattern(int x, int y, const uint8_t* data, bool value)
	{
		if (data)
		{
			auto colsLeft = static_cast<int>(*data & 0xF);
			const auto h = static_cast<int>(*data >> 4);

			++data;

			for (; colsLeft > 0 && x <= clippingRect.right; --colsLeft)
			{
				auto& col = cols[x];

				if (x >= clippingRect.left)
				{
					for (int ry = 0; ry < h; ++ry)
					{
						if (*data & (1 << ry))
							setColPixel(&col, y + ry, value);
					}
				}
				++x;
				++data;
			}
		}

	}

	int Display::print(int x, int y, bool value, const char* text)
	{
		if (text && *text)
		{
			while (*text && x <= clippingRect.right)
			{
				if (auto fc = getFontChar(static_cast<unsigned char>(*(text++))))
				{
					auto w = *(fc++);

					for (; w > 0; --w)
					{
						if (x >= clippingRect.left && x <= clippingRect.right)
						{
							for (int ry = 0; ry < 8; ++ry)
							{
								if (*fc & (1 << ry))
									setPixel(x, y + ry, value);
							}
						}

						++x;
						++fc;
					}

					++x;
				}
			}
			changed = true;
		}

		return x;
	}

	int Display::getTextWidth(char ch)
	{
		if (auto fc = getFontChar(ch))
			return *fc + 1;
		else
			return 0;
	}

	int Display::getTextWidth(const char* text, size_t chars)
	{
		auto w = 0;

		if (text && *text)
		{
			while (*text && chars--)
			{
				if (auto fc = getFontChar(*(text++)))
				{
					const auto cw = *(fc++);
					fc += cw;
					w = w + 1 + cw;
				}
			}

			if (w > 0)
				--w;
		}

		return w;
	}

	int Display::printSmall(int x, int y, bool value, const char* text)
	{
		if (text && y < Rows && y > -8 && x < Columns)
		{
			for (; *text && x < Columns; ++text)
			{
				if (*text >= '0' && *text <= '9')
				{
					auto pattern = SmallFontDigits[*text - '0'];
					paintPattern(x, y, pattern, value);

					x = x + 1 + (*pattern & 0xF);
				}
			}
		}

		return x;
	}

	int Display::getSmallTextWidth(char ch)
	{
		if (ch >= '0' && ch <= '9')
			return SmallFontDigits[ch - '0'][0] & 0xF;
		else
			return 0;
	}

	int Display::getSmallTextWidth(const char* text, size_t chars)
	{
		auto result = 0;

		for (; *text && chars--; ++text)
		{
			if (*text >= '0' && *text <= '9')
				result = result + 1 + (SmallFontDigits[*text - '0'][0] & 0xF);
		}

		return result;
	}

}
