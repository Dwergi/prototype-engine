#include "PCH.h"
#include "FluxGame.h"

#include "AssetManager.h"
#include "DebugUI.h"
#include "EntityPrototype.h"
#include "HitTest.h"
#include "InputKeyBindings.h"
#include "Input.h"
#include "IWindow.h"
#include "OrthoCamera.h"
#include "RenderManager.h"
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
	static dd::Service<dd::AssetManager> s_assetManager;
	static dd::Service<ddr::SpriteManager> s_spriteManager;
	static dd::Service<ddr::SpriteSheetManager> s_spriteSheetManager;
	static dd::Service<d2d::SpriteTileSystem> s_spriteTileSystem;
	static dd::Service<ddr::TextureManager> s_textureManager;
	static dd::Service<dd::IWindow> s_window;
	static dd::Service<dd::Input> s_input;
	static dd::Service<dd::DebugUI> s_debugUI;

	static ddr::OrthoCamera* s_camera;
	static dd::InputKeyBindings* s_keybindings;
	static ddr::SpriteSheetHandle s_playerSpriteSheet;

	static ddc::Entity s_player;

	static const glm::ivec2 MAP_SIZE(32, 24);

	static const char* PLAYER_SPRITESHEET = "flux_player.png";
	static const char* MAP_BACKGROUND = "map_background.png";
	static const char* MAP_FOREGROUND = "map_foreground.png";

	static ddc::Entity CreatePlayer(ddc::EntityLayer& layer)
	{
		ddc::Entity player = layer.CreateEntity<d2d::SpriteComponent, d2d::Transform2DComponent, d2d::BoxPhysicsComponent>();
		layer.AddTag(player, ddc::Tag::Visible);
		layer.AddTag(player, ddc::Tag::Static);

		d2d::Transform2DComponent* transform_cmp = player.Access<d2d::Transform2DComponent>();
		transform_cmp->Scale = glm::vec2(2);
		transform_cmp->Position = MAP_SIZE / 2;
		transform_cmp->Update();

		const ddr::SpriteSheet* spritesheet = s_playerSpriteSheet.Get();

		d2d::SpriteComponent* sprite_cmp = player.Access<d2d::SpriteComponent>();
		sprite_cmp->Sprite = spritesheet->Get(0, 0);
		sprite_cmp->ZIndex = 9;

		d2d::BoxPhysicsComponent* physics = player.Access<d2d::BoxPhysicsComponent>();
		physics->Elasticity = 0;

		return player;
	}

	void FluxGame::Initialize()
	{
		ddr::TextureManager& texture_manager = dd::Services::Register(new ddr::TextureManager());
		s_assetManager->Register(texture_manager);

		ddr::ShaderManager& shader_manager = dd::Services::Register(new ddr::ShaderManager());
		s_assetManager->Register(shader_manager);

		ddr::SpriteManager& sprite_manager = dd::Services::Register(new ddr::SpriteManager());
		s_assetManager->Register(sprite_manager);

		ddr::SpriteSheetManager& spritesheet_manager = dd::Services::Register(new ddr::SpriteSheetManager(*s_spriteManager));
		s_assetManager->Register(spritesheet_manager);

		ddr::TextureHandle player_tex_h = s_textureManager->Load(PLAYER_SPRITESHEET);
		s_playerSpriteSheet = s_spriteSheetManager->Load(PLAYER_SPRITESHEET, player_tex_h, glm::ivec2(32));

		/*ddr::TextureHandle background_tex_h = s_textureManager->Load(MAP_BACKGROUND);
		s_spriteSheetManager->Load(MAP_BACKGROUND, background_tex_h, glm::ivec2(16));

		ddr::TextureHandle foreground_tex_h = s_textureManager->Load(MAP_FOREGROUND);
		s_spriteSheetManager->Load(MAP_FOREGROUND, foreground_tex_h, glm::ivec2(16));*/
	}

	void FluxGame::RegisterSystems(ddc::SystemsManager& system_manager)
	{
		d2d::SpriteTileSystem& sprite_tile_system = dd::Services::Register(new d2d::SpriteTileSystem(MAP_SIZE, 16));
		system_manager.Register(sprite_tile_system);
	}

	void FluxGame::RegisterRenderers(ddr::RenderManager& renderer)
	{
		s_camera = new ddr::OrthoCamera();
		renderer.SetCamera(*s_camera);

		ddr::SpriteRenderer& sprite_renderer = dd::Services::Register(new ddr::SpriteRenderer());
		renderer.Register(sprite_renderer);
	}

	void FluxGame::CreateEntityLayers(std::vector<ddc::EntityLayer*>& entity_layers)
	{
		entity_layers.push_back(new ddc::EntityLayer("game"));
		entity_layers.push_back(new ddc::EntityLayer("background"));

		s_player = CreatePlayer(*entity_layers[0]);
	}

	void FluxGame::Update(const dd::GameUpdateData& update_data)
	{
		s_spriteManager->Update();
		s_spriteSheetManager->Update();
	}

	void FluxGame::Shutdown()
	{
		delete s_camera;
	}
}
