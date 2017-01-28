#include "camera.h"



Camera::Camera()
{
}


Camera::~Camera()
{

}

void Camera::perspective(float fovy, float aspect, float znear, float zfar)
{
	m_fovy = fovy;
	m_aspect = aspect;
	m_znear = znear;
	m_zfar = zfar;
	proj = vml::perspective(m_fovy, m_aspect, m_znear, m_zfar);
}

void Camera::lookAt(const vec3f &eye, const vec3f &center, const vec3f &up)
{
	view = vml::lookAt(eye, center, up);
}

void Camera::rotate(const vec3f & rot)
{
	m_rotation = rot;
}

void Camera::zoom(float delta)
{
	m_zoom += delta * m_zoomSpeed;
}


void Camera::update()
{
	Matrix4x4 rotMat;
	rotMat.translate(vec3f(0.f, 0.f, m_zoom));
	rotMat.rotate(AXIS::X, -m_rotation.x);
	rotMat.rotate(AXIS::Y, m_rotation.y);
	view = rotMat;
}

//void Camera::update(uint32_t width, uint32_t height)
//{
//	m_aspect = width / (float)height;
//	proj = vml::perspective(m_fovy, m_aspect, m_znear, m_zfar);
//	update();
//}
