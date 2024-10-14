#pragma once

#include <ZC/Video/OpenGL/Renderer/ZC_DrawerSet.h>

class G_ParticlesDrawer
{
public:
        //  - particles_count - will reserved place in particle_positions and particle_colors vectors
    virtual ~G_ParticlesDrawer();
    
        //  start/stop drawing
    void SetDrawState(bool need_draw);
    void SetAlpha(float alpha);
    float GetAlpha() const noexcept;
    bool IsDrawing();

protected:
    G_ParticlesDrawer(unsigned long particles_count, ZC_DrawerLevel _drawer_level, int point_size);
    
        //  emplace in vectors: particle_positions pos, in particle_colors color. In reference on pointers (rpPos, rpColor) sets relevant pointers from vectors.
    void AddPositionAndColor(const ZC_Vec4<float>& pos, const ZC_Vec3<unsigned char>& color, ZC_Vec4<float>*& rpPos, ZC_Vec3<unsigned char>*& rpColor);
    
    static ZC_Vec3<unsigned char> CalculateColor(const ZC_Vec3<unsigned char>& color_start, const ZC_Vec3<unsigned char>& color_end);

private:
    std::vector<ZC_Vec4<float>> particle_positions;     //  store all positions, last is alpha channel for gpu update with vertices, colors don't updates
    std::vector<ZC_Vec3<unsigned char>> particle_colors;    //  store all color
    
    ZC_DrawerSet drawer_set;
    ZC_DSController ds_con;
    ZC_DrawerLevel drawer_level;

    float unAlpha = 1.f;

    ZC_EC ec_updater;
        //  must return true if updated
    virtual bool UpdateParticlesPositions(float time) = 0;
    virtual bool UpdateParticlesColors(float time) { return false; }
    virtual void DrawingStopped() {}

    ZC_DrawerSet CreateDrawerSet(unsigned long particles_count);

    void Callback_Updater(float time);

    void GLSubDataPositions();
    void GLSubDataColors();
};