#include "jarppch.h"
#include "Camera.h"
#include "Platform/Windows/WindowsWindow.h"
#include "Utils.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace jarp {

	Camera::Camera()
		: Position(glm::vec3(0.0f, 0.0f, -3.0f)), FrontVector(glm::vec3(0.0f, 0.0f, 1.0f)), UpVector(glm::vec3(0.0f, 1.0f, 0.0f)), RightVector(glm::vec3()),
		Yaw(0.0f), Pitch(0.0f),
		FieldOfView(glm::radians(60.0f)), AspectRatio(16.0f / 9.0f),
		NearPlane(0.1f), FarPlane(100.0f)
	{
		RightVector = glm::normalize(glm::cross(FrontVector, UpVector));
	}

	Camera::~Camera()
	{
	}

	void Camera::Move(uint32_t DeltaSeconds)
	{
		//float CameraSpeed = 2.5f * DeltaSeconds;
		//if (WindowsWindow::IsKeyPressed(GLFW_KEY_W))
		//{
		//	Position += CameraSpeed * FrontVector;
		//}
		//if (WindowsWindow::IsKeyPressed(GLFW_KEY_S))
		//{
		//	Position -= CameraSpeed * FrontVector;
		//}
		//if (WindowsWindow::IsKeyPressed(GLFW_KEY_A))
		//{
		//	Position -= RightVector * CameraSpeed;
		//}
		//if (WindowsWindow::IsKeyPressed(GLFW_KEY_D))
		//{
		//	Position += RightVector * CameraSpeed;
		//}

		//float Sensitivity = 0.02f;
		//auto MouseOffset = WindowsWindow::GetMouseOffset();
		//MouseOffset.first *= Sensitivity;
		//MouseOffset.second *= Sensitivity;

		//Yaw += static_cast<float>(MouseOffset.first);
		//Pitch += static_cast<float>(MouseOffset.second);
		//Pitch = std::clamp<float>(Pitch, -89.0f, 89.0f);

		//FrontVector.x = -(cos(glm::radians(Pitch)) * cos(glm::radians(Yaw)));
		//FrontVector.y = -sin(glm::radians(Pitch));
		//FrontVector.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
		//FrontVector = glm::normalize(FrontVector);

		//RightVector = glm::normalize(glm::cross(FrontVector, glm::vec3(0.0f, -1.0f, 0.0f)));
		//UpVector = glm::normalize(glm::cross(RightVector, FrontVector));
	}

	glm::mat4 Camera::GetViewMatrix()
	{
		glm::mat4 ViewMatrix = glm::mat4();
		//ViewMatrix = glm::rotate(ViewMatrix, Pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		//ViewMatrix = glm::rotate(ViewMatrix, Yaw, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec3 TargetVector = Position + FrontVector;
		ViewMatrix = glm::lookAt(Position, TargetVector, UpVector);

		return ViewMatrix;
	}

	glm::mat4 Camera::GetProjectionMatrix()
	{
		glm::mat4 ProjectionMatrix = glm::mat4();
		ProjectionMatrix = glm::perspective(FieldOfView, AspectRatio, NearPlane, FarPlane);
		//ProjectionMatrix[1][1] *= -1;

		return ProjectionMatrix;
	}

}
