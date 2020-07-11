#include "PCH.h"
#include "FluxGame.h"

#include "EntityPrototype.h"
#include "DebugUI.h"
#include "HitTest.h"
#include "InputKeyBindings.h"
#include "Input.h"
#include "IWindow.h"
#include "OrthoCamera.h"
#include "SpriteRenderer.h"
#include "SpriteSheet.h"
#include "SystemsManager.h"
#include "Texture.h"

#include "d2d/BoxPhysicsComponent.h"
#include "d2d/CirclePhysicsComponent.h"
#include "d2d/Physics2DSystem.h"
#include "d2d/SpriteTileSystem.h"
#include "d2d/SpriteComponent.h"
#include "d2d/Transform2DComponent.h"

namespace flux
{
	static dd::Service<ddr::SpriteManager> s_spriteManager;
	static dd::Service<ddr::SpriteSheetManager> s_spriteSheetManager;
	static dd::Service<d2d::SpriteTileSystem> s_spriteTileSystem;
	static dd::Service<ddr::TextureManager> s_textureManager;
	static dd::Service<dd::IWindow> s_window;
	static dd::Service<dd::Input> s_input;
	static dd::Service<dd::DebugUI> s_debugUI;

	static ddr::OrthoCamera* s_camera;
	static dd::InputKeyBindings* s_keybindings;

	static ddc::Entity s_player;

	static const char* PLAYER_SPRITESHEET = "player_spritesheet.png";
	static const char* MAP_BACKGROUND = "map_background.png";
	static const char* MAP_FOREGROUND = "map_foreground.png";

	void FluxGame::Initialize()
	{
		s_camera = new ddr::OrthoCamera();

		dd::Services::Register(new ddr::SpriteManager());
		dd::Services::Register(new ddr::SpriteSheetManager(*s_spriteManager));

		ddr::TextureHandle spritesheet_tex_h = s_textureManager->Load(PLAYER_SPRITESHEET);
		s_spriteSheetManager->Load(PLAYER_SPRITESHEET, spritesheet_tex_h, glm::ivec2(32));

		ddr::TextureHandle background_tex_h = s_textureManager->Load(MAP_BACKGROUND);
		s_spriteSheetManager->Load(MAP_BACKGROUND, background_tex_h, glm::ivec2(16));

		ddr::TextureHandle foreground_tex_h = s_textureManager->Load(MAP_FOREGROUND);
		s_spriteSheetManager->Load(MAP_FOREGROUND, foreground_tex_h, glm::ivec2(16));
	}

	void FluxGame::RegisterSystems(ddc::SystemsManager& system_manager)
	{

	}

	void FluxGame::RegisterRenderers(ddr::RenderManager& renderer)
	{

	}

	void FluxGame::CreateEntityLayers(std::vector<ddc::EntityLayer*>& layers)
	{

	}

	void FluxGame::Update(const dd::GameUpdateData& update_data)
	{

	}

	void FluxGame::Shutdown()
	{

	}

	ddr::ICamera& FluxGame::GetCamera() const { return *s_camera; }
}