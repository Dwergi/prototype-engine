//
// Math_dd.h - Additional maths functions one might need.
// Copyright (C) Sebastian Nordgren 
// October 25th 2017
//

#pragma once

namespace ddm
{
#undef min
#undef max

	struct Ray;

	template <typename T>
	T min(T a, T b)
	{
		return a <= b ? a : b;
	}

	template <typename T>
	T min(T a, T b, T c)
	{
		return ddm::min(a, ddm::min(b, c));
	}

	template <typename T>
	T max(T a, T b)
	{
		return a >= b ? a : b;
	}

	template <typename T>
	T max(T a, T b, T c)
	{
		return ddm::max(a, ddm::max(b, c));
	}

	template <typename T>
	T clamp(T value, T min, T max)
	{
		return (value < min ? min : (value > max ? max : value));
	}

	template <typename T>
	T clamp1(T value)
	{
		return clamp(value, T(0), T(1)); 
	}

	template <typename T>
	std::enable_if_t<std::is_floating_point_v<T>, T> wrap(T value, T min, T max)
	{
		value = min + std::fmod(value - min, max - min);

		if (value < min)
			value += max;

		return value;
	}

	template <typename T>
	std::enable_if_t<std::is_integral_v<T>, T> wrap(T value, T min, T max)
	{
		value = min + (value - min) % (max - min);

		if (value < min)
			value += max;

		return value;
	}

	bool IsNaN(glm::vec2 v);
	bool IsNaN(glm::vec3 v);
	bool IsNaN(glm::vec4 v);

	bool IsInf(glm::vec2 v);
	bool IsInf(glm::vec3 v);
	bool IsInf(glm::vec4 v);

	glm::mat4 TransformFromOriginDir(const glm::vec3& origin, const glm::vec3& direction);
	glm::mat4 TransformFromRay(const ddm::Ray& ray);

	glm::vec3 DirectionFromPitchYaw(float pitch, float yaw);
	void PitchYawFromDirection(const glm::vec3& dir, float& pitch, float& yaw);

	glm::quat QuatFromPitchYaw(float pitch, float yaw);

	glm::vec3 NormalFromTriangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);
}

namespace std
{
	template <>
	struct hash<glm::vec2>
	{
		size_t operator()(const glm::vec2& v) const
		{
			size_t h = (*reinterpret_cast<const size_t*>(&v.x)) << 32;
			h = h | (*reinterpret_cast<const size_t*>(&v.y));
			return h;
		}
	};
}