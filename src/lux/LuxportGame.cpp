//
// LuxportGame.cpp - Game file for Luxport GMTK Game Jame 2019.
// Copyright (C) Sebastian Nordgren 
// August 3rd 2019
//

#include "PCH.h"
#include "LuxportGame.h"

#include "AssetManager.h"
#include "DebugUI.h"
#include "File.h"
#include "HitTest.h"
#include "InputKeyBindings.h"
#include "Input.h"
#include "IWindow.h"
#include "OrthoCamera.h"
#include "ScratchEntity.h"
#include "SpriteRenderer.h"
#include "SpriteSheet.h"
#include "SystemsManager.h"
#include "Texture.h"
#include "RenderManager.h"

#include "d2d/BoxPhysicsComponent.h"
#include "d2d/CirclePhysicsComponent.h"
#include "d2d/Physics2DSystem.h"
#include "d2d/SpriteTileSystem.h"
#include "d2d/SpriteAnimationComponent.h"
#include "d2d/SpriteAnimationSystem.h"
#include "d2d/SpriteComponent.h"
#include "d2d/Transform2DComponent.h"

#include "lux/LuxLightComponent.h"
#include "lux/LuxLightRenderer.h"
#include "lux/LuxportMap.h"

// magic: https://stackoverflow.com/questions/30412951/unresolved-external-symbol-imp-fprintf-and-imp-iob-func-sdl2
FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE* __cdecl __iob_func(void)
{
	return _iob;
}

#include <sfml/Audio.hpp>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include <fmt/format.h>

namespace lux
{
	static dd::Service<dd::AssetManager> s_assetManager;
	static dd::Service<ddr::SpriteManager> s_spriteManager;
	static dd::Service<ddr::SpriteSheetManager> s_spriteSheetManager;
	static dd::Service<d2d::SpriteTileSystem> s_spriteTileSystem;
	static dd::Service<dd::IWindow> s_window;
	static dd::Service<dd::Input> s_input;
	static dd::Service<dd::DebugUI> s_debugUI;

	static ddr::OrthoCamera* s_camera;

	static ddc::Entity s_player;
	static ddc::Entity s_teleporter;
	static ddr::SpriteHandle s_teleporterDynamicSprite;

	static LuxportMap* s_currentMap { nullptr };
	static int s_desiredMapIndex { 1 };

	static const float TELEPORTER_RADIUS = 0.2f;
	static const float TELEPORTER_ELASTICITY = 0.3f;
	static const int TELEPORTER_Z_INDEX = 10;

	static const int PLAYER_Z_INDEX = 9;
	static const char* const PLAYER_SPRITESHEET = "spacestation_character.png";

	static const int MAX_MAP = 6;
	static const glm::ivec2 MAP_SIZE(32, 24);
	static const char* const MAP_BACKGROUND = "spacestation_tileset.png";
	static const char* const MAP_FOREGROUND = "spacestation_objects.png";

	static sf::Music* s_music = nullptr;
	static sf::SoundBuffer* s_teleportSoundBuffer = nullptr;
	static sf::SoundBuffer* s_exitSoundBuffer = nullptr;
	static sf::SoundBuffer* s_throwSoundBuffer = nullptr;
	static sf::SoundBuffer* s_teleReturnSoundBuffer = nullptr;

	static sf::Sound* s_activeSound1 = nullptr;
	static sf::Sound* s_activeSound2 = nullptr;
	static sf::Listener* s_listener = nullptr;

