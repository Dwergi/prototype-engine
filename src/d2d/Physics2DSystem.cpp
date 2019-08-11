//
// Physics2DSystem.h
// Copyright (C) Sebastian Nordgren 
// August 4th 2018
//

#include "PCH.h"
#include "Physics2DSystem.h"

#include "d2d/BoxPhysicsComponent.h"
#include "d2d/CirclePhysicsComponent.h"
#include "d2d/Transform2DComponent.h"

namespace d2d
{
	PhysicsSystem::PhysicsSystem() :
		ddc::System( "Physics 2D" )
	{
		RequireTag(ddc::Tag::Visible, "circles");
		RequireTag(ddc::Tag::Dynamic, "circles");
		RequireWrite<d2d::CirclePhysicsComponent>("circles");
		RequireWrite<d2d::Transform2DComponent>("circles");

		RequireTag(ddc::Tag::Visible, "boxes");
		RequireTag(ddc::Tag::Dynamic, "boxes");
		RequireWrite<d2d::BoxPhysicsComponent>("boxes");
		RequireWrite<d2d::Transform2DComponent>("boxes");
	}

	static const glm::vec2 GRAVITY(0, 10);

	static void ResolveCollision(d2d::Physics2DBase& a, d2d::Physics2DBase& b, glm::vec2 normal, float penetration)
	{
		glm::vec2 rel_velocity = b.Velocity - a.Velocity;

		float velocity_in_normal = glm::dot(rel_velocity, normal);

		if (velocity_in_normal > 0)
		{
			return;
		}

		const float elasticity = ddm::min(a.Elasticity, b.Elasticity);

		// Calculate impulse scalar
		float j = (-(1 + elasticity) * velocity_in_normal) / (a.InverseMass + b.InverseMass);

		// Apply impulse
		glm::vec2 impulse = j * normal;
		a.Velocity -= a.InverseMass * impulse;
		b.Velocity += b.InverseMass * impulse;
	}

	static void PositionalCorrection(glm::vec2& a_pos, float a_inv_mass, glm::vec2& b_pos, float b_inv_mass, float penetration, glm::vec2 normal)
	{
		const float CORRECTION_PERCENT = 0.2f;

		glm::vec2 correction = (penetration / (a_inv_mass + b_inv_mass)) * normal * CORRECTION_PERCENT;
		a_pos -= correction * a_inv_mass;
		b_pos += correction * b_inv_mass;
	}

	struct CollisionManifold
	{
		d2d::Physics2DBase* APhysics; 
		d2d::Transform2DComponent* ATransform;
		d2d::Physics2DBase* BPhysics;
		d2d::Transform2DComponent* BTransform;

		glm::vec2 Normal;
		float Penetration;
	};
	
	void PhysicsSystem::Update(const ddc::UpdateData& update_data)
	{
		float delta_t = update_data.Delta();

		auto circles = update_data.Data("circles");
		auto circle_physics = circles.Write<d2d::CirclePhysicsComponent>();
		auto circle_transforms = circles.Write<d2d::Transform2DComponent>();

		auto boxes = update_data.Data("static_boxes");
		auto box_physics = boxes.Write<d2d::BoxPhysicsComponent>();
		auto box_transforms = boxes.Write<d2d::Transform2DComponent>();

		std::vector<CollisionManifold> collisions;

		for (int a = 0; a < boxes.Size(); ++a)
		{
			d2d::CirclePhysicsComponent& a_physics = circle_physics[a];
			d2d::Transform2DComponent& a_transform = circle_transforms[a];

			for (int b = 0; b < boxes.Size(); ++b)
			{
				d2d::CirclePhysicsComponent& b_physics = circle_physics[b];
				d2d::Transform2DComponent& b_transform = circle_transforms[a];

				if (a_physics.HitCircle.Intersects(b_physics.HitCircle))
				{
					CollisionManifold& manifold = collisions.emplace_back();
					manifold.APhysics = &a_physics;
					manifold.ATransform = &a_transform;
					manifold.BPhysics = &b_physics;
					manifold.BTransform = &b_transform;
					 
					glm::vec2 diff = b_transform.Position - a_transform.Position;
					float distance = glm::length(diff);

					DD_ASSERT(distance != 0);

					manifold.Penetration = (a_physics.HitCircle.Radius + b_physics.HitCircle.Radius) - distance;
					manifold.Normal = diff / distance;
				}
			}
		}

		for (int b1 = 0; b1 < boxes.Size(); ++b1)
		{
			d2d::BoxPhysicsComponent& box1_physics = box_physics[b1];

			for (int b2 = 0; b2 < boxes.Size(); ++b2)
			{
				d2d::BoxPhysicsComponent& box2_physics = box_physics[b2];

				if (box1_physics.HitBox.Intersects(box2_physics.HitBox))
				{
					//ResolveCollision(box1_physics, box2_physics, )
				}
			}
		}
	}



	/*void OldPhysics(ddc::UpdateDataBuffer& circles, ddc::WriteView<d2d::SpriteTileComponent>& circle_tiles, ddc::WriteView<d2d::CirclePhysicsComponent>& circle_physics, float delta_t, ddc::UpdateDataBuffer& static_boxes, ddc::ReadView<d2d::SpriteTileComponent>& static_box_tiles, ddc::ReadView<d2d::BoxPhysicsComponent>& static_box_physics, const ddc::UpdateData& update_data)
	{
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
	}*/
}