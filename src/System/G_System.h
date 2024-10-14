#pragma once

#include "G_Config.h"

class G_System
{
public:
    G_System();

    void RunMainCycle();

private:
    G_Config config;

    void LoadSounds();
    void SetUpGUI();
};