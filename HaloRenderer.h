#pragma once
#ifndef HALORENDERER_H
#define HALORENDERER_H

#include "../Base.h"

namespace Xylitol {

// HaloRenderer 函数
bool  HaloRenderer_Init(ID3D11Device* pDevice);
void  HaloRenderer_Shutdown();
void  HaloRenderer_NewFrame();
void  HaloRenderer_Render();
void  HaloRenderer_InstallHook(ID3D11DeviceContext* pContext);
void  HaloRenderer_UninstallHook();

} // namespace Xylitol

#endif // HALORENDERER_H
