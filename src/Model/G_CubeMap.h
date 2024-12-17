#pragma once

#include <ZC/Video/OpenGL/Renderer/ZC_DrawerSet.h>

class G_CubeMap
{
public:
    G_CubeMap();

private:
    ZC_DrawerSet ds;
    ZC_DSController dsCon;

    ZC_DrawerSet CreateDrawerSet();
        //  return vectices look from the inside of the box
    std::vector<float> GetCubeVretices();
};















// #include "System/G_System.h"
// #include "GamePlay/G_GameManager.h"

// #include <ZC/Audio/ZC_Sounds.h>
// // #include "System/G_SoundName.h"
// #include <ZC/Events/ZC_Events.h>
// #include <ZC/Tools/ZC_Random.h>



// class Camera
// {
// public:
// 	Camera();
// 	// glm::mat4 GetViewMatrix() const;
// 	// glm::mat4 GetProjectionMatrix() const;
// 	// const glm::vec3& GetPosition() const;
// 	// const glm::vec3& GetFront() const;
// 	// void SetReverseCamera();
// 	// const glm::vec3& GetUp() const;

// private:
//     ZC_Camera cam;


// 	ZC_Vec3<float> position{ 0.0f, 0.1f, 0.f };
// 	ZC_Vec3<float> front{ 0.0f, 0.f, 0.f };
// 	ZC_Vec3<float> up{ 0.f, 0.f, 1.f };
// 	const ZC_Vec3<float> worldUp{ up };
// 	ZC_Vec3<float> right;
// 	const float movementSpeed = 6.0f;
// 	float yaw = -90.f, pitch = 0.0f;
// 	const float mouseSensitivity = 0.1f;
// 	float fov = 45.0f;
// 	const float nearPlan = 0.1f, farPlan = 100.0f;
// 	bool firstMouse = true;
// 	float mouseLastX = 0.0f, mouseLastY = 0.0f;
// 	// std::shared_ptr<char> pToMyself = std::make_shared<char>();

//     ZC_Camera CreateCamera();
//     void CursorMove(float x, float y, float xPos, float yPos, float time);
// 	// void CursorMove(const double& xPos, const double& yPos);
// 	// void Scroll(const double& xPos, const double& yPos);
// 	void UpdateCameraVectors();
// 	void MoveForward(ZC_ButtonID, float deltaTime);
// 	void MoveBackward(ZC_ButtonID,float deltaTime);
// 	// void MoveLeft(const float& deltaTime);
// 	// void MoveRight(const float& deltaTime);
// };



// void Func(const ZC_Vec3<float> &) {}
// int main(int argv, char** args)
// {
//     G_System window;
//     // G_GameManager gm;
    
    
//     // ZC_Function<void(const ZC_Vec3<float>&)> f(&Func);
//     // G_Camera cam(std::move(f));
//     // cam.SetConnectionToEvents(true);

//     Camera cam;

//     G_CubeMap cm;
// // auto s = ZC_Sounds::GetSound(G_SoundName::G_SN__platform_disapear);
// // s->Play();
//     // G_Config::Update_language(G_L_English);
//     // G_Config::Update_language(G_L_Russian);


//     window.RunMainCycle();

//     return 0;
// }

// #include <ZC/Video/ZC_SWindow.h>

// Camera::Camera()
//     : cam(CreateCamera())
// {
//     ZC_Events::ConnectMouseMoveOnceInFrame({ &Camera::CursorMove, this });
//     // ZC_Events::ConnectSignalScroll([&](const double& xPos, const double& yPos) { Scroll(xPos, yPos); }, pToMyself, 0);
//     ZC_Events::ConnectButtonClick(ZC_ButtonID::K_W, { &Camera::MoveForward, this }, nullptr);
//     ZC_Events::ConnectButtonClick(ZC_ButtonID::K_S, { &Camera::MoveBackward, this }, nullptr);
//     // ZC_Events::ConnectSignalKeyboard(Keyboard::Actions::MoveLeft, [&](const float& deltaTime) { MoveLeft(deltaTime); }, pToMyself, 0);
//     // ZC_Events::ConnectSignalKeyboard(Keyboard::Actions::MoveRight, [&](const float& deltaTime) { MoveRight(deltaTime); }, pToMyself, 0);
//     UpdateCameraVectors();
// }

