//
// Transform2DComponent.h
// Copyright (C) Sebastian Nordgren 
// August 11th 2018
//

#pragma once

namespace d2d
{
	struct Transform2DComponent
	{
		glm::vec2 Position { 0, 0 };

		// radians rotated, clockwise
		float Rotation { 0 };
		glm::vec2 Scale { 1, 1 };

		const glm::mat3& Transform() const
		{
			DD_ASSERT( m_transform == CalculateTransform() );
			return m_transform;
		}

		void Update()
		{
			m_transform = CalculateTransform();
		}

		DD_ALIGNED_ALLOCATORS(16);
		DD_BEGIN_CLASS(d2d::Transform2DComponent)
			DD_COMPONENT();

			DD_MEMBER(Position);
			DD_MEMBER(Rotation);
			DD_MEMBER(Scale);
		DD_END_CLASS()


	private:

		// final transform, created by calling Update()
		glm::mat3 m_transform;

		glm::mat3 CalculateTransform() const
		{
			return Translate2DToMatrix(Position) * Rotation2DToMatrix(Rotation) * Scale2DToMatrix(Scale);
		}

		static glm::mat3 Translate2DToMatrix(glm::vec2 translate)
		{
			glm::mat3 result;
			result[0] = glm::vec3(1, 0, 0);
			result[1] = glm::vec3(0, 1, 0);
			result[2] = glm::vec3(translate.x, translate.y, 1);
			return result;
		}

		static glm::mat3 Rotation2DToMatrix(float rotate)
		{
			float cos = std::cosf(rotate);
			float sin = std::sinf(rotate);

			glm::mat3 result;
			result[0] = glm::vec3(cos, sin, 0);
			result[1] = glm::vec3(-sin, cos, 0);
			result[2] = glm::vec3(0, 0, 1);
			return result;
		} 

		static glm::mat3 Scale2DToMatrix(glm::vec2 scale)
		{
			glm::mat3 result;
			result[0] = glm::vec3(scale.x, 0, 0);
			result[1] = glm::vec3(0, scale.y, 0);
			result[2] = glm::vec3(0, 0, 1);
			return result;
		}
	};
}