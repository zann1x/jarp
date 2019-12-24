#include "Camera.h"

#include "Utils.hpp"
#include "Platform/Windows/WindowsWindow.h"

#include <glm/gtc/matrix_transform.hpp>

namespace jarp {

	Camera::Camera()
		: m_Position(glm::vec3(0.0f, 0.0f, -3.0f)),
			m_FrontVector(glm::vec3(0.0f, 0.0f, 1.0f)), m_UpVector(glm::vec3(0.0f, 1.0f, 0.0f)), m_RightVector(glm::vec3()),
			m_Yaw(0.0f), m_Pitch(0.0f),
			m_FieldOfView(glm::radians(60.0f)), m_AspectRatio(16.0f / 9.0f),
			m_NearPlane(0.1f), m_FarPlane(100.0f)
	{
		m_RightVector = glm::normalize(glm::cross(m_FrontVector, m_UpVector));
	}

	void Camera::Move(uint32_t deltaSeconds)
	{
		//float cameraSpeed = 2.5f * deltaSeconds;
		//if (WindowsWindow::IsKeyPressed(GLFW_KEY_W))
		//{
		//	m_Position += cameraSpeed * m_FrontVector;
		//}
		//if (WindowsWindow::IsKeyPressed(GLFW_KEY_S))
		//{
		//	m_Position -= cameraSpeed * m_FrontVector;
		//}
		//if (WindowsWindow::IsKeyPressed(GLFW_KEY_A))
		//{
		//	m_Position -= m_RightVector * cameraSpeed;
		//}
		//if (WindowsWindow::IsKeyPressed(GLFW_KEY_D))
		//{
		//	m_Position += m_RightVector * cameraSpeed;
		//}

		//float sensitivity = 0.02f;
		//auto mouseOffset = WindowsWindow::GetMouseOffset();
		//mouseOffset.first *= sensitivity;
		//mouseOffset.second *= sensitivity;

		//m_Yaw += static_cast<float>(mouseOffset.first);
		//m_Pitch += static_cast<float>(mouseOffset.second);
		//m_Pitch = std::clamp<float>(m_Pitch, -89.0f, 89.0f);

		//m_FrontVector.x = -(cos(glm::radians(m_Pitch)) * cos(glm::radians(m_Yaw)));
		//m_FrontVector.y = -sin(glm::radians(m_Pitch));
		//m_FrontVector.z = cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));
		//m_FrontVector = glm::normalize(m_FrontVector);

		//m_RightVector = glm::normalize(glm::cross(m_FrontVector, glm::vec3(0.0f, -1.0f, 0.0f)));
		//m_UpVector = glm::normalize(glm::cross(m_RightVector, m_FrontVector));
	}

	glm::mat4 Camera::GetViewMatrix()
	{
		glm::mat4 viewMatrix = glm::mat4();
		//viewMatrix = glm::rotate(viewMatrix, m_Pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		//viewMatrix = glm::rotate(viewMatrix, m_Yaw, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec3 targetVector = m_Position + m_FrontVector;
		viewMatrix = glm::lookAt(m_Position, targetVector, m_UpVector);

		return viewMatrix;
	}

	glm::mat4 Camera::GetProjectionMatrix()
	{
		glm::mat4 projectionMatrix = glm::mat4();
		projectionMatrix = glm::perspective(m_FieldOfView, m_AspectRatio, m_NearPlane, m_FarPlane);
		//projectionMatrix[1][1] *= -1;

		return projectionMatrix;
	}

}
