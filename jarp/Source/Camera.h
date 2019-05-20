#pragma once

#include "glm/glm.hpp"

class Camera
{
public:
	Camera();
	~Camera();

	void Move(float DeltaSeconds);
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();

	void SetAspectRatio(float AspectRatio) { this->AspectRatio = AspectRatio; }

private:
	glm::vec3 Position;
	glm::vec3 FrontVector;
	glm::vec3 UpVector;
	glm::vec3 RightVector;

	float Pitch;
	float Yaw;

	float FieldOfView;
	float AspectRatio;
	float NearPlane;
	float FarPlane;
};
