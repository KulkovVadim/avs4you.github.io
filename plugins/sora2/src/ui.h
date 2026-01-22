#pragma once
#include "plugin.h"

#define BASE_COLOR_RGB RGB(37, 150, 190)

class MessageOutputData
{
public:
	std::string text;
};

enum class WmMainWindowCommands : WPARAM
{
	ComboModel = 0x8001,
	ComboOrientation = 0x8002,
	ComboSize = 0x8003,
	ComboDuration = 0x8004,

	ButtonGenerate = 0x8005,
	ButtonSettings = 0x8006,

	Output = 0x8007,
	OutputStop = 0x8008,
};

namespace NSUI
{
	void CenterWindow(HWND hwnd);

	LRESULT CALLBACK SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void ShowSettingsWindow(HWND hwndParent, CSoraPlugin* plugin);

	LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void ShowPromptWindow(CSoraPlugin* plugin);
} // namespace NSUI