#pragma once
#ifndef XY_HALO_H
#define XY_HALO_H

#include "../Module.h"
#include "../SDKSausage.h"

namespace Xylitol {

class HaloModule : public Module {
public:
    HaloModule();
    
    void onRender3D() override;
    void onUpdate() override;
    
    BoolValue* m_enable;
    ModeValue* m_haloStyle;
    FloatValue* m_size;
    FloatValue* m_spacing;
    FloatValue* m_height;
    FloatValue* m_xRot;
    FloatValue* m_yRot;
    FloatValue* m_floatingHeight;
    FloatValue* m_floatingSpeed;
    FloatValue* m_distanceScale;
    
private:
    bool m_texturesLoaded = false;
    void LoadTextures();
};

} // namespace Xylitol

#endif // XY_HALO_H
