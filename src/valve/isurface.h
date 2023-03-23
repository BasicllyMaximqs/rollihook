#pragma once
#include <windows.h>
#include "../util/memory.h"
#include "../../gui.h"

enum FontDrawType
{
	FONT_DRAW_DEFAULT = 0,

	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,

	FONT_DRAW_TYPE_COUNT = 2,
};

using HFont = unsigned long;
enum EFontFlag
{
	FONT_FLAG_NONE,
	FONT_FLAG_ITALIC = 0x001,
	FONT_FLAG_UNDERLINE = 0x002,
	FONT_FLAG_STRIKEOUT = 0x004,
	FONT_FLAG_SYMBOL = 0x008,
	FONT_FLAG_ANTIALIAS = 0x010,
	FONT_FLAG_GAUSSIANBLUR = 0x020,
	FONT_FLAG_ROTARY = 0x040,
	FONT_FLAG_DROPSHADOW = 0x080,
	FONT_FLAG_ADDITIVE = 0x100,
	FONT_FLAG_OUTLINE = 0x200,
	FONT_FLAG_CUSTOM = 0x400,
	FONT_FLAG_BITMAP = 0x800,
};

class ISurface
{
public:
	constexpr void DrawSetColor(int r, int g, int b, int a = 255) noexcept
	{
		memory::Call<void>(this, 15, r, g, b, a);
	}

	constexpr void DrawFilledRect(int x, int y, int xx, int yy) noexcept
	{
		memory::Call<void>(this, 16, x, y, xx, yy);
	}

	constexpr void DrawOutlinedRect(int x, int y, int xx, int yy) noexcept
	{
		memory::Call<void>(this, 18, x, y, xx, yy);
	}
	constexpr void DrawLine(int x0, int y0, int x1, int y1) noexcept
	{
		memory::Call<void>(this, 19, x0, y0, x1, y1);
	}

	constexpr void DrawLine(CVector coords_start, CVector coords_end) noexcept
	{
		memory::Call<void>(this, 19, static_cast<int>(coords_start.x), static_cast<int>(coords_start.y), static_cast<int>(coords_end.x), static_cast<int>(coords_end.y));
	}

	HFont FontCreate() {
		return memory::Call<HFont>(this, 71);
	}

	CVector GetTextSize(HFont font, const wchar_t* text) {
		int wide, tall;
		memory::Call<void>(this, 79, font, text, &wide, &tall);
		return CVector(wide, tall);
	}


	constexpr void SetFontGlyphSet(HFont font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin = 0, int nRangeMax = 0) {
		memory::Call<void>(this, 72, font, windowsFontName, tall, weight, blur, scanlines, flags, nRangeMin, nRangeMax);
	}

	constexpr void DrawSetTextFont(HFont font) // 24
	{
		memory::Call<void>(this, 23, font);
	}
	constexpr void DrawSetTextColor(int r, int g, int b, int a)
	{
		memory::Call<void>(this, 25,r,g,b,a);
	}
	constexpr void DrawSetTextPos(int x, int y)
	{
		memory::Call<void>(this, 26,x,y);
	}
	constexpr void DrawGetTextPos(int& x, int& y)
	{
		memory::Call<void>(this, 27,x,y);
	}
	constexpr void DrawPrintText(const std::wstring_view text, int textLen, FontDrawType drawType = FONT_DRAW_DEFAULT)
	{
	    memory::Call<void>(this, 28,text,textLen);
	}
	constexpr void UnlockCursor()
	{
		return memory::Call<void>(this, 66);
	}
	constexpr void LockCursor()
	{
		return memory::Call<void>(this, 67);
	}

};