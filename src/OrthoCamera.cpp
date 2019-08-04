#include "PCH.h"
#include "OrthoCamera.h"

#include "Frustum.h"

#include "IWindow.h"

#include <glm/gtc/matrix_transform.hpp>

static dd::Service<dd::IWindow> s_window;

namespace ddr
{
	glm::mat4 OrthoCamera::GetProjectionMatrix() const
	{
		glm::ivec2 window_size = s_window->GetSize();
		
		float left = 0;
		float right = (float) window_size.x;
		float top = 0;
		float bottom = (float) window_size.y;

		glm::mat4 ortho(1);
		ortho[0][0] = 2.f / (right - left);
		ortho[1][1] = 2.f / (top - bottom);
		ortho[2][2] = -1.f;
		ortho[3][0] = -(right + left) / (right - left);
		ortho[3][1] = -(top + bottom) / (top - bottom);

		return ortho;
	}

	glm::mat4 OrthoCamera::GetViewMatrix() const
	{
		return glm::identity<glm::mat4>();
	}

	float OrthoCamera::GetVerticalFOV() const
	{
		return 0.0f;
	}

	float OrthoCamera::GetAspectRatio() const
	{
		return s_window->GetAspectRatio();
	}

	float OrthoCamera::GetNear() const
	{
		return 0.0f;
	}

	float OrthoCamera::GetFar() const
	{
		return 0.0f;
	}

	glm::vec3 OrthoCamera::GetDirection() const
	{
		return glm::vec3(0, 0, 1);
	}

	glm::vec3 OrthoCamera::GetPosition() const
	{
		return glm::vec3(0, 0, 0);
	}

	float OrthoCamera::GetYaw() const
	{
		return 0.0f;
	}

	float OrthoCamera::GetPitch() const
	{
		return 0.0f;
	}

	const ddr::Frustum& OrthoCamera::GetFrustum() const
	{
		static ddr::Frustum s_frustum;
		return s_frustum;
	}

	void OrthoCamera::Update(float delta_t)
	{
	}
}
