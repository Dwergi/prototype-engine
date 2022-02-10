//
// Uniforms.h - Container for setting uniforms.
// Copyright (C) Sebastian Nordgren 
// July 13th 2018
//

#pragma once

namespace ddr
{
	struct Shader;
	struct Texture;

	enum class UniformType
	{
		Invalid = 0,
		Boolean,
		Integer,
		Float,
		Vector2,
		Vector3,
		Vector4,
		Matrix3,
		Matrix4,
		Sampler2
	};

	UniformType GetUniformTypeFromName(std::string_view type_name);

	template <typename T> UniformType GetUniformType() { return UniformType::Invalid; }

	template <typename T>
	UniformType GetUniformTypeFor(const T& value)
	{
		return GetUniformType<T>();
	}

	struct IUniform
	{
		UniformType Type;
	};

	template <typename T>
	struct Uniform : IUniform
	{
		T Value;
	};

	struct UniformStorage
	{
	public:

		UniformStorage();
		~UniformStorage();

		IUniform* Create(std::string_view name, UniformType type);

		void Set(std::string_view name, bool value);
		void Set(std::string_view name, int value);
		void Set(std::string_view name, float value);
		void Set(std::string_view name, glm::vec2 value);
		void Set(std::string_view name, glm::vec3 value);
		void Set(std::string_view name, glm::vec4 value);
		void Set(std::string_view name, const glm::mat3& value);
		void Set(std::string_view name, const glm::mat4& value);
		void Set(std::string_view name, const ddr::Texture& value);

		void Upload(Shader& shader);

		IUniform* Find(std::string_view name);

		void Clear();

		//
		// Get values from another uniform storage.
		// Does not add any additional uniforms.
		//
		void GetValuesFrom(UniformStorage& other);

	private:

		static const int MAX_UNIFORMS = 256;
		static const int UNIFORM_SIZE = sizeof(Uniform<glm::mat4>);

		std::unordered_map<std::string, int> m_uniforms;
		byte m_storage[MAX_UNIFORMS * UNIFORM_SIZE] { 0 };

		template <typename T>
		static void SetValue(IUniform* uniform, const T& value);

		template <typename T>
		void SetHelper(std::string_view name, const T& value);

		static void CopyValue(IUniform* dst, const IUniform* src);

		IUniform* Access(int index);
	};
}