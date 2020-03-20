#include <gtest/gtest.h>
#include "ecs/Component.hpp"
#include "ecs/Entity.hpp"

using namespace ecs;

struct TestComponentA : IComponentBase
{
#define DEFAULT_VALUE 42
	int Value = DEFAULT_VALUE;
};

struct AbsentComponent : IComponentBase
{
};

struct BoolComponent : IComponentBase
{
	bool Value;
};

struct EntityTest : testing::Test
{
	std::unique_ptr<IEntityBase> Entity;

	EntityTest()
	{
		Entity = std::make_unique<IEntity<TestComponentA>>();
	}

	virtual ~EntityTest() {}
};

TEST_F(EntityTest, Entity_Has_One_Component)
{
	EXPECT_TRUE(Entity->HasComponents<TestComponentA>());
}

TEST_F(EntityTest, Entity_Has_Not_One_Component)
{
	EXPECT_FALSE(Entity->HasComponents<AbsentComponent>());
}

TEST_F(EntityTest, Entity_Can_Get_Data)
{
	EXPECT_EQ(DEFAULT_VALUE, Entity->GetComponentData<TestComponentA>().Value);
}

TEST_F(EntityTest, Entity_Can_Set_Data)
{
	int constexpr testVal = 10;

	static_assert(DEFAULT_VALUE != testVal,
		"Test value must be different from original value");

	TestComponentA a;
	a.Value = 10;

	Entity->SetComponentData<TestComponentA>(a);
	EXPECT_EQ(10, Entity->GetComponentData<TestComponentA>().Value);
}

TEST_F(EntityTest, Entity_Throw_MissingComponentException)
{
	EXPECT_THROW(Entity->GetComponentData<AbsentComponent>(), ecs::IEntityBase::MissingComponentException);
}

struct MultipleComponent : testing::Test
{
	std::unique_ptr<IEntityBase> Entity;

	MultipleComponent()
	{
		Entity = std::make_unique<IEntity<TestComponentA, BoolComponent>>();
	}

	virtual ~MultipleComponent() {}
};

TEST_F(MultipleComponent, Entity_Has_Multiple_Components)
{
	EXPECT_TRUE((Entity->HasComponents<TestComponentA, BoolComponent>()));
	EXPECT_TRUE((Entity->HasComponents<BoolComponent, TestComponentA>()));
}
