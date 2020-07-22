#include "PCH.h"
#include "Transform2DComponent.h"

DD_COMPONENT_CPP(d2d::Transform2DComponent);

namespace d2d
{
	glm::mat3 Translation2DToMatrix(glm::vec2 translation)
	{
		glm::mat3 result;
		result[0] = glm::vec3(1, 0, 0);
		result[1] = glm::vec3(0, 1, 0);
		result[2] = glm::vec3(translation.x, translation.y, 1);
		return result;
	}

	glm::mat3 Rotation2DToMatrix(float angle)
	{
		float cos = std::cosf(angle);
		float sin = std::sinf(angle);

		glm::mat3 rotation;
		rotation[0] = glm::vec3(cos, sin, 0);
		rotation[1] = glm::vec3(-sin, cos, 0);
		rotation[2] = glm::vec3(0, 0, 1);
		return rotation;
	}

	glm::mat3 Scale2DToMatrix(glm::vec2 scale)
	{
		glm::mat3 result;
		result[0] = glm::vec3(scale.x, 0, 0);
		result[1] = glm::vec3(0, scale.y, 0);
		result[2] = glm::vec3(0, 0, 1);
		return result;
	}

	glm::mat3 Calculate2DTransform(glm::vec2 position, glm::vec2 scale, float rotation, glm::vec2 pivot)
	{
		return Translation2DToMatrix(position) * Rotation2DToMatrix(rotation) * Scale2DToMatrix(scale) * Translation2DToMatrix(-pivot);
	}
}