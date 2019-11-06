#pragma once

#include <glm/glm.hpp>

namespace jarp {

	class Camera
	{
	public:
		Camera();

		void Move(uint32_t deltaSeconds);
		glm::mat4 GetViewMatrix();
		glm::mat4 GetProjectionMatrix();

		void SetAspectRatio(float aspectRatio) { this->m_AspectRatio = aspectRatio; }

	private:
		glm::vec3 m_Position;
		glm::vec3 m_FrontVector;
		glm::vec3 m_UpVector;
		glm::vec3 m_RightVector;

		float m_Pitch;
		float m_Yaw;

		float m_FieldOfView;
		float m_AspectRatio;
		float m_NearPlane;
		float m_FarPlane;
	};

}
