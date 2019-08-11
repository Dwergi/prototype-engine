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

#pragma optimize("", off)

namespace d2d
{
	PhysicsSystem::PhysicsSystem() :
		ddc::System( "Physics 2D" )
	{
		DD_TODO("Need static boxes/circles");

		RequireTag(ddc::Tag::Visible, "circles");
		RequireWrite<d2d::CirclePhysicsComponent>("circles");
		RequireWrite<d2d::Transform2DComponent>("circles");

		RequireTag(ddc::Tag::Visible, "boxes");
		RequireWrite<d2d::BoxPhysicsComponent>("boxes");
		RequireWrite<d2d::Transform2DComponent>("boxes");
	}

	static const glm::vec2 GRAVITY(0, 10);

	struct CollisionObject
	{
		CollisionObject() {}
		CollisionObject(d2d::Physics2DBase& physics, d2d::Transform2DComponent& transform) :
			Physics(&physics), Transform(&transform) {}

		d2d::Physics2DBase* Physics { nullptr };
		d2d::Transform2DComponent* Transform { nullptr };
	};

	struct CollisionManifold
	{
		CollisionManifold() {};
		CollisionManifold(CollisionManifold&& other) noexcept :
			A(*other.A.Physics, *other.A.Transform),
			B(*other.B.Physics, *other.B.Transform),
			Normal(other.Normal),
			Penetration(other.Penetration)
		{
		}

		CollisionObject A;
		CollisionObject B;

		glm::vec2 Normal { 0, 0 };
		float Penetration { 0 };
	};

	static void PositionalCorrection(const CollisionManifold& manifold)
	{
		const float CORRECTION_PERCENT = 0.2f;

		glm::vec2 correction = (manifold.Penetration / (manifold.A.Physics->InverseMass + manifold.B.Physics->InverseMass)) * manifold.Normal * CORRECTION_PERCENT;
		manifold.A.Transform->Position -= correction * manifold.A.Physics->InverseMass;
		manifold.A.Transform->Update();

		manifold.B.Transform->Position -= correction * manifold.A.Physics->InverseMass;
		manifold.B.Transform->Update();
	}

	static void ResolveCollision(const CollisionManifold& manifold)
	{
		glm::vec2 rel_velocity = manifold.B.Physics->Velocity - manifold.A.Physics->Velocity;

		float velocity_in_normal = glm::dot(rel_velocity, manifold.Normal);

		if (velocity_in_normal > 0)
		{
			return;
		}

		const float elasticity = ddm::min(manifold.A.Physics->Elasticity, manifold.B.Physics->Elasticity);

		// Calculate impulse scalar
		float j = (-(1 + elasticity) * velocity_in_normal) / (manifold.A.Physics->InverseMass + manifold.B.Physics->InverseMass);

		// Apply impulse
		glm::vec2 impulse = j * manifold.Normal;
		manifold.A.Physics->Velocity -= manifold.A.Physics->InverseMass * impulse;
		manifold.B.Physics->Velocity += manifold.B.Physics->InverseMass * impulse;

		PositionalCorrection(manifold);
	}
	
