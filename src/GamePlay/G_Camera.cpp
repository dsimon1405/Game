#include "G_Camera.h"

#include <ZC/Events/ZC_Events.h>
#include <ZC/Video/ZC_SWindow.h>
#include <ZC/Tools/Math/ZC_Math.h>
#include <System/G_UpdaterLevels.h>

G_Camera::G_Camera(ZC_Function<void(const ZC_Vec3<float>&)>&& _callback_camera_rotated, const ZC_Vec3<float>& look_on)
    : cam(CreateCamera()),
    callback_camera_roatted(std::move(_callback_camera_rotated))
{
    cam.MakeActive();
    SetDefaultState(look_on);
    ec_updater = ZC_SWindow::ConnectToUpdater({ &G_Camera::Callback_Updater, this }, G_UL__camera);
}

G_Camera::~G_Camera()
{
    ec_mouseMove.Disconnect();
    ec_mouseScroll.Disconnect();
    ec_updater.Disconnect();
}

void G_Camera::SetDefaultState(const ZC_Vec3<float>& look_on)
{
    cur_horizontal_angle = 0.f;
    horizontal_angle_must_be = 0.1f;
    cur_vertical_angle = vertical_angle_start_pos;
    vertical_angle_must_be = vertical_angle_start_pos;
    cam.SetLookOn(look_on);
    Callback_Updater(1.f);
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
    return cur_horizontal_angle;
}

void G_Camera::RotateCameraHorizontal(float angle)
{
    horizontal_angle_must_be += angle;
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
    if (yRel != 0.f)
    {
        static const float sensitivity_rotation_Y = 0.3f;   //  coef of angle on wich will be resturned camera

        float new_vertical_angle = vertical_angle_must_be + (sensitivity_rotation_Y * - yRel);
        new_vertical_angle = new_vertical_angle > vertical_angle_max ? vertical_angle_max : new_vertical_angle < vertical_angle_min ? vertical_angle_min : new_vertical_angle;
        
        if (new_vertical_angle != vertical_angle_must_be) vertical_angle_must_be = new_vertical_angle;
    }

    if (xRel != 0.f)
    {
        const float sensitivity_rotation_X = 3.f;   //  coef of angle on wich will be resturned camera

        float new_horizontal_angle = cur_horizontal_angle + (sensitivity_rotation_X * xRel);
        if (new_horizontal_angle < ZC_angle_0f) new_horizontal_angle += angle_1800f;
        if (new_horizontal_angle >= angle_1800f) new_horizontal_angle -= angle_1800f;
        
        if (new_horizontal_angle != cur_horizontal_angle) horizontal_angle_must_be = new_horizontal_angle;
    }
}

void G_Camera::RotateAroundObject()
{
    // //  manual rotation variant
    // float camX = 0.f,
    //       camY = 0.f,
    //       camZ = 0.f;
    // //  vertical rotaion (rotate around horizont)
    // float verticalRad = ZC_Vec::Radians(cur_vertical_angle);
    // camX = distanceToObject * cos(verticalRad);
    // camZ = distanceToObject * sin(verticalRad);
    // //  horizontal rotation (rotate around world up vector)
    // float horizontalRad = ZC_Vec::Radians(cur_horizontal_angle);
    // camY = camX * sin(horizontalRad);
    // camX = camX * cos(horizontalRad);

    // auto lookOn = camera.GetLookOn();
    // camera.SetPosition({ lookOn[0] + camX, lookOn[1] - camY, lookOn[2] + camZ });    //  on start positive X, positive Y, positive Z, if positive camX, camY, camZ

    //  analogue with rotation of the model matrix!
    ZC_Mat4<float> newModel(1.f);
    newModel.Rotate(cur_horizontal_angle, { 0.f, 0.f, -1.f }).Rotate(cur_vertical_angle, { -1.f, 0.f, 0.f });
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
    bool update_vertical_angle = cur_vertical_angle != vertical_angle_must_be;
    if (update_vertical_angle)
    {
        static const float vectical_angle_rotation_speed = 20.f / 1.f;    //  angle/sec (how fast will rotate camera)

        if (cur_vertical_angle < vertical_angle_must_be)    //  cur_vertical_angle go up
        {
            cur_vertical_angle += vectical_angle_rotation_speed * time;
            if (cur_vertical_angle >= vertical_angle_must_be)
            {
                cur_vertical_angle = vertical_angle_must_be;
            }
        }
        else
        {
            cur_vertical_angle -= vectical_angle_rotation_speed * time;
            if (cur_vertical_angle <= vertical_angle_must_be)
            {
                cur_vertical_angle = vertical_angle_must_be;
            }
        }
    }

    bool update_horizontal_angle = cur_horizontal_angle != horizontal_angle_must_be;
    if (update_horizontal_angle)
    {
        static const float horizontal_angle_rotation_speed = 190.f / 1.f;    //  angle/sec (how fast will rotate gcmera)

        if (cur_horizontal_angle < ZC_angle_360f && horizontal_angle_must_be > angle_1440f)
        {
            cur_horizontal_angle -= horizontal_angle_rotation_speed * time;
            if (cur_horizontal_angle < ZC_angle_0f) cur_horizontal_angle += angle_1800f;
        }
        else if (cur_horizontal_angle > angle_1440f && horizontal_angle_must_be < ZC_angle_360f)
        {
            cur_horizontal_angle += horizontal_angle_rotation_speed * time;
            if (cur_horizontal_angle > ZC_angle_360f) cur_horizontal_angle -= angle_1800f;
        }
        else if (cur_horizontal_angle < horizontal_angle_must_be)    //  cur_vertical_angle go up
        {
            cur_horizontal_angle += horizontal_angle_rotation_speed * time;
            if (cur_horizontal_angle >= horizontal_angle_must_be) cur_horizontal_angle = horizontal_angle_must_be;
        }
        else
        {
            cur_horizontal_angle -= horizontal_angle_rotation_speed * time;
            if (cur_horizontal_angle <= horizontal_angle_must_be) cur_horizontal_angle = horizontal_angle_must_be;
        }
    }

    if (update_vertical_angle || update_horizontal_angle)   //  was updated one or both of rotation angles
    {
        RotateAroundObject();
        callback_camera_roatted(cam.GetPosition());
    }
}