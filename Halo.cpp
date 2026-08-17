#include "Halo.h"
#include "../SDKSausage.h"
#include "../TextureLoader.h"
#include "../halo_resources.h"
#include <cmath>
#include <algorithm>

namespace Xylitol {

static TextureData* g_haloTextures[8] = { nullptr };
static float g_animTime = 0.0f;

HaloModule::HaloModule()
    : Module("蔚蓝光环", "在玩家头顶渲染类似 Blue Archive 的光环效果", ModuleCategory::Render) {
    
    m_enable           = new BoolValue("启用", "enable", true);
    m_haloStyle        = new ModeValue("光环样式", "haloStyle", {"白子", "芹香", "星野", "Logo"}, 0);
    m_size             = new FloatValue("大小", "size", 1.0f, 0.3f, 3.0f, 0.1f);
    m_spacing          = new FloatValue("层间距", "spacing", 0.08f, 0.01f, 0.3f, 0.01f);
    m_height           = new FloatValue("高度", "height", 1.6f, 1.0f, 2.5f, 0.1f);
    m_xRot             = new FloatValue("X旋转", "xRot", 0.0f, -90.0f, 90.0f, 5.0f);
    m_yRot             = new FloatValue("Y旋转", "yRot", 0.0f, -90.0f, 90.0f, 5.0f);
    m_floatingHeight   = new FloatValue("浮动高度", "floatingHeight", 0.15f, 0.0f, 0.5f, 0.05f);
    m_floatingSpeed    = new FloatValue("浮动速度", "floatingSpeed", 1.0f, 0.2f, 3.0f, 0.2f);
    m_distanceScale    = new FloatValue("距离缩放", "distanceScale", 1.0f, -3.0f, 3.0f, 0.1f);
    
    addValue(m_enable);
    addValue(m_haloStyle);
    addValue(m_size);
    addValue(m_spacing);
    addValue(m_height);
    addValue(m_xRot);
    addValue(m_yRot);
    addValue(m_floatingHeight);
    addValue(m_floatingSpeed);
    addValue(m_distanceScale);
}

void HaloModule::LoadTextures() {
    if (m_texturesLoaded) return;
    
    g_haloTextures[0] = TextureLoader::GetInstance().LoadTextureFromMemory(
        HaloResources::shiroko_layer0, HaloResources::shiroko_layer0_size, "shiroko0");
    g_haloTextures[1] = TextureLoader::GetInstance().LoadTextureFromMemory(
        HaloResources::shiroko_layer1, HaloResources::shiroko_layer1_size, "shiroko1");
    g_haloTextures[2] = TextureLoader::GetInstance().LoadTextureFromMemory(
        HaloResources::serika_layer0, HaloResources::serika_layer0_size, "serika0");
    g_haloTextures[3] = TextureLoader::GetInstance().LoadTextureFromMemory(
        HaloResources::serika_layer1, HaloResources::serika_layer1_size, "serika1");
    g_haloTextures[4] = TextureLoader::GetInstance().LoadTextureFromMemory(
        HaloResources::hoshino_layer0, HaloResources::hoshino_layer0_size, "hoshino0");
    g_haloTextures[5] = TextureLoader::GetInstance().LoadTextureFromMemory(
        HaloResources::hoshino_layer1, HaloResources::hoshino_layer1_size, "hoshino1");
    g_haloTextures[6] = TextureLoader::GetInstance().LoadTextureFromMemory(
        HaloResources::hoshino_layer2, HaloResources::hoshino_layer2_size, "hoshino2");
    g_haloTextures[7] = TextureLoader::GetInstance().LoadTextureFromMemory(
        HaloResources::halo_logo, HaloResources::halo_logo_size, "halo_logo");
    
    m_texturesLoaded = true;
    printf("[Halo] All embedded textures loaded\n");
}

void HaloModule::onUpdate() {
    if (!m_texturesLoaded) {
        LoadTextures();
    }
}

void HaloModule::onRender3D() {
    if (!getState() || !m_enable->get()) return;
    
    if (!SausageSDK::IsSausageReady()) return;
    
    if (!m_texturesLoaded) {
        LoadTextures();
    }
    
    auto* drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) return;
    
