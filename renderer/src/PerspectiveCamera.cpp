#include "PerspectiveCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>

PerspectiveCamera::PerspectiveCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : forward(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), m_Zoom(ZOOM)
{
	float fovy = 45.0;
	float aspect = 1.777;
	this->projection_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
	this->m_Position = position;
	this->worldUp = up;
	this->yaw = yaw;
	this->pitch = pitch;
	this->initPosition = position;
	OnUpdate(glm::vec2(0));
}

PerspectiveCamera::~PerspectiveCamera() {}

glm::mat4x4 PerspectiveCamera::getViewMatrix()
{
	return glm::lookAt(m_Position, m_Position + forward, up);
}

glm::mat4x4 PerspectiveCamera::getProjectionMatrix()
{
	return (projection_matrix);
}

void PerspectiveCamera::Move(CameraDirection direction, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;
	if (direction == FORWARD)
		m_Position += forward * velocity;
	if (direction == BACKWARD)
		m_Position -= forward * velocity;
	if (direction == LEFT)
		m_Position -= right * velocity;
	if (direction == RIGHT)
		m_Position += right * velocity;
}

void PerspectiveCamera::Rotate(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch -= yoffset;

	if (constrainPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}
	OnUpdate(glm::vec2(0));
}

void PerspectiveCamera::OnUpdate(glm::vec2 mp)
{
	glm::vec3 temp;
	temp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	temp.y = sin(glm::radians(pitch));
	temp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(temp);
	right = glm::normalize(glm::cross(forward, worldUp));
	up = glm::normalize(glm::cross(right, forward));
}

void PerspectiveCamera::Reset(glm::vec3 position, float yaw, float pitch)
{
	this->m_Position = position;
	this->yaw = yaw;
	this->pitch = pitch;
	OnUpdate(glm::vec2(0));
}

void PerspectiveCamera::BeginPan(glm::vec2 mousePosition) {}
void PerspectiveCamera::EndPan() {}
void PerspectiveCamera::Zoom(float delta, glm::vec2 mousePosition) {}
glm::vec2 PerspectiveCamera::ScreenToWorldSpace(int sx, int sy) { return glm::vec2(0); }
void PerspectiveCamera::OnResize(float width, float height) {}
