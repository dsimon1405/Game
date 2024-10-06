#include "G_Camera.h"

#include <ZC/Events/ZC_Events.h>
#include <ZC/Video/ZC_SWindow.h>
#include <ZC/Tools/Math/ZC_Math.h>
#include <System/G_UpdaterLevels.h>

G_Camera::G_Camera(ZC_Function<void(const ZC_Vec3<float>&)> _callback_camera_rotated)
    : cam(CreateCamera()),
    callback_camera_roatted(std::move(_callback_camera_rotated))
{
    cam.MakeActive();
    SetDefaultState();
}

G_Camera::~G_Camera()
{
    ec_mouseMove.Disconnect();
    ec_mouseScroll.Disconnect();
    ec_updater.Disconnect();
}

void G_Camera::SetDefaultState()
{
    horizontalAngle = 0.f;
    verticalAngle = 0.f;
    MouseMoveCallback(0.f, 0.f, 0.f, -30.f, 1.f);
}

void G_Camera::SetCameraLookOn(const ZC_Vec3<float>& lookOn)
{
    auto curLookOn = cam.GetLookOn();
    cam.SetPosition(lookOn + (cam.GetPosition() - curLookOn));
    cam.SetLookOn(lookOn);
}

void G_Camera::MoveCamera(const ZC_Vec3<float>& offset)
{
    cam.SetLookOn(cam.GetLookOn() + offset);
    cam.SetPosition(cam.GetPosition() + offset);
}

void G_Camera::SetConnectionToEvents(bool use_events)
{
    if (use_events == ec_mouseMove.IsConnected()) return;
    if (use_events)
    {
        ec_mouseMove.NewConnection(ZC_Events::ConnectMouseMoveOnceInFrame({ &G_Camera::MouseMoveCallback, this }));      //  disconnect scroll, connect mouse move callback
        ec_mouseScroll.NewConnection(ZC_Events::ConnectMouseScrollOnceInFrame({ &G_Camera::MouseWheelScroll, this }));
    }
    else
    {
        ec_mouseMove.Disconnect();
        ec_mouseScroll.Disconnect();
    }
}

float G_Camera::GetHorizontalRotationAngle() const
{
    return horizontalAngle;
}

void G_Camera::RotateCameraHorizontal(float angle)
{
    horizontalAngle += angle;
    RotateAroundObject();
    callback_camera_roatted(cam.GetPosition());
}

ZC_Camera G_Camera::CreateCamera()
{
    int width, height;
    ZC_SWindow::GetSize(width, height);
            //  no metter what sets in cam pos here. Camera position sets throught MouseMoveCallback() rotation, and MouseWheelScroll() distance
    return ZC_Camera({ 0, 5, 0 }, {0.f, 0.f, 0.f}, {0.f, 0.f, 1.f}, ZC_Perspective(45.f, static_cast<float>(width) / static_cast<float>(height), 0.01f, 1000.f),
        ZC_Ortho(0.f, static_cast<float>(width), 0.f, static_cast<float>(height)), true);
}

void G_Camera::MouseMoveCallback(float x, float y, float xRel, float yRel, float time)
{
    // if (xRel == 0 && yRel == 0) return;
    // horizontalAngle += yRel * sensitivityRotation;
    // if (horizontalAngle >= 90.f) horizontalAngle = 89.9f;
    // else if (horizontalAngle <= -90.f) horizontalAngle = -89.9f;
    // verticalAngle += xRel * sensitivityRotation;
    // if (verticalAngle >= ZC_angle_360f) verticalAngle -= ZC_angle_360f;
    // else if (verticalAngle <= - ZC_angle_360f) verticalAngle += ZC_angle_360f;

    const float vertical_max = 10.f;
    const float vertical_min = 30.f;

    if (yRel != 0.f)
    {
        float new_anle = vertical_angle_must_be + (sensitivityRotation * - yRel);
        new_anle = new_anle > vertical_max ? vertical_max : new_anle < vertical_min ? vertical_min : new_anle;
        if (new_anle != vertical_angle_must_be)
        {
            vertical_angle_must_be = new_anle;
            ec_updater = ZC_SWindow::ConnectToUpdater({ &G_Camera::Callback_Updater, this }, G_UL__game_play);
        }
    }
    verticalAngle += sensitivityRotation * - yRel;

    if (verticalAngle >= vertical_max) verticalAngle = vertical_max;
    else if (verticalAngle <= vertical_min) verticalAngle = vertical_min;
    horizontalAngle += sensitivityRotation * xRel;
    if (horizontalAngle >= ZC_angle_360f) horizontalAngle -= ZC_angle_360f;
    else if (horizontalAngle <= - ZC_angle_360f) horizontalAngle += ZC_angle_360f;

    RotateAroundObject();

    callback_camera_roatted(cam.GetPosition());
}

