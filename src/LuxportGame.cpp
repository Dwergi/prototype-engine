//
// Luxport.h - Game file for Luxport GMTK Game Jame 2019.
// Copyright (C) Sebastian Nordgren 
// August 3rd 2019
//

#include "PCH.h"
#include "LuxportGame.h"

#include "Box2DPhysicsComponent.h"
#include "Circle2DPhysicsComponent.h"
#include "EntityPrototype.h"
#include "File.h"
#include "InputBindings.h"
#include "IInputSource.h"
#include "InputSystem.h"
#include "IWindow.h"
#include "Light2DComponent.h"
#include "Light2DRenderer.h"
#include "LuxportMap.h"
#include "OrthoCamera.h"
#include "Physics2DSystem.h"
#include "SpriteAnimationComponent.h"
#include "SpriteAnimationSystem.h"
#include "SpriteComponent.h"
#include "SpriteRenderer.h"
#include "SpriteSheet.h"
#include "SpriteTileComponent.h"
#include "SpriteTileSystem.h"
#include "Texture.h"
#include "WorldRenderer.h"

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

#pragma optimize("",off)

namespace lux
{
	static dd::Service<ddr::SpriteManager> s_spriteManager;
	static dd::Service<ddr::SpriteSheetManager> s_spriteSheetManager;
	static dd::Service<lux::SpriteTileSystem> s_spriteTileSystem;
	static dd::Service<ddr::TextureManager> s_textureManager;
	static dd::Service<dd::IWindow> s_window;
	static dd::Service<ddr::WorldRenderer> s_renderer;
	static dd::Service<dd::IInputSource> s_inputSource;
	static dd::Service<dd::InputBindings> s_inputBindings;
	static dd::Service<dd::InputSystem> s_inputSystem;

	static ddc::Entity s_player;
	static ddc::Entity s_teleporter;
	static ddr::SpriteHandle s_teleporterDynamicSprite;

	static bool s_shouldThrow { false };
	static bool s_shouldReturn { false };
	static bool s_shouldReset { false };

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

	static std::vector<ddc::Entity> s_mapEntities;

	static sf::Music* s_music = nullptr;
	static sf::SoundBuffer* s_teleportSoundBuffer = nullptr;
	static sf::SoundBuffer* s_exitSoundBuffer = nullptr;
	static sf::SoundBuffer* s_throwSoundBuffer = nullptr;
	static sf::SoundBuffer* s_teleReturnSoundBuffer = nullptr;

	static sf::Sound* s_activeSound1 = nullptr;
	static sf::Sound* s_activeSound2 = nullptr;
	static sf::Listener* s_listener = nullptr;

	static void OnSwitchMap(dd::InputAction action, dd::InputType type)
	{
		if (type == dd::InputType::RELEASED)
		{
			if (action == dd::InputAction::PREVIOUS_MAP)
			{
				--s_desiredMapIndex;
				if (s_desiredMapIndex <= 0)
				{
					s_desiredMapIndex = MAX_MAP;
				}
			}
			else if (action == dd::InputAction::NEXT_MAP)
			{
				++s_desiredMapIndex;

				if (s_desiredMapIndex > MAX_MAP)
				{
					s_desiredMapIndex = 1;
				}
			}
		}
	}

	static void OnThrowTeleporter(dd::InputAction action, dd::InputType type)
	{
		if (type == dd::InputType::RELEASED && action == dd::InputAction::SHOOT)
		{
			s_shouldThrow = true;
		}
	}

	static void OnReturnTeleporter(dd::InputAction action, dd::InputType type)
	{
		if (type == dd::InputType::RELEASED && action == dd::InputAction::RETURN_TELEPORTER)
		{
			s_shouldReturn = true;
		}
	}

	static void OnReset(dd::InputAction action, dd::InputType type)
	{
		if (type == dd::InputType::RELEASED && action == dd::InputAction::RESET)
		{
			s_shouldReset = true;
		}
	}

