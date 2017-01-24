#pragma once

#include <vec3f.h>
#define VML_USE_VULKAN
//#define VML_USE_OPENGL
#include <matrix4x4.h>
#include <memory>
class Camera
{
public:
	Camera();
	~Camera();

	//in scene init
	void perspective(float fovy, float aspect, float znear, float zfar);
	void lookAt(const vec3f &eye, const vec3f &center, const vec3f &up);
	//in update uniform
	void rotate(const vec3f & rot);
	void zoom(float delta);
	

	Matrix4x4 proj;
	Matrix4x4 view;

	vec3f m_rotation;
	float m_zoom = -5;

	float m_zoomSpeed = 0.0066f;

	float m_aspect;
	float m_fovy;
	float m_znear;
	float m_zfar;

	void update();
	/*void update(uint32_t width, uint32_t height);*/
};

typedef std::shared_ptr<Camera> camera_ptr;