	static ddc::Entity CreateTeleporter(ddc::EntityLayer& layer)
	{
		ddc::Entity teleporter = layer.CreateEntity<d2d::SpriteComponent, d2d::Transform2DComponent, d2d::CirclePhysicsComponent, d2d::SpriteAnimationComponent>();
		layer.AddTag(teleporter, ddc::Tag::Visible);

		{
			ddr::SpriteSheetHandle fg_spritesheet_h = s_spriteSheetManager->Find(MAP_FOREGROUND);
			const ddr::SpriteSheet* fg_spritesheet = fg_spritesheet_h.Get();
			s_teleporterDynamicSprite = fg_spritesheet->Get(67);
		}

		ddr::SpriteSheetHandle char_spritesheet_h = s_spriteSheetManager->Find(PLAYER_SPRITESHEET);
		const ddr::SpriteSheet* char_spritesheet = char_spritesheet_h.Get();

		d2d::SpriteComponent* sprite_cmp = teleporter.Access<d2d::SpriteComponent>();

		d2d::SpriteAnimationComponent* sprite_anim_cmp = teleporter.Access<d2d::SpriteAnimationComponent>();
		sprite_anim_cmp->Frames.Add(char_spritesheet->Get(4, 0));
		sprite_anim_cmp->Frames.Add(char_spritesheet->Get(5, 0));
		sprite_anim_cmp->Frames.Add(char_spritesheet->Get(6, 0));
		sprite_anim_cmp->Frames.Add(char_spritesheet->Get(7, 0));
		sprite_anim_cmp->Framerate = 4;

		sprite_cmp->Sprite = sprite_anim_cmp->Frames[0];
		sprite_cmp->ZIndex = TELEPORTER_Z_INDEX;

		d2d::Transform2DComponent* transform_cmp = teleporter.Access<d2d::Transform2DComponent>();
		transform_cmp->Scale = glm::vec2(2);
		transform_cmp->Update();

		d2d::CirclePhysicsComponent* tele_physics = teleporter.Access<d2d::CirclePhysicsComponent>();
		tele_physics->HitCircle.Radius = TELEPORTER_RADIUS;
		tele_physics->Elasticity = TELEPORTER_ELASTICITY;

		lux::LuxLightComponent& tele_light = teleporter.Add<lux::LuxLightComponent>();
		tele_light.Type = lux::LightType::Teleporter;
		return teleporter;
	}

	static ddc::Entity CreatePlayer(ddc::EntityLayer& layer)
	{
		ddc::Entity player = layer.CreateEntity<d2d::SpriteComponent, d2d::Transform2DComponent, d2d::SpriteAnimationComponent, d2d::BoxPhysicsComponent>();
		layer.AddTag(player, ddc::Tag::Visible);
		layer.AddTag(player, ddc::Tag::Static);

		ddr::SpriteSheetHandle spritesheet_h = s_spriteSheetManager->Find(PLAYER_SPRITESHEET);
		const ddr::SpriteSheet* spritesheet = spritesheet_h.Get();

		d2d::Transform2DComponent* transform_cmp = player.Access<d2d::Transform2DComponent>();
		transform_cmp->Scale = glm::vec2(2);
		transform_cmp->Update();

		d2d::SpriteAnimationComponent* sprite_anim_cmp = player.Access<d2d::SpriteAnimationComponent>();
		sprite_anim_cmp->Frames.Add(spritesheet->Get(0, 0));
		sprite_anim_cmp->Frames.Add(spritesheet->Get(1, 0));
		sprite_anim_cmp->Frames.Add(spritesheet->Get(2, 0));
		sprite_anim_cmp->Frames.Add(spritesheet->Get(3, 0));
		sprite_anim_cmp->Framerate = 4;

		d2d::SpriteComponent* sprite_cmp = player.Access<d2d::SpriteComponent>();
		sprite_cmp->Sprite = sprite_anim_cmp->Frames[0];
		sprite_cmp->ZIndex = PLAYER_Z_INDEX;

		for (ddr::SpriteHandle frame : sprite_anim_cmp->Frames)
		{
			DD_ASSERT(frame.IsValid());
		}

		d2d::BoxPhysicsComponent* physics = player.Access<d2d::BoxPhysicsComponent>();
		physics->HitBox.Min = glm::vec2(0.28, 0.0625);
		physics->HitBox.Max = glm::vec2(0.72, 1);
		physics->Elasticity = 0;

		return player;
	}

