//
// Transform2DComponent.h
// Copyright (C) Sebastian Nordgren 
// August 11th 2018
//

#pragma once

namespace d2d
{
	glm::mat3 Translation2DToMatrix(glm::vec2 translate);
	glm::mat3 Rotation2DToMatrix(float rotate);
	glm::mat3 Scale2DToMatrix(glm::vec2 scale);
	glm::mat3 Calculate2DTransform(glm::vec2 position, glm::vec2 scale, float rotation, glm::vec2 pivot = { 0, 0 });

	struct Transform2DComponent
	{
		glm::vec2 Position { 0, 0 };

		// radians rotated, clockwise
		float Rotation { 0 };
		glm::vec2 Scale { 1, 1 };

		const glm::mat3& Transform() const
		{
			DD_ASSERT(m_transform == Calculate2DTransform(Position, Scale, Rotation));
			return m_transform;
		}

		void Update()
		{
			m_transform = Calculate2DTransform(Position, Scale, Rotation);
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
		glm::mat3 m_transform { 1.0f };
	};
}