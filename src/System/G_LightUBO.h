#pragma once

#include "G_LightSet.h"
#include <ZC/Video/OpenGL/Buffer/ZC_UBO.h>
#include <ZC/Tools/ZC_uptr.h>

#include <vector>

enum G_LightName
{
    G_LN__Star = 0,
    G_LN__Platform = 1,

    G_LN__sizeof
};

enum G_LightAttenuationName
{
    G_LAN_PlayerSphere = 0,
    G_LAN_Platform = 1,
    G_LAN_CubeMap = 2,

    G_LAN_sizeof
};

struct G_LightAttenuation
{
    float linear;
    float quadratic;
};

class G_LightUBO
{
public:
    static void Init();
    static void Destroy();

    static void UpdateLightData(G_LightName light_name, const G_LightSet& light_set);
    static void UpdateAttenuation(G_LightAttenuationName att_name, const G_LightAttenuation& attenuation);

private:
    static inline ZC_uptr<ZC_UBO> upUBO_light;
    struct Light
    {
        G_LightSet light_sets[G_LN__sizeof];
        G_LightAttenuation attenuations[G_LAN_sizeof];
    } static inline light;

    static inline bool update_light = false;
    static inline bool update_attenuation = false;

    static void Callback_UBOLightUpdate();

public:
    static inline const Light& c_light = light;
};




// #include <ZC/GUI/ZC_GUI__Window.h>
// #include <ZC/GUI/Button/ZC_GUI__ButtonNumberText.h>
// struct TestLight
// {
//     struct TestParams
//     {
//         float test_atten_linear = 3.f;
//         float test_atten_quadratic = 0.44f;
//         float test_ambient = 0.04f;
//         float test_difuse = 0.7f;
//         float test_specular = 1.2f;
//         float test_spec_pow = 8.f;
//     } test_params;
//     static inline float test_light_offset = 9.5f;

//     ZC_GUI__WinImmutable win;
//     ZC_GUI__ButtonNumberText<float> bnt_test_atten_linear;
//     ZC_GUI__ButtonNumberText<float> bnt_test_atten_quadratic;
//     ZC_GUI__ButtonNumberText<float> bnt_test_ambient;
//     ZC_GUI__ButtonNumberText<float> bnt_test_difuse;
//     ZC_GUI__ButtonNumberText<float> bnt_test_specular;
//     ZC_GUI__ButtonNumberText<float> bnt_test_spec_pow;
//     ZC_GUI__ButtonNumberText<float> bnt_test_light_offset;

//     TestLight()
//         : test_params{},
//         win(ZC_WOIData(330.f, 200.f, 0.f, 0.f, ZC_WOIF__X_Right_Pixel | ZC_WOIF__Y_Top_Pixel), ZC_GUI_WF__NeedDraw | ZC_GUI_WF__Scrollable),
//         bnt_test_atten_linear(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.test_atten_linear, 0.f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &TestLight::Atten_linear, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"atten_linear", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
//         bnt_test_atten_quadratic(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.test_atten_quadratic, 0.f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &TestLight::Atten_quadratic, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"atten_quadratic", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
//         bnt_test_ambient(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.test_ambient, 0.f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &TestLight::Ambient, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"ambient", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
//         bnt_test_difuse(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.test_difuse, 0.f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &TestLight::Difuse, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"difuse", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
//         bnt_test_specular(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.test_specular, 0.f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &TestLight::Specular, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"specular", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
//         bnt_test_spec_pow(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.test_spec_pow, 0.f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &TestLight::Spec_pow, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"spec_pow", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
//         bnt_test_light_offset(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_light_offset, 0.f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &TestLight::Light_offset, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"light_offset", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0)))
//     {
//         win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_test_atten_linear.GetObj() }));
//         win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_test_atten_quadratic.GetObj() }));
//         win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_test_ambient.GetObj() }));
//         win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_test_difuse.GetObj() }));
//         win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_test_specular.GetObj() }));
//         win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_test_spec_pow.GetObj() }));
//         win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_test_light_offset.GetObj() }));
//     }

//     void Atten_linear(float v)
//     {
//         test_params.test_atten_linear = v;
//     }
//     void Atten_quadratic(float v)
//     {
//         test_params.test_atten_quadratic = v;
//     }
//     void Ambient(float v)
//     {
//         test_params.test_ambient = v;
//     }
//     void Difuse(float v)
//     {
//         test_params.test_difuse = v;
//     }
//     void Specular(float v)
//     {
//         test_params.test_specular = v;
//     }
//     void Spec_pow(float v)
//     {
//         test_params.test_spec_pow = v;
//     }
//     void Light_offset(float v)
//     {
//         test_light_offset = v;
//     }
// };