	void LuxportGame::RegisterSystems(ddc::SystemsManager& system_manager)
	{
		d2d::PhysicsSystem& physics_system = dd::Services::Register(new d2d::PhysicsSystem());
		system_manager.Register(physics_system);
		s_debugUI->RegisterDebugPanel(physics_system);

		d2d::SpriteAnimationSystem& sprite_anim_system = dd::Services::Register(new d2d::SpriteAnimationSystem());
		system_manager.Register(sprite_anim_system);

		d2d::SpriteTileSystem& sprite_tile_system = dd::Services::Register(new d2d::SpriteTileSystem(MAP_SIZE, 16));
		sprite_tile_system.DependsOn(physics_system);
		sprite_tile_system.DependsOn(sprite_anim_system);
		system_manager.Register(sprite_tile_system);
	}

	void LuxportGame::RegisterRenderers(ddr::RenderManager& renderer)
	{
		s_camera = new ddr::OrthoCamera();
		renderer.SetCamera(*s_camera);

		lux::LuxLightRenderer& lights_renderer = dd::Services::Register(new lux::LuxLightRenderer());
		renderer.Register(lights_renderer);

		ddr::SpriteRenderer& sprite_renderer = dd::Services::Register(new ddr::SpriteRenderer());
		renderer.Register(sprite_renderer);
	}

	void LuxportGame::CreateEntityLayers(std::vector<ddc::EntityLayer*>& entity_layers)
	{
		entity_layers.push_back(new ddc::EntityLayer("game"));

		s_teleporter = CreateTeleporter(*entity_layers[0]);
		s_player = CreatePlayer(*entity_layers[0]);
	}

	void LuxportGame::Initialize()
	{
		dd::File::AddOverridePath("./lux");

		ddr::ShaderManager& shader_manager = dd::Services::Register(new ddr::ShaderManager());
		s_assetManager->Register(shader_manager);

		ddr::TextureManager& texture_manager = dd::Services::Register(new ddr::TextureManager());
		s_assetManager->Register(texture_manager);

		ddr::SpriteManager& sprite_manager = dd::Services::Register(new ddr::SpriteManager());
		s_assetManager->Register(sprite_manager);

		ddr::SpriteSheetManager& spritesheet_manager = dd::Services::Register(new ddr::SpriteSheetManager(*s_spriteManager));
		s_assetManager->Register(spritesheet_manager);

		ddr::TextureHandle spritesheet_tex_h = texture_manager.Load(PLAYER_SPRITESHEET);
		s_spriteSheetManager->Load(PLAYER_SPRITESHEET, spritesheet_tex_h, glm::ivec2(32));

		ddr::TextureHandle background_tex_h = texture_manager.Load(MAP_BACKGROUND);
		s_spriteSheetManager->Load(MAP_BACKGROUND, background_tex_h, glm::ivec2(16));

		ddr::TextureHandle foreground_tex_h = texture_manager.Load(MAP_FOREGROUND);
		s_spriteSheetManager->Load(MAP_FOREGROUND, foreground_tex_h, glm::ivec2(16));

		dd::InputModeConfig& game_input = dd::InputModeConfig::Create("game");
		game_input.ShowCursor(true)
			.CaptureMouse(true)
			.CentreMouse(false);

		s_input->SetCurrentMode("game");

		dd::InputKeyBindings& bindings = s_input->AccessKeyBindings();
		bindings.BindKey(dd::Key::ESCAPE, dd::InputAction::TOGGLE_DEBUG_UI);
		bindings.BindKey(dd::Key::MOUSE_LEFT, dd::InputAction::SHOOT, "game");
		bindings.BindKey(dd::Key::MOUSE_RIGHT, dd::InputAction::RETURN_TELEPORTER, "game");
		bindings.BindKey(dd::Key::R, dd::InputAction::RESET, "game");
		bindings.BindKey(dd::Key::ENTER, dd::InputAction::NEXT_MAP);
		bindings.BindKey(dd::Key::ENTER, dd::ModifierFlags(dd::Modifier::Shift), dd::InputAction::PREVIOUS_MAP);

		s_music = new sf::Music();
		std::filesystem::path sound_path = dd::File::GetWritePath();
		sound_path /= "sounds";

		std::filesystem::path music_path = sound_path / "bgmusic.ogg";
		s_music->openFromFile(music_path.string());
		s_music->setVolume(50);
		s_music->setLoop(true);
		//s_music->play();

		s_teleportSoundBuffer = new sf::SoundBuffer();
		s_teleportSoundBuffer->loadFromFile((sound_path / "teleport.wav").string());

		s_exitSoundBuffer = new sf::SoundBuffer();
		s_exitSoundBuffer->loadFromFile((sound_path / "exit.wav").string());

		s_throwSoundBuffer = new sf::SoundBuffer();
		s_throwSoundBuffer->loadFromFile((sound_path / "throw.wav").string());

		s_teleReturnSoundBuffer = new sf::SoundBuffer();
		s_teleReturnSoundBuffer->loadFromFile((sound_path / "telereturn.wav").string());

		s_listener = new sf::Listener();
	}

