#pragma once
#include <Windows.h>
#include <unordered_map>
#include <string>

enum class COLOR {
	PURPLE, //default html export header bg
	BLACK,
	WHITE, //main app window info text
	YELLOW, //metrics MEDIUM value
	SYS_GRAY, //main app background
	RED, //metrics HIGH value
	GREEN, //snapshot text, metrics LOW value
	BLUE,
	LIGHT_BLUE //main window system info labels 
};

static std::unordered_map<COLOR, COLORREF> colorPresetsMap = {
	{COLOR::PURPLE, RGB(128, 0, 128)}, 
	{COLOR::BLACK, RGB(0, 0, 0)},
	{COLOR::WHITE, RGB(255, 255, 255)},
	{COLOR::YELLOW, RGB(255, 255, 0)},
	{COLOR::SYS_GRAY, RGB(81, 81, 81)},
	{COLOR::RED, RGB(255, 0, 0)},
	{COLOR::GREEN, RGB(0, 255, 0)},
	{COLOR::BLUE, RGB(0, 0, 255)},
	{COLOR::LIGHT_BLUE, RGB(125, 207, 246)}
};

static COLORREF getColorFromPreset(COLOR c) {
	return colorPresetsMap.at(c);
}