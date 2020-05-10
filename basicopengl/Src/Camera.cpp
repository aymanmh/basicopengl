#include "Camera.h"
#include "gtx/transform.hpp"

const float DEF_FOV = 45.0f;

float Camera::getFOV() const
{
	return FOV_m;
}

void Camera::setFOV(float fov)
{
	FOV_m = fov;
}

Camera::Camera()
	:position_m(glm::vec3(0.0f, 0.0f, 0.0f)),
	targetPos_m(glm::vec3(0.0f, 0.0f, 0.0f)),
	up_m(glm::vec3(0.0f, 1.0f, 0.0f)),
	look_m(glm::vec3(0.0f, 0.0f, 0.0f)),
	yaw_m(glm::pi<float>()),
	pitch_m(0.0f),
	right_m(0.0f, 0.0f, 0.0f),
	worldup_m(0.0f, 1.0f, 0.0f),
	FOV_m(DEF_FOV)
{
}

Camera::~Camera()
{
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(position_m, targetPos_m, up_m);
}

void Camera::roate(float yaw, float pitch)
{
}

void Camera::serPosition(const glm::vec3& position)
{
}

void Camera::move(const glm::vec3& offestPos)
{
}

const glm::vec3& Camera::getLook() const
{
	return look_m;
}

const glm::vec3& Camera::getRight() const
{
	return right_m;
}

const glm::vec3& Camera::getUp() const
{
	return up_m;
}

OrbitCamera::OrbitCamera() : radius_m(10.0f)
{
}

void OrbitCamera::roate(float yaw, float pitch)
{
	yaw_m = glm::radians(yaw);
	pitch_m = glm::radians(pitch);
	pitch_m = glm::clamp(pitch_m, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);

	updateCameraVectors();
}

void OrbitCamera::setLookAt(const glm::vec3& target)
{
	targetPos_m = target;
}

void OrbitCamera::setRadius(float radius)
{
	radius_m = glm::clamp(radius, 2.0f, 80.0f);
	updateCameraVectors();
}

void OrbitCamera::updateCameraVectors()
{
	position_m.x = targetPos_m.x + radius_m * cosf(pitch_m) * sinf(yaw_m);
	position_m.y = targetPos_m.y + radius_m * sinf(pitch_m);
	position_m.z = targetPos_m.z + radius_m * cosf(pitch_m) * cosf(yaw_m);
}