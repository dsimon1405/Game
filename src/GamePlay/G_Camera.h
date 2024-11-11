#pragma once

#include <ZC/Objects/Camera/ZC_Camera.h>
#include <ZC/Events/ZC_EC.h>
#include <ZC/Events/ZC_ButtonID.h>

class G_Camera
{
public:
    ZC_Camera cam;

    G_Camera(ZC_Function<void(const ZC_Vec3<float>&)>&& _callback_camera_roatted, const ZC_Vec3<float>& look_on);
    ~G_Camera();

    void SetDefaultState(const ZC_Vec3<float>& look_on);
    //  Change camera's look on. New position of camera save previous position's (camPos - lookOn) vector.
    void SetCameraLookOn(const ZC_Vec3<float>& lookOn);
    void MoveCamera(const ZC_Vec3<float>& lookOn);
    
    void SetConnectionToEvents(bool use);
        //  rotate around Z axis
    float GetHorizontalRotationAngle() const;
    void RotateCameraHorizontal(float angle);

private:
    ZC_Function<void(const ZC_Vec3<float>&)> callback_camera_roatted;

    ZC_Vec3<float> dirFront,
        dirUp,
        dirRight;
    bool isDirsActual = false;

    const float maxDistanceToObject = 30.f,
        // minDistanceToObject = 0.1f,
        minDistanceToObject = 5.f,
        sensivityScroll = 1.f;
    float distanceToObject = 20.f;
        //  rotation
            //  rotate around Z
    const float angle_1800f = ZC_angle_360f * 5.f;
    const float angle_1440f = ZC_angle_360f * 4.f;
    float cur_horizontal_angle = 0.f;
    float horizontal_angle_must_be = 0.1f;
            //  rotate around X
    const float vertical_angle_max = 30.f;
    const float vertical_angle_min = 10.f;
    // const float vertical_angle_max = 89.f;
    // const float vertical_angle_min = -89.f;
    const float vertical_angle_start_pos = vertical_angle_min + ((vertical_angle_max - vertical_angle_min) / 2.f);
    float cur_vertical_angle = vertical_angle_start_pos;
    float vertical_angle_must_be = vertical_angle_start_pos;

    ZC_EC ec_mouseMove;
    ZC_EC ec_mouseScroll;

    ZC_EC ec_updater;

    ZC_Camera CreateCamera();
    void MouseMoveCallback(float x, float y, float xRel, float yRel, float time);
    void SetVerticalAngleMoreOrLessThan90(bool isMoreThan90);
    void RotateAroundObject();
    void CalculateDirections();
    void MouseWheelDown(ZC_ButtonID buttonId, float time);
    void MouseWheelUp(ZC_ButtonID buttonId, float time);
    void MouseWheelScroll(float rotationHorizontal, float rotationVertical, float time);

    void Callback_Updater(float time);
};