//
// Luxport.h - Game file for Luxport GMTK Game Jame 2019.
// Copyright (C) Sebastian Nordgren 
// August 3rd 2019
//

#include "PCH.h"
#include "Luxport.h"

#include "OrthoCamera.h"
#include "SpriteAnimationComponent.h"
#include "SpriteRenderer.h"
#include "Texture.h"

namespace lux
{
	static dd::Service<ddr::SpriteManager> s_spriteManager;
	static dd::Service<ddr::TextureManager> s_textureManager;

	void LuxGame::Initialize(ddc::World& world)
	{
		dd::Services::RegisterInterface<ddr::ICamera>(new ddr::OrthoCamera());

		dd::Services::Register(new ddr::SpriteRenderer());

		ddr::TextureHandle tileset_h = s_textureManager->Load("textures\\spacestation_tileset.png");
		
		std::vector<ddr::SpriteHandle> sprites = s_spriteManager->LoadSpriteSheet("space_station", tileset_h, glm::ivec2(16, 16));
	}

	void LuxGame::Shutdown(ddc::World& world)
	{

	}

	void LuxGame::Update(ddc::World& world)
	{

	} 

	void LuxGame::RenderUpdate(ddc::World& world)
	{

	}
}