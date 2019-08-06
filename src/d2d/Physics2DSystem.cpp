#include "PCH.h"
#include "Physics2DSystem.h"

#include "d2d/BoxPhysicsComponent.h"
#include "d2d/CirclePhysicsComponent.h"
#include "d2d/SpriteTileComponent.h"
#include "d2d/SpriteTileSystem.h"

DD_TYPE_CPP(d2d::BoxPhysicsComponent);
DD_TYPE_CPP(d2d::CirclePhysicsComponent);

dd::Service<d2d::SpriteTileSystem> s_spriteTileSystem;

namespace d2d
{
	PhysicsSystem::PhysicsSystem() :
		ddc::System( "Physics 2D" )
	{
		RequireTag(ddc::Tag::Visible, "circles");
		RequireTag(ddc::Tag::Dynamic, "circles");
		RequireWrite<d2d::CirclePhysicsComponent>("circles");
		RequireWrite<d2d::SpriteTileComponent>("circles");

		RequireTag(ddc::Tag::Visible, "dynamic_boxes");
		RequireTag(ddc::Tag::Dynamic, "dynamic_boxes");
		RequireWrite<d2d::BoxPhysicsComponent>("dynamic_boxes");
		RequireWrite<d2d::SpriteTileComponent>("dynamic_boxes");

		RequireTag(ddc::Tag::Visible, "static_boxes");
		RequireTag(ddc::Tag::Static, "static_boxes");
		RequireRead<d2d::BoxPhysicsComponent>("static_boxes");
		RequireRead<d2d::SpriteTileComponent>("static_boxes");
	}

	static glm::vec2 NearestPointBox(glm::vec2 pt, glm::vec2 box_min, glm::vec2 box_max)
	{
		return glm::clamp(pt, box_min, box_max);
	}

	glm::vec2 GetBoxNormal(glm::vec2 dir)
	{
		if (std::abs(dir.x) > std::abs(dir.y))
		{
			// closer on x than y
			if (dir.x < 0) { return glm::vec2(-1, 0); }
			else { return glm::vec2(1, 0); }
		}
		else
		{
			if (dir.y < 0) { return glm::vec2(0, -1); }
			else { return glm::vec2(0, 1); }
		}
	}

	static bool BoxBoxIntersect(glm::vec2 a_min, glm::vec2 a_max, glm::vec2 b_min, glm::vec2 b_max)
	{
		return glm::all(glm::lessThanEqual(a_min, b_max)) && glm::all(glm::greaterThanEqual(a_max, b_min));
	}

	static const glm::vec2 GRAVITY(0, 10);
	static const int REST_THRESHOLD = 10;
	
