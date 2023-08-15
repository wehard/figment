#include "PerspectiveCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>

PerspectiveCamera::PerspectiveCamera(float aspectRatio, glm::vec3 position, glm::vec3 up, float yaw, float pitch) : m_AspectRatio(aspectRatio), m_Position(position), m_Up(up), m_WorldUp(up), m_Yaw(yaw), m_Pitch(pitch), m_Forward(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), m_Zoom(ZOOM)
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

PerspectiveCamera::~PerspectiveCamera() {}

void PerspectiveCamera::UpdateProjectionMatrix()
{
	m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearClip, m_FarClip);
}

void PerspectiveCamera::UpdateViewMatrix()
{
	glm::vec3 temp;
	temp.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	temp.y = sin(glm::radians(m_Pitch));
	temp.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Forward = glm::normalize(temp);
	m_Right = glm::normalize(glm::cross(m_Forward, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
}

glm::mat4 PerspectiveCamera::GetViewMatrix()
{
	return m_ViewMatrix;
}

glm::mat4 PerspectiveCamera::GetProjectionMatrix()
{
	return m_ProjectionMatrix;
}

void PerspectiveCamera::Move(CameraDirection direction, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;
	if (direction == CameraDirection::Forward)
		m_Position += m_Forward * velocity;
	if (direction == CameraDirection::Backward)
		m_Position -= m_Forward * velocity;
	if (direction == CameraDirection::Left)
		m_Position -= m_Right * velocity;
	if (direction == CameraDirection::Right)
		m_Position += m_Right * velocity;
}

void PerspectiveCamera::Rotate(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	m_Yaw += xoffset;
	m_Pitch -= yoffset;

	if (constrainPitch)
	{
		if (m_Pitch > 89.0f)
			m_Pitch = 89.0f;
		if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;
	}
	OnUpdate(glm::vec2(0));
}

void PerspectiveCamera::OnUpdate(glm::vec2 mp)
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void PerspectiveCamera::Reset(glm::vec3 position, float yaw, float pitch)
{
	m_Position = position;
	m_Yaw = yaw;
	m_Pitch = pitch;

	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void PerspectiveCamera::BeginPan(glm::vec2 mousePosition) {}
void PerspectiveCamera::EndPan() {}

void PerspectiveCamera::Zoom(float delta, glm::vec2 mousePosition)
{
	m_Position.z -= delta;
	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void PerspectiveCamera::SetPosition(glm::vec3 position)
{
	m_Position = position;
	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void PerspectiveCamera::SetZoom(float amount)
{
	m_Zoom = amount;
	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

static glm::vec3 intersect(glm::vec3 planeP, glm::vec3 planeN, glm::vec3 rayP, glm::vec3 rayD)
{
	float d = glm::dot(planeP, -planeN);
	float t = -(d + rayP.z * planeN.z + rayP.y * planeN.y + rayP.x * planeN.x) / (rayD.z * planeN.z + rayD.y * planeN.y + rayD.x * planeN.x);
	return rayP + t * rayD;
}

static glm::vec3 projectMouse(int mouseX, int mouseY, float width, float height, glm::mat4 proj, glm::mat4 view)
{
	float x = (2.0f * mouseX) / width - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / height;
	float z = 1.0f;

	glm::vec3 ndc = glm::vec3(x, y, z);

	// homogenous clip space
	glm::vec4 clip = glm::vec4(ndc.x, ndc.y, -1.0, 1.0);

	// eye space
	glm::vec4 eye = glm::inverse(proj) * clip;

	eye = glm::vec4(eye.x, eye.y, -1.0, 0.0);

	glm::vec4 temp = glm::inverse(view) * eye;
	glm::vec3 world = glm::vec3(temp.x, temp.y, temp.z);

	world = glm::normalize(world);

	return world;
}

glm::vec3 PerspectiveCamera::ScreenToWorldSpace(glm::vec2 screenPosition, glm::vec2 viewportSize)
{
	glm::vec3 world;

	auto pPos = glm::vec3(0.0);
	auto pNormal = glm::normalize(m_Position - pPos);

	glm::vec3 worldPos = intersect(pPos, pNormal, m_Position, projectMouse(screenPosition.x, screenPosition.y, viewportSize.x, viewportSize.y, GetProjectionMatrix(), GetViewMatrix()));

	return worldPos;
}

void PerspectiveCamera::OnResize(float width, float height)
{
	m_AspectRatio = width / height;
	UpdateViewMatrix();
	UpdateProjectionMatrix();
}
