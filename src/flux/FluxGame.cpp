#include "PCH.h"
#include "FluxGame.h"

#include "AssetManager.h"
#include "DebugUI.h"
#include "File.h"
#include "InputKeyBindings.h"
#include "Input.h"
#include "IWindow.h"
#include "OrthoCamera.h"
#include "SpriteRenderer.h"
#include "SpriteSheet.h"

#include "d2d/BoxPhysicsComponent.h"
#include "d2d/CirclePhysicsComponent.h"
#include "d2d/Physics2DSystem.h"
#include "d2d/SpriteTileSystem.h"
#include "d2d/SpriteComponent.h"
#include "d2d/Transform2DComponent.h"

#include "ddc/EntityPrototype.h"
#include "ddc/SystemsManager.h"

#include "ddm/HitTest.h"

#include "ddr/RenderManager.h"
#include "ddr/Texture.h"

#include "flux/FluxBulletSystem.h"
#include "flux/FluxEnemySystem.h"
#include "flux/FluxPlayerComponent.h"
#include "flux/FluxPlayerController.h"

namespace flux
{
	static dd::Service<dd::AssetManager> s_assetManager;
	static dd::Service<ddr::SpriteManager> s_spriteManager;
	static dd::Service<ddr::SpriteSheetManager> s_spriteSheetManager;
	static dd::Service<d2d::SpriteTileSystem> s_spriteTileSystem;
	static dd::Service<ddr::TextureManager> s_textureManager;
	static dd::Service<dd::IWindow> s_window;
	static dd::Service<dd::Input> s_input;
	static dd::Service<dd::InputKeyBindings> s_keybindings;
	static dd::Service<dd::IDebugUI> s_debugUI;
	static dd::Service<flux::FluxPlayerController> s_playerController;
	static dd::Service<flux::FluxEnemySystem> s_enemySystem;

	static ddr::OrthoCamera* s_camera;
	static ddr::SpriteSheetHandle s_playerSpriteSheet;
	static ddr::SpriteSheetHandle s_playerBulletSpriteSheet;

	static ddc::Entity s_player;

	static const glm::ivec2 MAP_SIZE = { 32, 24 };
	static const int TILE_SIZE = 16;

	static const char* PLAYER_SPRITESHEET = "player.png";
	static const char* PLAYER_BULLET_SPRITESHEET = "player_bullet.png";
	static const char* ENEMY_SPRITESHEET = "enemy.png";
	static const char* ENEMY_BULLET_SPRITESHEET = "enemy_bullet.png";

	static const char* MAP_BACKGROUND = "map_background.png";
	static const char* MAP_FOREGROUND = "map_foreground.png";

	flux::PlayerStats s_playerStats;
	std::vector<flux::Weapon> s_weapons;

	static ddc::Entity CreatePlayer(ddc::EntityLayer& layer)
	{
		ddc::ScratchEntity player = ddc::ScratchEntity::Create<d2d::SpriteComponent, d2d::Transform2DComponent, d2d::CirclePhysicsComponent, flux::FluxPlayerComponent>();
		player.AddTag(ddc::Tag::Visible);
		player.AddTag(ddc::Tag::Static);

		d2d::Transform2DComponent* transform_cmp = player.Access<d2d::Transform2DComponent>();
		transform_cmp->Scale = { 1, 1 };
		transform_cmp->Position = MAP_SIZE / 2;
		transform_cmp->Update();

		d2d::SpriteComponent* sprite_cmp = player.Access<d2d::SpriteComponent>();
		sprite_cmp->Sprite = s_playerSpriteSheet->Get(0, 0);
		sprite_cmp->ZIndex = 9;
		sprite_cmp->Pivot = { 0.5, 0.5 };

		d2d::CirclePhysicsComponent* physics_cmp = player.Access<d2d::CirclePhysicsComponent>();
		physics_cmp->Elasticity = 0;

		flux::FluxPlayerComponent* player_cmp = player.Access<flux::FluxPlayerComponent>();
		player_cmp->Stats = &s_playerStats;
		player_cmp->EquippedWeapon = &s_weapons[0];

		return player.Instantiate(layer);
	}

	static void ResetPlayer()
	{
		d2d::Transform2DComponent* transform_cmp = s_player.Access<d2d::Transform2DComponent>();
		transform_cmp->Position = MAP_SIZE / 2;
		transform_cmp->Update();
	}