	void PhysicsSystem::Update(const ddc::UpdateData& update_data)
	{
		float delta_t = update_data.Delta();
		glm::vec2 accel_change = GRAVITY * delta_t;

		auto circles = update_data.Data("circles");
		auto circle_entities = circles.Entities();
		auto circle_physics = circles.Write<d2d::CirclePhysicsComponent>();
		auto circle_transforms = circles.Write<d2d::Transform2DComponent>();

		auto boxes = update_data.Data("boxes");
		auto box_entities = boxes.Entities();
		auto box_physics = boxes.Write<d2d::BoxPhysicsComponent>();
		auto box_transforms = boxes.Write<d2d::Transform2DComponent>();

		// update positions
		for (int i = 0; i < circles.Size(); ++i)
		{
			if (circle_entities[i].HasTag(ddc::Tag::Static))
			{
				DD_TODO("Hack, filter in UpdateData.");
				continue;
			}

			circle_physics[i].Velocity += accel_change;

			glm::vec2 displacement = circle_physics[i].Velocity * delta_t;
			circle_transforms[i].Position += displacement;
			circle_transforms[i].Update();
		}

		for (int i = 0; i < boxes.Size(); ++i)
		{
			if (box_entities[i].HasTag(ddc::Tag::Static))
			{
				DD_TODO("Hack, filter in UpdateData.");
				continue;
			}

			box_physics[i].Velocity += accel_change;

			glm::vec2 displacement = box_physics[i].Velocity * delta_t;
			box_transforms[i].Position += displacement;
			box_transforms[i].Update();
		}

		// find collision manifolds
		std::vector<CollisionManifold> collisions;

		for (int a = 0; a < circles.Size(); ++a)
		{
			d2d::CirclePhysicsComponent& a_physics = circle_physics[a];
			d2d::Transform2DComponent& a_transform = circle_transforms[a];

			for (int b = 0; b < circles.Size(); ++b)
			{
				if (a == b)
				{
					continue;
				}

				d2d::CirclePhysicsComponent& b_physics = circle_physics[b];
				d2d::Transform2DComponent& b_transform = circle_transforms[a];

				if (a_physics.HitCircle.Intersects(b_physics.HitCircle))
				{
					glm::vec2 diff = b_transform.Position - a_transform.Position;
					float distance = glm::length(diff);

					DD_ASSERT(distance != 0);

					CollisionManifold& manifold = collisions.emplace_back();
					manifold.A = CollisionObject(a_physics, a_transform);
					manifold.B = CollisionObject(b_physics, b_transform);
					manifold.Penetration = (a_physics.HitCircle.Radius + b_physics.HitCircle.Radius) - distance;
					manifold.Normal = diff / distance;
				}
			}

			for (int b = 0; b < boxes.Size(); ++b)
			{
				d2d::BoxPhysicsComponent& b_physics = box_physics[b];
				d2d::Transform2DComponent& b_transform = box_transforms[b];

				glm::vec2 b_half_extents = b_physics.HitBox.HalfExtents();

				glm::vec2 diff = b_transform.Position - a_transform.Position;
				glm::vec2 nearest = b_physics.HitBox.NearestPoint(a_physics.HitCircle.Centre);

				bool inside = false;

				if (diff == nearest)
				{
					// inside AABB
					inside = true;
					
					if (std::abs(diff.x) > std::abs(diff.y))
					{
						if (diff.x > 0)
						{
							nearest.x = b_half_extents.x;
						}
						else
						{
							nearest.x = -b_half_extents.x;
						}
					}
					else
					{
						if (diff.y > 0)
						{
							nearest.y = b_half_extents.y;
						}
						else
						{
							nearest.y = -b_half_extents.y;
						}
					}
				}

				glm::vec2 normal = diff - nearest;
				float distance = glm::length2(normal);

				if (distance > a_physics.HitCircle.Radius * a_physics.HitCircle.Radius)
				{
					continue;
				}

				distance = std::sqrtf(distance);

				CollisionManifold& manifold = collisions.emplace_back();
				manifold.A = CollisionObject(a_physics, a_transform);
				manifold.B = CollisionObject(b_physics, b_transform);
				manifold.Penetration = distance - a_physics.HitCircle.Radius;
				manifold.Normal = inside ? -(normal / distance) : (normal / distance);
			}
		}

		for (int a = 0; a < boxes.Size(); ++a)
		{
			d2d::BoxPhysicsComponent& a_physics = box_physics[a];
			d2d::Transform2DComponent& a_transform = box_transforms[a];

			glm::vec2 a_half_extents = a_physics.HitBox.HalfExtents();

			for (int b = 0; b < boxes.Size(); ++b)
			{
				if (a == b)
				{
					continue;
				}

				d2d::BoxPhysicsComponent& b_physics = box_physics[b];
				d2d::Transform2DComponent& b_transform = box_transforms[b];

				glm::vec2 b_half_extents = b_physics.HitBox.HalfExtents();

				glm::vec2 diff = b_transform.Position - a_transform.Position;
				glm::vec2 overlap = a_half_extents + b_half_extents - glm::abs(diff);

				if (overlap.x > 0 || overlap.y > 0)
				{
					CollisionManifold& manifold = collisions.emplace_back();
					manifold.A = CollisionObject(a_physics, a_transform);
					manifold.B = CollisionObject(b_physics, b_transform);
					manifold.Penetration = ddm::min(overlap.x, overlap.y);
					manifold.Normal = ddm::AABB2D::NearestNormal(diff);
				}
			}
		}

		for (CollisionManifold& manifold : collisions)
		{
			//ResolveCollision(manifold);
		}
	}

	void PhysicsSystem::DrawDebugInternal()
	{
		ImGui::Value("Collision Count", m_collisionCount);
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