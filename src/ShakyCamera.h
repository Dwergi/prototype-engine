#pragma once

#include "ICamera.h"
#include "IDebugDraw.h"
#include "InputAction.h"

namespace dd
{
	class FPSCamera;
	class InputBindings;

	class ShakyCamera : public IDebugDraw, public ICamera
	{
	public:

		//
		// The rate at which trauma decays over time, in units per second.
		//
		static float TraumaDecayRate;

		//
		// The maximum yaw amount to shake, in degrees.
		//
		static float MaximumYaw;

		//
		// The maximum pitch amount to shake, in degrees.
		//
		static float MaximumPitch;

		//
		// The maximum yaw amount, in degrees.
		//
		static float MaximumRoll;

		//
		// A multiplier on the speed. 
		//
		static float SpeedMultiplier;

		//
		// Constructor.
		//
		ShakyCamera( const FPSCamera& camera, InputBindings& bindings );

		//
		// Destructor.
		//
		~ShakyCamera();

		//
		// Update the shakiness.
		//
		void Update( float delta_t );

		// 
		// Add trauma (0-1) to the current amount. 
		// 
		void AddTrauma( float amount );

		//
		// Get the current amount of trauma (0-1).
		//
		float GetTrauma() const { return m_trauma; }

		//
		// Set the seed value.
		//
		void SetSeed( float seed ) { m_seed = seed; }


		//
		// IDebugDraw implementation
		//
		virtual const char * GetDebugTitle() const override { return "Shaky Camera"; }

		//
		// ICamera implementation
		//
		virtual glm::mat4 GetProjectionMatrix() const override;
		virtual glm::mat4 GetCameraMatrix() const override;
		virtual float GetVerticalFOV() const override;
		virtual float GetAspectRatio() const override;
		virtual float GetNear() const override;
		virtual float GetFar() const override;
		virtual glm::vec3 GetDirection() const override;
		virtual glm::vec3 GetPosition() const override;
		virtual bool IsDirty() const override;

	protected:

		virtual void DrawDebugInternal() override;

	private:

		const FPSCamera& m_sourceCamera;

		float m_seed { 0 };
		float m_trauma { 0 };
		float m_time { 0 };

		glm::vec3 m_direction;
		float m_yaw { 0 };
		float m_pitch { 0 };
		float m_roll { 0 };

		void AddTraumaHandler( InputAction action, InputType type );
	};
}