	void PhysicsSystem::Update(const ddc::UpdateData& update_data)
	{
		auto static_boxes = update_data.Data("static_boxes");
		auto static_box_physics = static_boxes.Read<d2d::BoxPhysicsComponent>();
		auto static_box_tiles = static_boxes.Read<d2d::SpriteTileComponent>();

		auto circles = update_data.Data("circles");
		auto circle_physics = circles.Write<d2d::CirclePhysicsComponent>();
		auto circle_tiles = circles.Write<d2d::SpriteTileComponent>();

		float delta_t = update_data.Delta();

		for (int c = 0; c < circles.Size(); ++c)
		{
			glm::vec2 tile_pos = circle_tiles[c].Coordinate;

			glm::vec2 circle_velocity = circle_physics[c].Velocity;
			float circle_elasticity = circle_physics[c].Elasticity;
			float circle_radius = circle_physics[c].Radius;

			if (circle_physics[c].IsResting())
			{
				continue;
			}

			if (glm::length2(circle_velocity) < 0.02f)
			{
				circle_physics[c].RestingFrames++;
			}

			glm::vec2 accel_change = GRAVITY * delta_t;
			circle_velocity += accel_change;

			glm::vec2 displacement = circle_velocity * delta_t;
			tile_pos += displacement;

			glm::vec2 circle_centre = tile_pos + glm::vec2(circle_radius);
			float circle_radius2 = circle_radius * circle_radius;

			float nearest_dist = FLT_MAX;
			glm::vec2 nearest_pt;

			// find nearest collision
			for (int b = 0; b < static_boxes.Size(); ++b)
			{
				glm::vec2 box_min = static_box_tiles[b].Coordinate + static_box_physics[b].HitBoxMin * static_box_tiles[b].Scale;
				glm::vec2 box_max = static_box_tiles[b].Coordinate + static_box_physics[b].HitBoxMax * static_box_tiles[b].Scale;

				glm::vec2 box_near_pt = NearestPointBox(circle_centre, box_min, box_max);
				glm::vec2 box_separation = circle_centre - box_near_pt;
				float box_distance = glm::length2(box_separation);

				if (box_distance < circle_radius2 && box_distance < nearest_dist)
				{
					nearest_dist = box_distance;
					nearest_pt = box_near_pt;
				}
			}

			// resolve collision
			if (nearest_dist < FLT_MAX)
			{
				glm::vec2 separation = circle_centre - nearest_pt;

				// collision
				glm::vec2 normal = GetBoxNormal(separation);
				DD_ASSERT(glm::length2(normal) == 1);

				// reflect velocity
				float speed = glm::length(circle_velocity);
				glm::vec2 reflect_dir = circle_velocity / speed;
				circle_velocity = glm::reflect(reflect_dir, normal) * speed * circle_elasticity;

				// fix penetration
				glm::vec2 new_centre = nearest_pt + normal * (circle_radius * 1.001f);
				glm::vec2 penetration = circle_centre - new_centre;

				tile_pos += -penetration;

				float pen_amt = glm::length(penetration);
				float displacement_amt = glm::length(displacement);

				float remainder_pct = 1.0f - pen_amt / displacement_amt;
				DD_ASSERT(remainder_pct < 1);
				tile_pos += circle_velocity * remainder_pct * delta_t;
			}

			circle_tiles[c].Coordinate = tile_pos;
			circle_physics[c].Velocity = circle_velocity;
		}

		// TODO: BROKEN
		auto dynamic_boxes = update_data.Data("dynamic_boxes");
		auto dynamic_box_physics = dynamic_boxes.Write<d2d::BoxPhysicsComponent>();
		auto dynamic_box_tiles = dynamic_boxes.Write<d2d::SpriteTileComponent>();

		// dynamic boxes
		for (int db = 0; db < dynamic_boxes.Size(); ++db)
		{
			d2d::BoxPhysicsComponent& db_physics = dynamic_box_physics[db];
			d2d::SpriteTileComponent& db_tile = dynamic_box_tiles[db];

			glm::vec2 tile_pos = db_tile.Coordinate;

			if (db_physics.IsResting())
			{
				continue;
			}

			if (glm::length2(db_physics.Velocity) < 0.02f)
			{
				db_physics.RestingFrames++;
			}

			glm::vec2 accel_change = GRAVITY * delta_t;
			db_physics.Velocity += accel_change;

			glm::vec2 displacement = db_physics.Velocity * delta_t;
			db_tile.Coordinate += displacement;

			glm::vec2 db_min = db_tile.Coordinate + db_physics.HitBoxMin * db_tile.Scale;
			glm::vec2 db_max = db_tile.Coordinate + db_physics.HitBoxMax * db_tile.Scale;

			glm::vec2 dynamic_half_ex = (db_max - db_min) / 2.0f;
			glm::vec2 dynamic_centre = db_min + dynamic_half_ex;

			//float nearest_dist = FLT_MAX;
			//glm::vec2 nearest_pt;

			// find nearest collision
			for (int b = 0; b < static_boxes.Size(); ++b)
			{
				glm::vec2 static_box_min = static_box_tiles[b].Coordinate + static_box_physics[b].HitBoxMin * static_box_tiles[b].Scale;
				glm::vec2 static_box_max = static_box_tiles[b].Coordinate + static_box_physics[b].HitBoxMax * static_box_tiles[b].Scale;

				if (BoxBoxIntersect(db_min, db_max, static_box_min, static_box_max))
				{
					glm::vec2 static_half_ex = (static_box_max - static_box_min) / 2.0f;
					glm::vec2 static_centre = static_box_min + static_half_ex;

					glm::vec2 diff = glm::abs(dynamic_centre - static_centre) - (dynamic_half_ex, static_half_ex);
					if (diff.x < 0 && diff.y < 0)
					{
						if (diff.x > diff.y)
						{
							db_tile.Coordinate.x += diff.x;
							dynamic_centre.x += diff.x;

							glm::vec2 normal = dynamic_centre - static_centre;

							float speed = glm::length(db_physics.Velocity);
							glm::vec2 reflect_dir = db_physics.Velocity / speed;
							db_physics.Velocity = glm::reflect(reflect_dir, normal) * speed * db_physics.Elasticity;
						}
						else
						{
							db_tile.Coordinate.y += diff.y;
							dynamic_centre.y += diff.y;

							glm::vec2 normal = dynamic_centre - static_centre;

							float speed = glm::length(db_physics.Velocity);
							glm::vec2 reflect_dir = db_physics.Velocity / speed;
							db_physics.Velocity = glm::reflect(reflect_dir, normal) * speed * db_physics.Elasticity;

							db_tile.Coordinate.y -= 0.01;
						}
					}
				}
			}
		}
	}
}