	void LuxportGame::Shutdown()
	{
		if (s_currentMap != nullptr)
		{
			delete s_currentMap;
		}

		s_music->stop();
		delete s_music;

		if (s_activeSound1 != nullptr) delete s_activeSound1;
		if (s_activeSound2 != nullptr) delete s_activeSound2;

		delete s_teleportSoundBuffer;
		delete s_exitSoundBuffer; 
		delete s_throwSoundBuffer;
		delete s_teleReturnSoundBuffer;
	}

	static void ReturnTeleporterToPlayer()
	{
		s_teleporter.RemoveTag(ddc::Tag::Dynamic);

		d2d::Transform2DComponent* player_transform = s_player.Access<d2d::Transform2DComponent>();

		d2d::Transform2DComponent* tele_transform = s_teleporter.Access<d2d::Transform2DComponent>();
		tele_transform->Position = player_transform->Position;
		tele_transform->Scale = glm::vec2(2);
		tele_transform->Update();

		d2d::CirclePhysicsComponent* tele_physics = s_teleporter.Access<d2d::CirclePhysicsComponent>();
		tele_physics->Velocity = glm::vec2(0);
		tele_physics->RestingFrames = 0;

		d2d::SpriteAnimationComponent* player_anim = s_player.Access<d2d::SpriteAnimationComponent>();
		d2d::SpriteAnimationComponent* tele_anim = s_teleporter.Access<d2d::SpriteAnimationComponent>();
		tele_anim->CurrentFrame = player_anim->CurrentFrame;
		tele_anim->Time = player_anim->Time;
		tele_anim->IsPlaying = true;

		d2d::SpriteComponent* tele_sprite = s_teleporter.Access<d2d::SpriteComponent>();
		tele_sprite->Sprite = tele_anim->Frames[tele_anim->CurrentFrame];
	}

	static void ResetPlayerToStart()
	{
		ddc::Entity start_entity = s_currentMap->GetStart();
		d2d::Transform2DComponent* start_tile = start_entity.Access<d2d::Transform2DComponent>();

		glm::vec2 offset = glm::vec2(0, -1);

		d2d::Transform2DComponent* player_transform = s_player.Access<d2d::Transform2DComponent>();
		player_transform->Position = start_tile->Position + offset;
		player_transform->Update();

		d2d::BoxPhysicsComponent* player_physics = s_player.Access<d2d::BoxPhysicsComponent>();
		player_physics->Velocity = glm::vec2(0);
		player_physics->RestingFrames = 0;

		d2d::SpriteAnimationComponent* player_anim = s_player.Access<d2d::SpriteAnimationComponent>();
		player_anim->PlayFromStart();

		ReturnTeleporterToPlayer();
	}

	static void SwitchMap(ddc::EntityLayer& layer, int index)
	{
		DD_ASSERT(index > 0 && index <= MAX_MAP);

		if (s_currentMap != nullptr)
		{
			s_currentMap->Unload(layer);
			delete s_currentMap;
		}

		s_currentMap = new lux::LuxportMap(index);
		s_currentMap->Load(layer);

		ResetPlayerToStart();
	}

