#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : forward(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY)
{
	float fovy = 45.0;
	float aspect = 1.777;

	float halfScreenWidth = 1280.0 * 0.5;
	float halfScreenHeight = 720.0 * 0.5;

	this->projection_matrix = glm::ortho(-halfScreenWidth, halfScreenWidth, halfScreenHeight, -halfScreenHeight, 0.1f, 1000.0f);
	// glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
	this->position = position;
	this->worldUp = up;
	this->yaw = yaw;
	this->pitch = pitch;
	this->initPosition = position;
	update();
}

Camera::~Camera() {}

glm::mat4x4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + forward, up);
}

glm::mat4x4 Camera::getProjectionMatrix()
{
	return (projection_matrix);
}

void Camera::Move(CameraDirection direction, float distance)
{
	if (direction == FORWARD)
		position += forward * distance;
	if (direction == BACKWARD)
		position -= forward * distance;
	if (direction == LEFT)
		position -= right * distance;
	if (direction == RIGHT)
		position += right * distance;
	if (direction == UP)
		position += up * distance;
	if (direction == DOWN)
		position -= up * distance;
}

void Camera::Rotate(float xoffset, float yoffset, bool constrainPitch)
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
	update();
}

void Camera::update()
{
	glm::vec3 temp;
	temp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	temp.y = sin(glm::radians(pitch));
	temp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(temp);
	right = glm::normalize(glm::cross(forward, worldUp));
	up = glm::normalize(glm::cross(right, forward));
}

void Camera::Reset(glm::vec3 position, float yaw, float pitch)
{
	this->position = position;
	this->yaw = yaw;
	this->pitch = pitch;
	update();
}

void Camera::Scale(float delta, glm::vec2 mouse)
{
	// position.x = (mouse.x - 1280.0 * 0.5);
	// position.y = (mouse.y - 720.0 * 0.5);

	scale *= 1.0 + 0.05 * delta;
	// float halfScreenWidth = 1280.0 * 0.5 * scale;
	// float halfScreenHeight = 720.0 * 0.5 * scale;

	// glm::mat4x4 m = glm::ortho(-halfScreenWidth, halfScreenWidth, halfScreenHeight, -halfScreenHeight, 0.1f, 1000.0f);
	// this->projection_matrix = m;

	float sWidth = 1280.0;
	float sHeight = 720.0;
	// 200%
	float xPos = 0.75 * sWidth; // where we are centred
	float yPos = 0.5 * sHeight; // where we are centred
	float left = -sWidth / (2 * scale) + xPos;
	float right = sWidth / (2 * scale) + xPos;
	float top = -sHeight / (2 * scale) + yPos;
	float bottom = sHeight / (2 * scale) + yPos;
	this->projection_matrix = glm::orthoLH(left, right, bottom, top, 0.1f, 1000.0f);

	update();
}