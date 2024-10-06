#pragma once

#include <ZC/Objects/Camera/ZC_Camera.h>
#include <ZC/Events/ZC_EC.h>
#include <ZC/Events/ZC_ButtonID.h>

class G_Camera
{
public:
    ZC_Camera cam;

    G_Camera(ZC_Function<void(const ZC_Vec3<float>&)> _callback_camera_roatted);
    ~G_Camera();

    void SetDefaultState();
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

    const float maxDistanceToObject = 100.f,
        minDistanceToObject = 0.2f,
        sensivityScroll = 1.f;
    float distanceToObject = 15.f;
    
    const float sensitivityRotation = 1.f;
    float horizontalAngle = 0.f;    //  rotate around Z
    float verticalAngle = 0.f;        //  rotate around X
    float vertical_angle_must_be = 0.f;

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