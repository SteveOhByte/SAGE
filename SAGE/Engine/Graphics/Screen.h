#pragma once
#include <Windows.h>

class Screen
{
public:
    static int GetWidth()
    {
        RECT desktop;
        const HWND hDesktop = GetDesktopWindow();
        GetWindowRect(hDesktop, &desktop);
        return desktop.right - desktop.left;
    }

    static int GetHeight()
    {
        RECT desktop;
        const HWND hDesktop = GetDesktopWindow();
        GetWindowRect(hDesktop, &desktop);
        return desktop.bottom - desktop.top;
    }

    static float GetCentreX() { return GetWidth() * 0.5f; }
    static float GetCentreY() { return GetHeight() * 0.5f; }

    static float GetAspectRatio() { return static_cast<float>(GetWidth()) / GetHeight(); }
};
