#pragma once

#include "glm.hpp"
#include "gtc/constants.hpp"

class Camera
{
public:
	~Camera();
	glm::mat4 getViewMatrix() const;
	virtual void roate(float yaw, float pitch);// in degrees
	virtual void serPosition(const glm::vec3& position);// in degrees
	virtual void move(const glm::vec3& offestPos);// in degrees

	const glm::vec3& getLook() const;
	const glm::vec3& getRight() const;
	const glm::vec3& getUp() const;

	float getFOV() const;
	void setFOV(float fov);
protected:
	Camera();
	glm::vec3 position_m;
	glm::vec3 targetPos_m;
	glm::vec3 up_m;
	glm::vec3 look_m;
	glm::vec3 right_m;

	const glm::vec3 worldup_m;
	float FOV_m; // degrees
	// Eulers angles in radians
	float yaw_m;
	float pitch_m;
};

class OrbitCamera : public Camera
{
public:
	OrbitCamera();
	virtual void roate(float yaw, float pitch);// in degrees
	void setLookAt(const glm::vec3& target);
	void setRadius(float radius);

private:
	void updateCameraVectors();
	float radius_m;
};