	static ddc::Entity CreateTeleporter(ddc::World& world)
	{
		ddc::Entity teleporter = world.CreateEntity<dd::SpriteComponent, lux::SpriteTileComponent, dd::Circle2DPhysicsComponent, dd::SpriteAnimationComponent>();
		world.AddTag(teleporter, ddc::Tag::Visible);

		{
			ddr::SpriteSheetHandle fg_spritesheet_h = s_spriteSheetManager->Find(MAP_FOREGROUND);
			const ddr::SpriteSheet* fg_spritesheet = fg_spritesheet_h.Get();
			s_teleporterDynamicSprite = fg_spritesheet->Get(67);
		}

		ddr::SpriteSheetHandle char_spritesheet_h = s_spriteSheetManager->Find(PLAYER_SPRITESHEET);
		const ddr::SpriteSheet* char_spritesheet = char_spritesheet_h.Get();

		dd::SpriteComponent* sprite_cmp = world.Access<dd::SpriteComponent>(teleporter);

		dd::SpriteAnimationComponent* sprite_anim_cmp = world.Access<dd::SpriteAnimationComponent>(teleporter);
		sprite_anim_cmp->Frames.Add(char_spritesheet->Get(4, 0));
		sprite_anim_cmp->Frames.Add(char_spritesheet->Get(5, 0));
		sprite_anim_cmp->Frames.Add(char_spritesheet->Get(6, 0));
		sprite_anim_cmp->Frames.Add(char_spritesheet->Get(7, 0));
		sprite_anim_cmp->Framerate = 4;

		sprite_cmp->Sprite = sprite_anim_cmp->Frames[0];
		sprite_cmp->ZIndex = TELEPORTER_Z_INDEX;

		lux::SpriteTileComponent* sprite_tile_cmp = world.Access<lux::SpriteTileComponent>(teleporter);
		sprite_tile_cmp->Scale = glm::vec2(2);

		dd::Circle2DPhysicsComponent* physics = world.Access<dd::Circle2DPhysicsComponent>(teleporter);
		physics->Radius = TELEPORTER_RADIUS;
		physics->Elasticity = TELEPORTER_ELASTICITY;

		lux::Light2DComponent& light = world.Add<lux::Light2DComponent>(teleporter);
		light.Type = lux::LightType::Teleporter;
		return teleporter;
	}

	static ddc::Entity CreatePlayer(ddc::World& world)
	{
		ddc::Entity player = world.CreateEntity<dd::SpriteComponent, lux::SpriteTileComponent, dd::SpriteAnimationComponent, dd::Box2DPhysicsComponent>();
		world.AddTag(player, ddc::Tag::Visible);

		ddr::SpriteSheetHandle spritesheet_h = s_spriteSheetManager->Find(PLAYER_SPRITESHEET);
		const ddr::SpriteSheet* spritesheet = spritesheet_h.Get();

		lux::SpriteTileComponent* sprite_tile_cmp = world.Access<lux::SpriteTileComponent>(player);
		sprite_tile_cmp->Scale = glm::vec2(2);

		dd::SpriteAnimationComponent* sprite_anim_cmp = world.Access<dd::SpriteAnimationComponent>(player);
		sprite_anim_cmp->Frames.Add(spritesheet->Get(0, 0));
		sprite_anim_cmp->Frames.Add(spritesheet->Get(1, 0));
		sprite_anim_cmp->Frames.Add(spritesheet->Get(2, 0));
		sprite_anim_cmp->Frames.Add(spritesheet->Get(3, 0));
		sprite_anim_cmp->Framerate = 4;

		dd::SpriteComponent* sprite_cmp = world.Access<dd::SpriteComponent>(player);
		sprite_cmp->Sprite = sprite_anim_cmp->Frames[0];
		sprite_cmp->ZIndex = PLAYER_Z_INDEX;

		for (ddr::SpriteHandle frame : sprite_anim_cmp->Frames)
		{
			DD_ASSERT(frame.IsValid());
		}

		dd::Box2DPhysicsComponent* physics = world.Access<dd::Box2DPhysicsComponent>(player);
		physics->HitBoxMin = glm::vec2(0.28, 0.0625);
		physics->HitBoxMax = glm::vec2(0.72, 1);
		physics->Elasticity = 0;

		return player;
	}