	void FluxGame::Initialize()
	{
		dd::File::AddOverridePath("./flux");

		dd::Services::Register(new ddc::EntityPrototypeManager());

		ddr::TextureManager& texture_manager = dd::Services::Register(new ddr::TextureManager());
		s_assetManager->Register(texture_manager);

		ddr::SpriteManager& sprite_manager = dd::Services::Register(new ddr::SpriteManager());
		s_assetManager->Register(sprite_manager);

		ddr::SpriteSheetManager& spritesheet_manager = dd::Services::Register(new ddr::SpriteSheetManager(*s_spriteManager));
		s_assetManager->Register(spritesheet_manager);

		ddr::TextureHandle player_tex_h = s_textureManager->Load(PLAYER_SPRITESHEET);
		s_playerSpriteSheet = s_spriteSheetManager->Load(PLAYER_SPRITESHEET, player_tex_h, glm::ivec2(32));

		ddr::TextureHandle player_bullet_tex_h = s_textureManager->Load(PLAYER_BULLET_SPRITESHEET);
		s_playerBulletSpriteSheet = s_spriteSheetManager->Load(PLAYER_BULLET_SPRITESHEET, player_bullet_tex_h, glm::ivec2(32));

		ddr::TextureHandle enemy_tex_h = s_textureManager->Load(ENEMY_SPRITESHEET);
		s_spriteSheetManager->Load(ENEMY_SPRITESHEET, enemy_tex_h, glm::ivec2(32));

		/*ddr::TextureHandle enemy_bullet_tex_h = s_textureManager->Load(ENEMY_BULLET_SPRITESHEET);
		s_spriteSheetManager->Load(ENEMY_BULLET_SPRITESHEET, enemy_bullet_tex_h, glm::ivec2(32));*/

		/*ddr::TextureHandle background_tex_h = s_textureManager->Load(MAP_BACKGROUND);
		s_spriteSheetManager->Load(MAP_BACKGROUND, background_tex_h, glm::ivec2(16));

		ddr::TextureHandle foreground_tex_h = s_textureManager->Load(MAP_FOREGROUND);
		s_spriteSheetManager->Load(MAP_FOREGROUND, foreground_tex_h, glm::ivec2(16));*/

		dd::InputModeConfig& game_input = dd::InputModeConfig::Create("game");
		game_input.ShowCursor(true)
			.CaptureMouse(true)
			.CentreMouse(false);

		s_input->SetCurrentMode("game");

		s_keybindings->BindKey(dd::Key::W, dd::InputAction::UP, "game");
		s_keybindings->BindKey(dd::Key::S, dd::InputAction::DOWN, "game");
		s_keybindings->BindKey(dd::Key::A, dd::InputAction::LEFT, "game");
		s_keybindings->BindKey(dd::Key::D, dd::InputAction::RIGHT, "game");
		s_keybindings->BindKey(dd::Key::MOUSE_LEFT, dd::InputAction::SHOOT, "game");
		s_keybindings->BindKey(dd::Key::R, dd::InputAction::RESET, "game");

		s_input->AddHandler(dd::InputAction::RESET, &ResetPlayer);

		s_playerStats.MaxHealth = 100;

		s_playerStats.MaxSpeed = 15; // tiles/s
		s_playerStats.Acceleration = 50; // tiles/s/s
		s_playerStats.Deceleration = 100; // tiles/s/s

		flux::Weapon weapon;
		weapon.BulletDamage = 10; // tiles/s
		weapon.BulletSpeed = 30; // tiles/s
		weapon.ShotDelay = 0.2f; // seconds

		s_weapons.push_back(weapon);
	}

	void FluxGame::RegisterSystems(ddc::SystemsManager& system_manager)
	{
		d2d::SpriteTileSystem& sprite_tile_system = dd::Services::Register(new d2d::SpriteTileSystem(MAP_SIZE, TILE_SIZE));

		flux::FluxBulletSystem& flux_bullet_system = dd::Services::Register(new flux::FluxBulletSystem(MAP_SIZE));
		system_manager.Register(flux_bullet_system);

		flux::FluxPlayerController& flux_player_system = dd::Services::Register(new flux::FluxPlayerController(sprite_tile_system));
		system_manager.Register(flux_player_system);

		flux::FluxEnemySystem& flux_enemy_system = dd::Services::Register(new flux::FluxEnemySystem(MAP_SIZE));
		system_manager.Register(flux_enemy_system);

		sprite_tile_system.DependsOn(flux_player_system);
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

	void FluxGame::DrawDebugInternal()
	{
		ImGui::SliderInt("Max Health", &s_playerStats.MaxHealth, 0, 1000);
		ImGui::SliderFloat("Acceleration", &s_playerStats.Acceleration, 0, 100, "%.1f");
		ImGui::SliderFloat("Deceleration", &s_playerStats.Deceleration, 0, 100, "%.1f");
		ImGui::SliderFloat("Max Speed", &s_playerStats.MaxSpeed, 0, 50, "%.1f");
	}
}