    SausageSDK::Vector3 camPos = SausageSDK::SDKSausage_GetCameraPosition();
    
    g_animTime += ImGui::GetIO().DeltaTime * m_floatingSpeed->get();
    float floatOffset = sinf(g_animTime * 3.0f) * m_floatingHeight->get();
    
    auto players = SausageSDK::SDKSausage_GetAllPlayersForRender();
    
    int style = m_haloStyle->get();
    int startIdx = 0;
    int layerCount = 1;
    
    switch (style) {
        case 0: startIdx = 0; layerCount = 2; break;
        case 1: startIdx = 2; layerCount = 2; break;
        case 2: startIdx = 4; layerCount = 3; break;
        case 3: startIdx = 7; layerCount = 1; break;
    }
    
    for (size_t i = 0; i < players.size(); i++) {
        uintptr_t player = players[i];
        if (player == 0) continue;
        
        SausageSDK::PlayerData pData = SausageSDK::SDKSausage_ReadPlayerForRender(player);
        if (!pData.isValid) continue;
        
        float haloHeight = m_height->get() + floatOffset;
        SausageSDK::Vector3 haloCenter = {
            pData.position.x,
            pData.position.y + haloHeight,
            pData.position.z
        };
        
        float dist = SausageSDK::SDKSausage_GetDistance(camPos, haloCenter);
        if (dist > 50.0f) continue;
        
        float scale = 1.0f;
        if (dist > 5.0f) {
            scale = 5.0f / dist;
        }
        scale *= m_size->get();
        scale *= m_distanceScale->get();
        
        float size = 0.5f * scale;
        
        for (int layer = 0; layer < layerCount; layer++) {
            int texIdx = startIdx + layer;
            if (texIdx < 0 || texIdx >= 8) continue;
            
            TextureData* tex = g_haloTextures[texIdx];
            if (!tex || !tex->SRV) continue;
            
            float layerOffset = (float)layer * m_spacing->get() * 0.1f;
            float layerSize = size * (1.0f - layerOffset * 0.5f);
            
            SausageSDK::Vector3 layerCenter = {
                haloCenter.x,
                haloCenter.y + layerOffset * 0.5f,
                haloCenter.z
            };
            
            SausageSDK::Vector3 corners[4];
            
            // 垂直于地面的平面（X-Z 平面）
            corners[0] = { layerCenter.x - layerSize, layerCenter.y, layerCenter.z - layerSize }; // 左下
            corners[1] = { layerCenter.x + layerSize, layerCenter.y, layerCenter.z - layerSize }; // 右下
            corners[2] = { layerCenter.x + layerSize, layerCenter.y, layerCenter.z + layerSize }; // 右上
            corners[3] = { layerCenter.x - layerSize, layerCenter.y, layerCenter.z + layerSize }; // 左上
            
            ImVec2 screenPts[4];
            bool allOnScreen = true;
            for (int j = 0; j < 4; j++) {
                SausageSDK::Vector2 screenPt;
                if (!SausageSDK::SDKSausage_WorldToScreen(corners[j], screenPt)) {
                    allOnScreen = false;
                    break;
                }
                screenPts[j] = ImVec2(screenPt.x, screenPt.y);
            }
            
            if (!allOnScreen) continue;
            
            float alpha = 0.95f - (float)layer * 0.05f;
            ImU32 col = IM_COL32(255, 255, 255, (ImU32)(alpha * 255.0f));
            
            ImVec2 uv[4] = {
                ImVec2(0, 1), // 左下
                ImVec2(1, 1), // 右下
                ImVec2(1, 0), // 右上
                ImVec2(0, 0)  // 左上
            };
            
            drawList->AddImageQuad(
                tex->SRV,
                screenPts[0], screenPts[1], screenPts[2], screenPts[3],
                uv[0], uv[1], uv[2], uv[3],
                col
            );
        }
    }
}

} // namespace Xylitol