	static const glm::vec2 TELEPORTER_DYNAMIC_OFFSET(1, 0.5);
	static const float TELEPORTER_DISTANCE_FACTOR = 10;
	static const float TELEPORTER_MIN_SPEED = 5;
	static const float TELEPORTER_MAX_SPEED = 15;

	static void PlaySound(sf::SoundBuffer* sound_buffer, glm::vec2 position, bool global = false)
	{
		DD_TODO("Sounds disabled");
		return; 

		DD_ASSERT(sound_buffer != nullptr);

		bool use_first = true;

		if (s_activeSound1 != nullptr && s_activeSound1->getStatus() == sf::Sound::Playing)
		{
			use_first = false;
			if (s_activeSound2 != nullptr && s_activeSound2->getStatus() == sf::Sound::Playing)
			{
				if (s_activeSound1->getPlayingOffset() < s_activeSound2->getPlayingOffset())
				{
					use_first = false;
				}
			}
		}

		if (use_first )
		{
			if (s_activeSound1 != nullptr)
			{
				s_activeSound1->stop();
				delete s_activeSound1;
			}

			s_activeSound1 = new sf::Sound(*sound_buffer);
			if (global)
			{
				s_activeSound1->setRelativeToListener(true);
			}
			else
			{
				s_activeSound1->setAttenuation(2);
				s_activeSound1->setPosition(position.x, position.y, 0.0f);
			}
			s_activeSound1->setVolume(70);
			s_activeSound1->play();
		}
		else
		{
			if (s_activeSound2 != nullptr)
			{
				s_activeSound2->stop();
				delete s_activeSound2;
			}

			s_activeSound2 = new sf::Sound(*sound_buffer);

			if (global)
			{
				s_activeSound2->setRelativeToListener(true);
			}
			else
			{
				s_activeSound2->setAttenuation(2);
				s_activeSound2->setPosition(position.x, position.y, 0.0f);
			}
			s_activeSound2->setVolume(70);
			s_activeSound2->play();
		}
	}

	static void ThrowTeleporter()
	{
		s_teleporter.AddTag(ddc::Tag::Dynamic);

		d2d::SpriteAnimationComponent* tele_anim = s_teleporter.Access<d2d::SpriteAnimationComponent>();
		tele_anim->Stop();

		d2d::SpriteComponent* tele_sprite = s_teleporter.Access<d2d::SpriteComponent>();
		tele_sprite->Sprite = s_teleporterDynamicSprite;

		d2d::Transform2DComponent* tele_transform = s_teleporter.Access<d2d::Transform2DComponent>();
		tele_transform->Position += TELEPORTER_DYNAMIC_OFFSET;
		tele_transform->Scale = glm::vec2(1);
		tele_transform->Update();

		dd::MousePosition mouse_pos = s_input->GetMousePosition();
		glm::vec2 coords = s_spriteTileSystem->PixelsToCoord(mouse_pos.Absolute);
		float distance = glm::length(coords - tele_transform->Position);

		float strength = ddm::clamp(distance / TELEPORTER_DISTANCE_FACTOR, 0.0f, 1.0f);
		glm::vec2 dir = (coords - tele_transform->Position) / distance;

		d2d::CirclePhysicsComponent* tele_physics = s_teleporter.Access<d2d::CirclePhysicsComponent>();
		tele_physics->RestingFrames = 0;
		tele_physics->Velocity = dir * ddm::clamp(TELEPORTER_MAX_SPEED * strength, TELEPORTER_MIN_SPEED, TELEPORTER_MAX_SPEED);

		PlaySound(s_throwSoundBuffer, tele_transform->Position);
	}
	
