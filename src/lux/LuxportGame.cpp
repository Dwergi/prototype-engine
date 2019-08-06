//
// Luxport.h - Game file for Luxport GMTK Game Jame 2019.
// Copyright (C) Sebastian Nordgren 
// August 3rd 2019
//

#include "PCH.h"
#include "LuxportGame.h"

#include "d2d/BoxPhysicsComponent.h"
#include "d2d/CirclePhysicsComponent.h"
#include "EntityPrototype.h"
#include "File.h"
#include "HitTest.h"
#include "Input.h"
#include "IWindow.h"
#include "lux/LuxLightComponent.h"
#include "lux/LuxLightRenderer.h"
#include "lux/LuxportMap.h"
#include "OrthoCamera.h"
#include "d2d/Physics2DSystem.h"
#include "d2d/SpriteAnimationComponent.h"
#include "d2d/SpriteAnimationSystem.h"
#include "d2d/SpriteComponent.h"
#include "SpriteRenderer.h"
#include "SpriteSheet.h"
#include "d2d/SpriteTileComponent.h"
#include "d2d/SpriteTileSystem.h"
#include "SystemManager.h"
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
	static dd::Service<d2d::SpriteTileSystem> s_spriteTileSystem;
	static dd::Service<ddr::TextureManager> s_textureManager;
	static dd::Service<dd::IWindow> s_window;
	static dd::Service<ddr::WorldRenderer> s_renderer;
	static dd::Service<dd::Input> s_input;
	static dd::Service<ddc::SystemManager> s_systemManager;

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

	static std::vector<ddc::Entity> s_mapspace;

	static sf::Music* s_music = nullptr;
	static sf::SoundBuffer* s_teleportSoundBuffer = nullptr;
	static sf::SoundBuffer* s_exitSoundBuffer = nullptr;
	static sf::SoundBuffer* s_throwSoundBuffer = nullptr;
	static sf::SoundBuffer* s_teleReturnSoundBuffer = nullptr;

	static sf::Sound* s_activeSound1 = nullptr;
	static sf::Sound* s_activeSound2 = nullptr;
	static sf::Listener* s_listener = nullptr;

	static ddc::Entity CreateTeleporter(ddc::EntitySpace& space)
	{
		ddc::Entity teleporter = space.CreateEntity<d2d::SpriteComponent, d2d::SpriteTileComponent, d2d::CirclePhysicsComponent, d2d::SpriteAnimationComponent>();
		space.AddTag(teleporter, ddc::Tag::Visible);

		{
			ddr::SpriteSheetHandle fg_spritesheet_h = s_spriteSheetManager->Find(MAP_FOREGROUND);
			const ddr::SpriteSheet* fg_spritesheet = fg_spritesheet_h.Get();
			s_teleporterDynamicSprite = fg_spritesheet->Get(67);
		}

		ddr::SpriteSheetHandle char_spritesheet_h = s_spriteSheetManager->Find(PLAYER_SPRITESHEET);
		const ddr::SpriteSheet* char_spritesheet = char_spritesheet_h.Get();

		d2d::SpriteComponent* sprite_cmp = space.Access<d2d::SpriteComponent>(teleporter);

		d2d::SpriteAnimationComponent* sprite_anim_cmp = space.Access<d2d::SpriteAnimationComponent>(teleporter);
		sprite_anim_cmp->Frames.Add(char_spritesheet->Get(4, 0));
		sprite_anim_cmp->Frames.Add(char_spritesheet->Get(5, 0));
		sprite_anim_cmp->Frames.Add(char_spritesheet->Get(6, 0));
		sprite_anim_cmp->Frames.Add(char_spritesheet->Get(7, 0));
		sprite_anim_cmp->Framerate = 4;

		sprite_cmp->Sprite = sprite_anim_cmp->Frames[0];
		sprite_cmp->ZIndex = TELEPORTER_Z_INDEX;

		d2d::SpriteTileComponent* sprite_tile_cmp = space.Access<d2d::SpriteTileComponent>(teleporter);
		sprite_tile_cmp->Scale = glm::vec2(2);

		d2d::CirclePhysicsComponent* physics = space.Access<d2d::CirclePhysicsComponent>(teleporter);
		physics->Radius = TELEPORTER_RADIUS;
		physics->Elasticity = TELEPORTER_ELASTICITY;

		lux::LuxLightComponent& light = space.Add<lux::LuxLightComponent>(teleporter);
		light.Type = lux::LightType::Teleporter;
		return teleporter;
	}

	static ddc::Entity CreatePlayer(ddc::EntitySpace& space)
	{
		ddc::Entity player = space.CreateEntity<d2d::SpriteComponent, d2d::SpriteTileComponent, d2d::SpriteAnimationComponent, d2d::BoxPhysicsComponent>();
		space.AddTag(player, ddc::Tag::Visible);

		ddr::SpriteSheetHandle spritesheet_h = s_spriteSheetManager->Find(PLAYER_SPRITESHEET);
		const ddr::SpriteSheet* spritesheet = spritesheet_h.Get();

		d2d::SpriteTileComponent* sprite_tile_cmp = space.Access<d2d::SpriteTileComponent>(player);
		sprite_tile_cmp->Scale = glm::vec2(2);

		d2d::SpriteAnimationComponent* sprite_anim_cmp = space.Access<d2d::SpriteAnimationComponent>(player);
		sprite_anim_cmp->Frames.Add(spritesheet->Get(0, 0));
		sprite_anim_cmp->Frames.Add(spritesheet->Get(1, 0));
		sprite_anim_cmp->Frames.Add(spritesheet->Get(2, 0));
		sprite_anim_cmp->Frames.Add(spritesheet->Get(3, 0));
		sprite_anim_cmp->Framerate = 4;

		d2d::SpriteComponent* sprite_cmp = space.Access<d2d::SpriteComponent>(player);
		sprite_cmp->Sprite = sprite_anim_cmp->Frames[0];
		sprite_cmp->ZIndex = PLAYER_Z_INDEX;

		for (ddr::SpriteHandle frame : sprite_anim_cmp->Frames)
		{
			DD_ASSERT(frame.IsValid());
		}

		d2d::BoxPhysicsComponent* physics = space.Access<d2d::BoxPhysicsComponent>(player);
		physics->HitBoxMin = glm::vec2(0.28, 0.0625);
		physics->HitBoxMax = glm::vec2(0.72, 1);
		physics->Elasticity = 0;

		return player;
	}

	void LuxportGame::Initialize(const dd::GameUpdateData& update_data)
	{
		dd::Services::RegisterInterface<ddr::ICamera>(new ddr::OrthoCamera());

		ddc::EntitySpace& space = update_data.EntitySpace();

		dd::Services::Register(new ddr::SpriteManager());
		dd::Services::Register(new ddr::SpriteSheetManager(*s_spriteManager));

		lux::LuxLightRenderer& lights_renderer = dd::Services::Register(new lux::LuxLightRenderer());
		s_renderer->Register(lights_renderer);

		ddr::SpriteRenderer& sprite_renderer = dd::Services::Register(new ddr::SpriteRenderer());
		s_renderer->Register(sprite_renderer);

		d2d::PhysicsSystem& physics_system = dd::Services::Register(new d2d::PhysicsSystem());
		s_systemManager->Register(physics_system);

		d2d::SpriteAnimationSystem& sprite_anim_system = dd::Services::Register(new d2d::SpriteAnimationSystem());
		s_systemManager->Register(sprite_anim_system);

		d2d::SpriteTileSystem& sprite_tile_system = dd::Services::Register(new d2d::SpriteTileSystem(MAP_SIZE, 16));
		sprite_tile_system.DependsOn(physics_system);
		sprite_tile_system.DependsOn(sprite_anim_system);
		s_systemManager->Register(sprite_tile_system);

		ddr::TextureHandle spritesheet_tex_h = s_textureManager->Load(PLAYER_SPRITESHEET);
		s_spriteSheetManager->Load(PLAYER_SPRITESHEET, spritesheet_tex_h, glm::ivec2(32));

		ddr::TextureHandle background_tex_h = s_textureManager->Load(MAP_BACKGROUND);
		s_spriteSheetManager->Load(MAP_BACKGROUND, background_tex_h, glm::ivec2(16));

		ddr::TextureHandle foreground_tex_h = s_textureManager->Load(MAP_FOREGROUND);
		s_spriteSheetManager->Load(MAP_FOREGROUND, foreground_tex_h, glm::ivec2(16));

		s_teleporter = CreateTeleporter(space);
		s_player = CreatePlayer(space);

		dd::InputModeConfig& game_input = dd::InputModeConfig::Create("game");
		game_input.ShowCursor(true)
			.CaptureMouse(true)
			.CentreMouse(false);

		s_input->SetCurrentMode("game");

		/*s_inputBindings->AddHandler(dd::InputAction::NEXT_MAP, &OnSwitchMap);
		s_inputBindings->AddHandler(dd::InputAction::PREVIOUS_MAP, &OnSwitchMap);
		s_inputBindings->AddHandler(dd::InputAction::SHOOT, &OnThrowTeleporter);
		s_inputBindings->AddHandler(dd::InputAction::RETURN_TELEPORTER, &OnReturnTeleporter);
		s_inputBindings->AddHandler(dd::InputAction::RESET, &OnReset);*/

		/*s_inputSource->BindKey(dd::InputAction::NEXT_MAP, dd::Key::ENTER);
		s_inputSource->BindKey(dd::InputAction::PREVIOUS_MAP, dd::Key::ENTER, dd::SHIFT);
		s_inputSource->BindKey(dd::InputAction::RETURN_TELEPORTER, dd::Key::MOUSE_RIGHT);
		s_inputSource->BindKey(dd::InputAction::RESET, dd::Key::R);*/

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

	void LuxportGame::Shutdown(const dd::GameUpdateData& update_data)
	{
		if (s_currentMap != nullptr)
		{
			s_currentMap->Unload(update_data.EntitySpace());
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

		d2d::SpriteTileComponent* player_tile = s_player.Access<d2d::SpriteTileComponent>();

		d2d::SpriteTileComponent* tele_tile = s_teleporter.Access<d2d::SpriteTileComponent>();
		tele_tile->Coordinate = player_tile->Coordinate;
		tele_tile->Scale = glm::vec2(2);

		d2d::CirclePhysicsComponent* tele_physics = s_teleporter.Access<d2d::CirclePhysicsComponent>();
		tele_physics->Reset();

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
		DD_TODO("It would be good to be able to make a full copy of an entity as a scratch copy instead of hitting the EntitySpace multiple times a frame");

		ddc::Entity start_entity = s_currentMap->GetStart();
		d2d::SpriteTileComponent* start_tile = start_entity.Access<d2d::SpriteTileComponent>();

		glm::vec2 offset = glm::vec2(0, -1);

		d2d::SpriteTileComponent* player_tile = s_player.Access<d2d::SpriteTileComponent>();
		player_tile->Coordinate = start_tile->Coordinate + offset;

		d2d::BoxPhysicsComponent* player_physics = s_player.Access<d2d::BoxPhysicsComponent>();
		player_physics->Reset();

		d2d::SpriteAnimationComponent* player_anim = s_player.Access<d2d::SpriteAnimationComponent>();
		player_anim->PlayFromStart();

		ReturnTeleporterToPlayer();
	}

	static void SwitchMap(ddc::EntitySpace& space, int index)
	{
		DD_ASSERT(index > 0 && index <= MAX_MAP);

		if (s_currentMap != nullptr)
		{
			s_currentMap->Unload(space);
			delete s_currentMap;
		}

		s_currentMap = new lux::LuxportMap(index);
		s_currentMap->Load(space);

		ResetPlayerToStart();
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

	static void ThrowTeleporter()
	{
		s_teleporter.AddTag(ddc::Tag::Dynamic);

		d2d::SpriteAnimationComponent* tele_anim = s_teleporter.Access<d2d::SpriteAnimationComponent>();
		tele_anim->Stop();

		d2d::SpriteComponent* tele_sprite = s_teleporter.Access<d2d::SpriteComponent>();
		tele_sprite->Sprite = s_teleporterDynamicSprite;

		d2d::SpriteTileComponent* tele_tile = s_teleporter.Access<d2d::SpriteTileComponent>();
		tele_tile->Coordinate += TELEPORTER_DYNAMIC_OFFSET;
		tele_tile->Scale = glm::vec2(1);

		dd::MousePosition mouse_pos = s_input->GetMousePosition();
		glm::vec2 coords = s_spriteTileSystem->PixelsToCoord(mouse_pos.Absolute);
		float distance = glm::length(coords - tele_tile->Coordinate);

		float strength = ddm::clamp(distance / TELEPORTER_DISTANCE_FACTOR, 0.0f, 1.0f);
		glm::vec2 dir = (coords - tele_tile->Coordinate) / distance;

		d2d::CirclePhysicsComponent* tele_physics = s_teleporter.Access<d2d::CirclePhysicsComponent>();
		tele_physics->Reset(); 
		tele_physics->Velocity = dir * ddm::clamp(TELEPORTER_MAX_SPEED * strength, TELEPORTER_MIN_SPEED, TELEPORTER_MAX_SPEED);

		PlaySound(s_throwSoundBuffer, tele_tile->Coordinate);
	}
	
	static void TeleportPlayer()
	{
		d2d::SpriteTileComponent* tele_tile = s_teleporter.Access<d2d::SpriteTileComponent>();

		d2d::SpriteTileComponent* player_tile = s_player.Access<d2d::SpriteTileComponent>();
		player_tile->Coordinate = tele_tile->Coordinate - glm::vec2(1.375, 1.375);

		d2d::BoxPhysicsComponent* player_physics = s_player.Access<d2d::BoxPhysicsComponent>();
		player_physics->Reset();

		ReturnTeleporterToPlayer();

		PlaySound(s_teleportSoundBuffer, tele_tile->Coordinate);
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
		s_spriteManager->Update();
		s_spriteSheetManager->Update();

		if (s_currentMap == nullptr)
		{
			SwitchMap(update.EntitySpace(), 1);
		}

		if (s_input->GotInput(dd::InputAction::NEXT_MAP))
		{
			SwitchMap(update.EntitySpace(), ddm::wrap(s_currentMap->GetIndex() + 1, 0, MAX_MAP));
		}

		if (s_input->GotInput(dd::InputAction::PREVIOUS_MAP))
		{
			SwitchMap(update.EntitySpace(), ddm::wrap(s_currentMap->GetIndex() - 1, 0, MAX_MAP));
		}

		d2d::SpriteTileComponent* tele_tile = s_teleporter.Access<d2d::SpriteTileComponent>();
		d2d::SpriteTileComponent* player_tile = s_player.Access<d2d::SpriteTileComponent>();

		if (s_input->GotInput(dd::InputAction::RESET))
		{
			ResetPlayerToStart();
			PlaySound(s_teleReturnSoundBuffer, tele_tile->Coordinate);
		}

		if (s_input->GotInput(dd::InputAction::SHOOT))
		{
			OnTeleporterInput();
		}

		if (s_input->GotInput(dd::InputAction::RETURN_TELEPORTER))
		{
			PlaySound(s_teleReturnSoundBuffer, tele_tile->Coordinate);

			ReturnTeleporterToPlayer();
		}
		 
		if (player_tile->Coordinate.x < 0 || player_tile->Coordinate.x > MAP_SIZE.x ||
			player_tile->Coordinate.y < 0 || player_tile->Coordinate.y > MAP_SIZE.y)
		{
			PlaySound(s_exitSoundBuffer, player_tile->Coordinate, true);
			ResetPlayerToStart();
		}

		{
			d2d::SpriteTileComponent* end_tile = s_currentMap->GetEnd().Access<d2d::SpriteTileComponent>();
			d2d::BoxPhysicsComponent* player_box = s_player.Access<d2d::BoxPhysicsComponent>();
			glm::vec2 player_min = player_tile->Coordinate + player_box->HitBoxMin * player_tile->Scale;
			glm::vec2 player_max = player_tile->Coordinate + player_box->HitBoxMax * player_tile->Scale;
			if (ddm::BoxBoxIntersect(player_min, player_max, end_tile->Coordinate - glm::vec2(1), end_tile->Coordinate + glm::vec2(2)))
			{
				s_desiredMapIndex = s_currentMap->GetIndex() + 1;
				PlaySound(s_exitSoundBuffer, player_tile->Coordinate, true);
			}
		}

		if (!s_teleporter.HasTag(ddc::Tag::Dynamic))
		{
			ReturnTeleporterToPlayer();
		}

		if (tele_tile->Coordinate.x < 0 || tele_tile->Coordinate.x > MAP_SIZE.x ||
			tele_tile->Coordinate.y < 0 || tele_tile->Coordinate.y > MAP_SIZE.y)
		{
			ReturnTeleporterToPlayer();
		}

		s_listener->setPosition(sf::Vector3(player_tile->Coordinate.x, player_tile->Coordinate.y, 0.0f));
	}

	void LuxportGame::RenderUpdate(const dd::GameUpdateData& update_data)
	{

	}

	static std::vector<ddc::EntitySpace*> s_spaces;

	const std::vector<ddc::EntitySpace*>& LuxportGame::GetEntitySpaces() const
	{
		return s_spaces;
	}
}