// ZC_Camera Camera::CreateCamera()
// {
//     int width, height;
//     ZC_SWindow::GetSize(width, height);
//             //  no metter what sets in cam pos here. Camera position sets throught MouseMoveCallback() rotation, and MouseWheelScroll() distance
//     return ZC_Camera({ 0, 0.1f, 0.f }, {0.f, 0.f, 0.f}, {0.f, 0.f, 1.f}, ZC_Perspective(45.f, static_cast<float>(width) / static_cast<float>(height), 0.01f, 1000.f),
//         ZC_Ortho(0.f, static_cast<float>(width), 0.f, static_cast<float>(height)), true);
// }

// // glm::mat4 Camera::GetViewMatrix() const
// // {
// //     return glm::lookAt(position, position + front, up);
// // }
// //glm::mat4 Camera::LookAt()
// //{
// //    glm::vec3 camDirection = glm::normalize(position - (position + front));
// //    glm::vec3 camRight = glm::normalize(glm::cross(worldUp, camDirection));
// //    glm::vec3 camUp = glm::cross(camDirection, camRight);
// //    glm::mat4 translation = glm::mat4(1.0f);
// //    translation[3][0] = -position.x;
// //    translation[3][1] = -position.y;
// //    translation[3][2] = -position.z;
// //    glm::mat4 rotation = glm::mat4(1.0f);
// //    rotation[0][0] = camRight.x;
// //    rotation[1][0] = camRight.y;
// //    rotation[2][0] = camRight.z;
// //    rotation[0][1] = camUp.x;
// //    rotation[1][1] = camUp.y;
// //    rotation[2][1] = camUp.z;
// //    rotation[0][2] = camDirection.x;
// //    rotation[1][2] = camDirection.y;
// //    rotation[2][2] = camDirection.z;
// //    return rotation * translation;
// //}

// // glm::mat4 Camera::GetProjectionMatrix() const
// // {
// //     return glm::perspective(glm::radians(fov), (float)Window::GetWidth() / (float)Window::GetHeight(), nearPlan, farPlan);
// // }

// // const glm::vec3& Camera::GetPosition() const
// // {
// //     return position;
// // }

// // void Camera::SetReverseCamera()
// // {
// //     front *= -1;
// // }

// // const glm::vec3& Camera::GetFront() const
// // {
// //     return front;
// // }

// // const glm::vec3& Camera::GetUp() const
// // {
// //     return up;
// // }

// void Camera::CursorMove(float xPos, float yPos, float x_rel, float y_rel, float time)
// {
//     float mouseSensitivity = 1.f;
//     yaw -= x_rel * mouseSensitivity;
//     pitch += y_rel * mouseSensitivity;

//     UpdateCameraVectors();
// }

// // void Camera::Scroll(const double& xOffset, const double& yOffset)
// // {
// //     fov -= (float)yOffset * 4;
// //     if (fov < 1.0f) fov = 1.0f;
// //     if (fov > 45.0f) fov = 45.0f;
// // }

// void Camera::UpdateCameraVectors()
// {
//     if (pitch > 89.0f) pitch = 89.0f;
//     if (pitch < -89.0f) pitch = -89.0f;

//     ZC_Vec3<float> direction;
//     direction[0] = cos(ZC_Vec::Radians(yaw)) * cos(ZC_Vec::Radians(pitch));
//     direction[1] = sin(ZC_Vec::Radians(yaw)) * cos(ZC_Vec::Radians(pitch));
//     direction[2] = sin(ZC_Vec::Radians(pitch));
//     front = ZC_Vec::Normalize(direction);

//     right = ZC_Vec::Normalize(ZC_Vec::Cross(front, worldUp));
//     up = ZC_Vec::Normalize(ZC_Vec::Cross(right, front));

//     cam.SetLookOn(position + front);
// }
// float move_speed = 0.2f;
// void Camera::MoveForward(ZC_ButtonID, float deltaTime)
// {
//     position += front * move_speed;
//     cam.SetPosition(position);
//     cam.SetLookOn(position + front);
// }

// void Camera::MoveBackward(ZC_ButtonID, float deltaTime)
// {
//     position -= front * move_speed;
//     cam.SetPosition(position);
//     cam.SetLookOn(position + front);
// }

// // void Camera::MoveLeft(const float& deltaTime)
// // {
// //     position -= right * movementSpeed * deltaTime;
// // }

// // void Camera::MoveRight(const float& deltaTime)
// // {
// //     position += right * movementSpeed * deltaTime;
// // }