	static void TeleportPlayer()
	{
		d2d::Transform2DComponent* tele_transform = s_teleporter.Access<d2d::Transform2DComponent>();

		d2d::Transform2DComponent* player_transform = s_player.Access<d2d::Transform2DComponent>();
		player_transform->Position = tele_transform->Position - glm::vec2(1.375, 1.375);
		player_transform->Update();

		d2d::BoxPhysicsComponent* player_physics = s_player.Access<d2d::BoxPhysicsComponent>();
		player_physics->RestingFrames = 0;
		player_physics->Velocity = glm::vec2(0);

		ReturnTeleporterToPlayer();

		PlaySound(s_teleportSoundBuffer, tele_transform->Position);
	}

	static void OnTeleporterInput()
	{
		DD_ASSERT(s_teleporter.IsValid());
		d2d::CirclePhysicsComponent* tele_physics = s_teleporter.Access<d2d::CirclePhysicsComponent>();

		if (s_teleporter.HasTag(ddc::Tag::Dynamic))
		{
			if (tele_physics->IsResting())
			{
				TeleportPlayer();
			}

			// teleporter still moving
		}
		else
		{
			ThrowTeleporter();
		}
	}

	void LuxportGame::Update(const dd::GameUpdateData& update)
	{
		if (s_input->GotInput(dd::InputAction::NEXT_MAP))
		{
			s_desiredMapIndex = ddm::wrap(s_desiredMapIndex + 1, 1, MAX_MAP);
		}

		if (s_input->GotInput(dd::InputAction::PREVIOUS_MAP))
		{
			s_desiredMapIndex = ddm::wrap(s_desiredMapIndex - 1, 1, MAX_MAP);
		}

		if (s_currentMap == nullptr || s_desiredMapIndex != s_currentMap->GetIndex())
		{
			SwitchMap(update.EntityLayer(), s_desiredMapIndex);
		}

		DD_TODO("Try using ScratchEntity here.");

		d2d::Transform2DComponent* tele_transform = s_teleporter.Access<d2d::Transform2DComponent>();
		d2d::Transform2DComponent* player_transform = s_player.Access<d2d::Transform2DComponent>();

		if (s_input->GotInput(dd::InputAction::RESET))
		{
			ResetPlayerToStart();
			PlaySound(s_teleReturnSoundBuffer, tele_transform->Position);
		}

		if (s_input->GotInput(dd::InputAction::SHOOT))
		{
			OnTeleporterInput();
		}

		if (s_input->GotInput(dd::InputAction::RETURN_TELEPORTER))
		{
			PlaySound(s_teleReturnSoundBuffer, tele_transform->Position);

			ReturnTeleporterToPlayer();
		}
		 
		if (player_transform->Position.x < 0 || player_transform->Position.x > MAP_SIZE.x ||
			player_transform->Position.y < 0 || player_transform->Position.y > MAP_SIZE.y)
		{
			PlaySound(s_exitSoundBuffer, player_transform->Position, true);
			ResetPlayerToStart();
		}

		{
			d2d::Transform2DComponent* end_tile = s_currentMap->GetEnd().Access<d2d::Transform2DComponent>();
			d2d::BoxPhysicsComponent* player_box = s_player.Access<d2d::BoxPhysicsComponent>();
			glm::vec2 player_min = player_transform->Position + player_box->HitBox.Min * player_transform->Scale;
			glm::vec2 player_max = player_transform->Position + player_box->HitBox.Max * player_transform->Scale;
			if (ddm::BoxBoxIntersect(player_min, player_max, end_tile->Position - glm::vec2(1), end_tile->Position + glm::vec2(2)))
			{
				s_desiredMapIndex = s_currentMap->GetIndex() + 1;
				PlaySound(s_exitSoundBuffer, player_transform->Position, true);
			}
		}

		if (!s_teleporter.HasTag(ddc::Tag::Dynamic))
		{
			ReturnTeleporterToPlayer();
		}

		if (tele_transform->Position.x < 0 || tele_transform->Position.x > MAP_SIZE.x ||
			tele_transform->Position.y < 0 || tele_transform->Position.y > MAP_SIZE.y)
		{
			ReturnTeleporterToPlayer();
		}

		s_listener->setPosition(sf::Vector3(player_transform->Position.x, player_transform->Position.y, 0.0f));
	}
}