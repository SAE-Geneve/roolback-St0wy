#include <cmath>

#include <engine/entity.hpp>

#include <gtest/gtest.h>

#include "engine/component.hpp"

TEST(Entity, CreateEntity)
{
	core::EntityManager entityManager;
	const auto newEntity = entityManager.CreateEntity();
	EXPECT_TRUE(entityManager.EntityExists(newEntity));
	EXPECT_TRUE(entityManager.HasComponent(
		newEntity,
		static_cast<core::EntityMask>(core::ComponentType::Empty)));

	entityManager.DestroyEntity(newEntity);
	EXPECT_FALSE(entityManager.EntityExists(newEntity));
	EXPECT_FALSE(entityManager.HasComponent(
		newEntity,
		static_cast<core::EntityMask>(core::ComponentType::Empty)));
}

TEST(Entity, EntityComponent)
{
	static constexpr core::Component newComponent = 2u;
	static constexpr core::Component newComponent2 = 4u;
	core::EntityManager entityManager;
	const auto newEntity = entityManager.CreateEntity();

	entityManager.AddComponent(newEntity, newComponent);
	EXPECT_TRUE(entityManager.HasComponent(newEntity, newComponent));
	EXPECT_FALSE(entityManager.HasComponent(newEntity, newComponent2));

	entityManager.AddComponent(newEntity, newComponent2);
	EXPECT_TRUE(entityManager.HasComponent(newEntity, newComponent));
	EXPECT_TRUE(entityManager.HasComponent(newEntity, newComponent2));

	entityManager.RemoveComponent(newEntity, newComponent);
	EXPECT_FALSE(entityManager.HasComponent(newEntity, newComponent));
	EXPECT_TRUE(entityManager.HasComponent(newEntity, newComponent2));

	entityManager.DestroyEntity(newEntity);
	EXPECT_FALSE(entityManager.HasComponent(newEntity, newComponent));
	EXPECT_FALSE(entityManager.HasComponent(newEntity, newComponent2));
}
