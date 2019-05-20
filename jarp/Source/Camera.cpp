#include "Camera.h"
#include "CrossPlatformWindow.h"
#include "Utils.hpp"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
	: Position(glm::vec3(0.0f, 0.0f, -3.0f)), FrontVector(glm::vec3(0.0f, 0.0f, 1.0f)), UpVector(glm::vec3(0.0f, -1.0f, 0.0f)),
		Yaw(0.0f), Pitch(0.0f),
		FieldOfView(glm::radians(60.0f)), AspectRatio(16.0f / 9.0f),
		NearPlane(0.1f), FarPlane(100.0f)
{
}

Camera::~Camera()
{
}

void Camera::Move(float DeltaSeconds)
{
	float CameraSpeed = 2.5f * DeltaSeconds;
	if (CrossPlatformWindow::IsKeyPressed(GLFW_KEY_W))
	{
		Position += CameraSpeed * FrontVector;
	}
	if (CrossPlatformWindow::IsKeyPressed(GLFW_KEY_S))
	{
		Position -= CameraSpeed * FrontVector;
	}
	if (CrossPlatformWindow::IsKeyPressed(GLFW_KEY_A))
	{
		glm::vec3 dot = glm::cross(FrontVector, UpVector);
		Position -= glm::normalize(glm::cross(FrontVector, UpVector)) * CameraSpeed;
	}
	if (CrossPlatformWindow::IsKeyPressed(GLFW_KEY_D))
	{
		Position += glm::normalize(glm::cross(FrontVector, UpVector)) * CameraSpeed;
	}
	CONSOLE_LOG("{ x=" << Position.x << ", y=" << Position.y << ", z=" << Position.z << " }");
}

glm::mat4 Camera::GetViewMatrix()
{
	glm::mat4 ViewMatrix = glm::mat4();
	//ViewMatrix = glm::rotate(ViewMatrix, Pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	//ViewMatrix = glm::rotate(ViewMatrix, Yaw, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 Target = Position + FrontVector;
	ViewMatrix = glm::lookAt(Position, Target, UpVector);

	return ViewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix()
{
	glm::mat4 ProjectionMatrix = glm::mat4();
	ProjectionMatrix = glm::perspective(FieldOfView, AspectRatio, NearPlane, FarPlane);
	//ProjectionMatrix[1][1] *= -1;

	return ProjectionMatrix;
}
