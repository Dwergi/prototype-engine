//
// ICamera.h - Interface for all camera implementations.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#pragma once

namespace dd
{
	class ICamera
	{
	public:

		//
		// Get the projection matrix of the camera.
		//
		virtual glm::mat4 GetProjectionMatrix() const = 0;

		//
		// Get the transformation matrix of the camera.
		//
		virtual glm::mat4 GetCameraMatrix() const = 0;

		//
		// Get the vertical FOV of the camera in radians.
		//
		virtual float GetVerticalFOV() const = 0;

		//
		// Get the aspect ratio of the camera.
		//
		virtual float GetAspectRatio() const = 0;

		//
		// Get the near plane of the camera.
		//
		virtual float GetNear() const = 0;

		//
		// Get the far plane of the camera.
		//
		virtual float GetFar() const = 0;
		
		//
		// Get the direction the camera is facing, as a normalized vector.
		//
		virtual glm::vec3 GetDirection() const = 0;

		//
		// Get the position of the camera.
		//
		virtual glm::vec3 GetPosition() const = 0;

		//
		// Is the camera dirty?
		//
		virtual bool IsDirty() const = 0;
	};
}