void G_Camera::RotateAroundObject()
{
    // //  manual rotation variant
    // float camX = 0.f,
    //       camY = 0.f,
    //       camZ = 0.f;
    // //  vertical rotaion (rotate around horizont)
    // float verticalRad = ZC_Vec::Radians(verticalAngle);
    // camX = distanceToObject * cos(verticalRad);
    // camZ = distanceToObject * sin(verticalRad);
    // //  horizontal rotation (rotate around world up vector)
    // float horizontalRad = ZC_Vec::Radians(horizontalAngle);
    // camY = camX * sin(horizontalRad);
    // camX = camX * cos(horizontalRad);

    // auto lookOn = camera.GetLookOn();
    // camera.SetPosition({ lookOn[0] + camX, lookOn[1] - camY, lookOn[2] + camZ });    //  on start positive X, positive Y, positive Z, if positive camX, camY, camZ

    //  analogue with rotation of the model matrix!
    ZC_Mat4<float> newModel(1.f);
    newModel.Rotate(horizontalAngle, { 0.f, 0.f, -1.f }).Rotate(verticalAngle, { -1.f, 0.f, 0.f });
    ZC_Vec4<float> camPos = newModel * ZC_Vec4<float>(0.f, -distanceToObject, 0.f, 1.f);    //  start position on -y for capatability with ZC_TurnToCamera
    auto lookOn = cam.GetLookOn();
    cam.SetPosition({ lookOn[0] + camPos[0], lookOn[1] + camPos[1], lookOn[2] + camPos[2] });

    isDirsActual = false;
}

void G_Camera::CalculateDirections()
{
    if (isDirsActual) return;
    dirFront = ZC_Vec::Normalize(cam.GetLookOn() - cam.GetPosition());
    dirRight = ZC_Vec::Normalize(ZC_Vec::Cross(dirFront, {0.f,0.f,1.f}));    //  world up
    dirUp = ZC_Vec::Normalize(ZC_Vec::Cross(dirRight, dirFront));
    isDirsActual = true;
}

void G_Camera::MouseWheelScroll(float rotationHorizontal, float rotationVertical, float time)
{
    if (rotationVertical == 0.f || (distanceToObject == minDistanceToObject && rotationVertical > 0.f) ||
        (distanceToObject == maxDistanceToObject && rotationVertical < 0.f)) return;
    CalculateDirections();
    float newDistance = distanceToObject - (rotationVertical * sensivityScroll);
    if (newDistance >= maxDistanceToObject)
    {
        distanceToObject = maxDistanceToObject;
        cam.SetPosition(cam.GetLookOn() - (dirFront * maxDistanceToObject));
    }
    else if (newDistance <= minDistanceToObject)
    {
        distanceToObject = minDistanceToObject;
        cam.SetPosition(cam.GetLookOn() - (dirFront * distanceToObject));
    }
    else
    {
        distanceToObject = newDistance;
        cam.SetPosition(cam.GetLookOn() - (dirFront * distanceToObject));
    }
}

void G_Camera::Callback_Updater(float time)
{
    static const float vectical_angle_rotation_speed = 0.3f / 1.f;    //  angle/sec

    if (verticalAngle < vertical_angle_must_be)
    {
        verticalAngle += 
    }
    else
    {

    }
}