#include "PCH.h"
#include "Physics2DGame.h"

#include "FPSCameraComponent.h"

namespace phys2d
{
	void Physics2DGame::Initialize()
	{
	}

	void Physics2DGame::RegisterSystems(ddc::SystemsManager& system_manager)
	{

	}

	void Physics2DGame::RegisterRenderers(ddr::RenderManager& renderer)
	{
	}

	void Physics2DGame::CreateEntityLayers(std::vector<ddc::EntityLayer*>& spaces)
	{
	}

	void Physics2DGame::Shutdown()
	{
	}

	void Physics2DGame::Update(const dd::GameUpdateData& update_data)
	{
	}

	static dd::FPSCameraComponent s_camera;
	ddr::ICamera& Physics2DGame::GetCamera() const
	{
		return s_camera;
	}
}