	void LuxportGame::Initialize(ddc::World& world)
	{
		dd::Services::RegisterInterface<ddr::ICamera>(new ddr::OrthoCamera());

		lux::Light2DRenderer& lights_renderer = dd::Services::Register(new lux::Light2DRenderer());
		s_renderer->Register(lights_renderer);

		ddr::SpriteRenderer& sprite_renderer = dd::Services::Register(new ddr::SpriteRenderer());
		s_renderer->Register(sprite_renderer);

		dd::Physics2DSystem& physics_system = dd::Services::Register(new dd::Physics2DSystem());
		world.RegisterSystem(physics_system);

		dd::SpriteAnimationSystem& sprite_anim_system = dd::Services::Register(new dd::SpriteAnimationSystem());
		world.RegisterSystem(sprite_anim_system);

		lux::SpriteTileSystem& sprite_tile_system = dd::Services::Register(new lux::SpriteTileSystem(MAP_SIZE, 16));
		sprite_tile_system.DependsOn(physics_system);
		sprite_tile_system.DependsOn(sprite_anim_system);
		world.RegisterSystem(sprite_tile_system);

		ddr::TextureHandle spritesheet_tex_h = s_textureManager->Load(PLAYER_SPRITESHEET);
		s_spriteSheetManager->Load(PLAYER_SPRITESHEET, spritesheet_tex_h, glm::ivec2(32));

		ddr::TextureHandle background_tex_h = s_textureManager->Load(MAP_BACKGROUND);
		s_spriteSheetManager->Load(MAP_BACKGROUND, background_tex_h, glm::ivec2(16));

		ddr::TextureHandle foreground_tex_h = s_textureManager->Load(MAP_FOREGROUND);
		s_spriteSheetManager->Load(MAP_FOREGROUND, foreground_tex_h, glm::ivec2(16));

		s_teleporter = CreateTeleporter(world);
		s_player = CreatePlayer(world);

		dd::InputMode* game_input = dd::InputMode::Access(dd::InputMode::GAME);
		game_input->ShowCursor(true)
			.CaptureMouse(true)
			.CentreMouse(false);

		s_inputBindings->RegisterHandler(dd::InputAction::NEXT_MAP, &OnSwitchMap);
		s_inputBindings->RegisterHandler(dd::InputAction::PREVIOUS_MAP, &OnSwitchMap);
		s_inputBindings->RegisterHandler(dd::InputAction::SHOOT, &OnThrowTeleporter);
		s_inputBindings->RegisterHandler(dd::InputAction::RETURN_TELEPORTER, &OnReturnTeleporter);
		s_inputBindings->RegisterHandler(dd::InputAction::RESET, &OnReset);

		s_inputSource->BindKey(dd::InputAction::NEXT_MAP, dd::Key::ENTER);
		s_inputSource->BindKey(dd::InputAction::PREVIOUS_MAP, dd::Key::ENTER, dd::SHIFT);
		s_inputSource->BindKey(dd::InputAction::RETURN_TELEPORTER, dd::Key::MOUSE_RIGHT);
		s_inputSource->BindKey(dd::InputAction::RESET, dd::Key::R);

		s_music = new sf::Music();
		std::filesystem::path sound_path = dd::File::GetDataRoot();
		sound_path /= "sounds";

		std::filesystem::path music_path = sound_path / "bgmusic.ogg";
		s_music->openFromFile(music_path.string());
		s_music->setVolume(50);
		s_music->setLoop(true);
		s_music->play();

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

	void LuxportGame::Shutdown(ddc::World& world)
	{
		if (s_currentMap != nullptr)
		{
			s_currentMap->Unload(world);
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

	static void ReturnTeleporterToPlayer(ddc::World& world)
	{
		world.RemoveTag(s_teleporter, ddc::Tag::Dynamic);

		lux::SpriteTileComponent* player_tile = world.Access<lux::SpriteTileComponent>(s_player);

		lux::SpriteTileComponent* tele_tile = world.Access<lux::SpriteTileComponent>(s_teleporter);
		tele_tile->Coordinate = player_tile->Coordinate;
		tele_tile->Scale = glm::vec2(2);

		dd::Circle2DPhysicsComponent* tele_physics = world.Access<dd::Circle2DPhysicsComponent>(s_teleporter);
		tele_physics->Reset();

		dd::SpriteAnimationComponent* player_anim = world.Access<dd::SpriteAnimationComponent>(s_player);
		dd::SpriteAnimationComponent* tele_anim = world.Access<dd::SpriteAnimationComponent>(s_teleporter);
		tele_anim->CurrentFrame = player_anim->CurrentFrame;
		tele_anim->Time = player_anim->Time;
		tele_anim->IsPlaying = true;

		dd::SpriteComponent* tele_sprite = world.Access<dd::SpriteComponent>(s_teleporter);
		tele_sprite->Sprite = tele_anim->Frames[tele_anim->CurrentFrame];
	}

	static void ResetPlayerToStart(ddc::World& world)
	{
		ddc::Entity start_entity = s_currentMap->GetStart();
		lux::SpriteTileComponent* start_tile = world.Access<lux::SpriteTileComponent>(start_entity);

		glm::vec2 offset = glm::vec2(0, -1);

		lux::SpriteTileComponent* player_tile = world.Access<lux::SpriteTileComponent>(s_player);
		player_tile->Coordinate = start_tile->Coordinate + offset;

		dd::Box2DPhysicsComponent* player_physics = world.Access<dd::Box2DPhysicsComponent>(s_player);
		player_physics->Reset();

		dd::SpriteAnimationComponent* player_anim = world.Access<dd::SpriteAnimationComponent>(s_player);
		player_anim->PlayFromStart();

		ReturnTeleporterToPlayer(world);
	}

	static void SwitchMapUpdate(ddc::World& world)
	{
		DD_ASSERT(s_desiredMapIndex > 0 && s_desiredMapIndex <= MAX_MAP);

		if (s_currentMap != nullptr)
		{
			s_currentMap->Unload(world);
			delete s_currentMap;
		}

		s_currentMap = new lux::LuxportMap(s_desiredMapIndex);
		s_currentMap->Load(world);

		ResetPlayerToStart(world);
	}

	static const glm::vec2 TELEPORTER_DYNAMIC_OFFSET(1, 0.5);
	static const float TELEPORTER_DISTANCE_FACTOR = 10;
	static const float TELEPORTER_MIN_SPEED = 5;
	static const float TELEPORTER_MAX_SPEED = 15;

	static void PlaySound(sf::SoundBuffer* sound_buffer, glm::vec2 position, bool global = false)
	{
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

	static void ThrowTeleporter(ddc::World& world)
	{
		world.AddTag(s_teleporter, ddc::Tag::Dynamic);

		dd::SpriteAnimationComponent* tele_anim = world.Access<dd::SpriteAnimationComponent>(s_teleporter);
		tele_anim->Stop();

		dd::SpriteComponent* tele_sprite = world.Access<dd::SpriteComponent>(s_teleporter);
		tele_sprite->Sprite = s_teleporterDynamicSprite;

		lux::SpriteTileComponent* tele_tile = world.Access<lux::SpriteTileComponent>(s_teleporter);
		tele_tile->Coordinate += TELEPORTER_DYNAMIC_OFFSET;
		tele_tile->Scale = glm::vec2(1);

		dd::MousePosition mouse_pos = s_inputSource->GetMousePosition();
		glm::vec2 coords = s_spriteTileSystem->PixelsToCoord(mouse_pos.Absolute);
		float distance = glm::length(coords - tele_tile->Coordinate);

		float strength = ddm::clamp(distance / TELEPORTER_DISTANCE_FACTOR, 0.0f, 1.0f);
		glm::vec2 dir = (coords - tele_tile->Coordinate) / distance;

		dd::Circle2DPhysicsComponent* tele_physics = world.Access<dd::Circle2DPhysicsComponent>(s_teleporter);
		tele_physics->Reset(); 
		tele_physics->Velocity = dir * ddm::clamp(TELEPORTER_MAX_SPEED * strength, TELEPORTER_MIN_SPEED, TELEPORTER_MAX_SPEED);

		PlaySound(s_throwSoundBuffer, tele_tile->Coordinate);
	}
	
	static void TeleportPlayer(ddc::World& world)
	{
		lux::SpriteTileComponent* tele_tile = world.Access<lux::SpriteTileComponent>(s_teleporter);

		lux::SpriteTileComponent* player_tile = world.Access<lux::SpriteTileComponent>(s_player);
		player_tile->Coordinate = tele_tile->Coordinate - glm::vec2(1.375, 1.375);

		dd::Box2DPhysicsComponent* player_physics = world.Access<dd::Box2DPhysicsComponent>(s_player);
		player_physics->Reset();

		ReturnTeleporterToPlayer(world);

		PlaySound(s_teleportSoundBuffer, tele_tile->Coordinate);
	}

	static void OnTeleporterInput(ddc::World& world)
	{
		DD_ASSERT(s_teleporter.IsValid());
		dd::Circle2DPhysicsComponent* tele_physics = world.Access<dd::Circle2DPhysicsComponent>(s_teleporter);

		if (world.HasTag(s_teleporter, ddc::Tag::Dynamic))
		{
			if (tele_physics->IsResting())
			{
				TeleportPlayer(world);
			}

			// teleporter still moving
		}
		else
		{
			ThrowTeleporter(world);
		}
	}

	static bool BoxBoxIntersect(glm::vec2 a_min, glm::vec2 a_max, glm::vec2 b_min, glm::vec2 b_max)
	{
		return glm::all(glm::lessThanEqual(a_min, b_max)) && glm::all(glm::greaterThanEqual(a_max, b_min));
	}

	void LuxportGame::Update(ddc::World& world)
	{
		if (s_currentMap == nullptr || s_currentMap->GetIndex() != s_desiredMapIndex)
		{
			SwitchMapUpdate(world);
		}

		lux::SpriteTileComponent* tele_tile = world.Access<lux::SpriteTileComponent>(s_teleporter);
		lux::SpriteTileComponent* player_tile = world.Access<lux::SpriteTileComponent>(s_player);

		if (s_shouldReset)
		{
			ResetPlayerToStart(world);
			PlaySound(s_teleReturnSoundBuffer, tele_tile->Coordinate);
			s_shouldReset = false;
		}

		if (s_shouldThrow)
		{
			OnTeleporterInput(world);
			s_shouldThrow = false;
		}

		if (s_shouldReturn)
		{
			PlaySound(s_teleReturnSoundBuffer, tele_tile->Coordinate);

			ReturnTeleporterToPlayer(world);
			s_shouldReturn = false;
		}
		 
		if (player_tile->Coordinate.x < 0 || player_tile->Coordinate.x > MAP_SIZE.x ||
			player_tile->Coordinate.y < 0 || player_tile->Coordinate.y > MAP_SIZE.y)
		{
			PlaySound(s_exitSoundBuffer, player_tile->Coordinate, true);
			ResetPlayerToStart(world);
		}

		{
			lux::SpriteTileComponent* end_tile = world.Access<lux::SpriteTileComponent>(s_currentMap->GetEnd());
			dd::Box2DPhysicsComponent* player_box = world.Access<dd::Box2DPhysicsComponent>(s_player);
			glm::vec2 player_min = player_tile->Coordinate + player_box->HitBoxMin * player_tile->Scale;
			glm::vec2 player_max = player_tile->Coordinate + player_box->HitBoxMax * player_tile->Scale;
			if (BoxBoxIntersect(player_min, player_max, end_tile->Coordinate - glm::vec2(1), end_tile->Coordinate + glm::vec2(2)))
			{
				s_desiredMapIndex = s_currentMap->GetIndex() + 1;
				PlaySound(s_exitSoundBuffer, player_tile->Coordinate, true);
			}
		}

		if (!world.HasTag(s_teleporter, ddc::Tag::Dynamic))
		{
			ReturnTeleporterToPlayer(world);
		}

		if (tele_tile->Coordinate.x < 0 || tele_tile->Coordinate.x > MAP_SIZE.x ||
			tele_tile->Coordinate.y < 0 || tele_tile->Coordinate.y > MAP_SIZE.y)
		{
			ReturnTeleporterToPlayer(world);
		}

		s_listener->setPosition(sf::Vector3(player_tile->Coordinate.x, player_tile->Coordinate.y, 0.0f));
	}

	void LuxportGame::RenderUpdate(ddc::World& world)
	{

	}
}