#pragma once

#include "Engine/Scripting/Script.h"

API_CLASS() class GAME_API TestScript : public Script
{
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE( TestScript );

    void OnEnable(
        ) override;
};
