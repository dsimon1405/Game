#pragma once

#include "G_Config.h"

class G_Window
{
public:
    G_Window();

    void RunMainCycle();

private:
    G_Config config;

    void LoadSounds();
};