#pragma once
#include "Entity.h"
#include "World.h"

struct MoveableClass {
    MoveableClass& operator=(const MoveableClass&) = delete;
    MoveableClass& operator=(MoveableClass&&) = default;
    int a;
};
static_assert(!std::is_copy_assignable_v<MoveableClass>);
static_assert(std::is_move_assignable_v<MoveableClass>);

struct CopyableClass {
    CopyableClass& operator=(const CopyableClass&) = default;
    CopyableClass& operator=(CopyableClass&&) = delete;
    int a;
};
static_assert(!std::is_move_assignable_v<CopyableClass>);
static_assert(std::is_copy_assignable_v<CopyableClass>);

struct TestClass0 {
    int a;
    double b;
    long long c;
    bool d;
};

struct TestClass1 {
    bool d;
    int* a;
    long long* c;
    double b;
};

struct IndexStruct {
    size_t index = 0;
};

struct Size1KB {
    std::array<std::byte, 1024> padding;
};

struct Size128B {
    std::array<std::byte, 128> padding;
};

void WorldTest()
{
    World world;
    // Entity entity = world.CreateEntity<int, float*>();
    // Entity entity = world.CreateEntity<int, float&>();
    Entity entity_FixedClass_0 = world.CreateEntity<int, MoveableClass>();
    Entity entity_TestClass0_0 = world.CreateEntity<int, TestClass0>();
    Entity entity_FixedClass_1 = world.CreateEntity<int, MoveableClass>();
    Entity entity_TestClass0_1 = world.CreateEntity<int, TestClass0>();
    Entity entity_TestClass1_0 = world.CreateEntity<TestClass1, int>();
    Entity entity_TestClass1_1 = world.CreateEntity<int, TestClass1>();
    Entity entity_TestClass1_2 = world.CreateEntity<float, int, TestClass0>();
    Entity entity_TestClass1_3 = world.CreateEntity<float, int, TestClass1>();
    Entity entity_TestClass1_4 = world.CreateEntity<float, int, double, TestClass1>();
    Entity entity_TestClass1_5 = world.CreateEntity<float, int, TestClass1>();
    auto float_ptr = world.Get<float>(entity_TestClass1_4);
    *float_ptr = 0.5f;
    auto [float_ptr_, duble_ptr] = world.GetTypes<float, double>(entity_TestClass1_4);

    auto test_class_1_array = world.GetCdArray<TestClass1>();
    auto float_array = world.GetCdArray<float>();

    //constexpr size_t kEntitySize = 2048;
    constexpr size_t kEntitySize = 24;

    std::vector<Entity> test_entities;
    for (size_t i = 0; i < kEntitySize; i++) {
        Entity index_entity = world.CreateEntity<IndexStruct, int, MoveableClass>();
        test_entities.push_back(index_entity);
        [[maybe_unused]] auto index_struct_ptr = world.Get<IndexStruct>(index_entity)->index = i;
    }

    assert(world.IsValid(test_entities.front()));
    world.DestroyEntity(test_entities.front());
    assert(!world.IsValid(test_entities.front()));

    for (auto itr = test_entities.begin(); itr != test_entities.end(); ) {
        size_t index = std::distance(test_entities.begin(), itr);
        if (index % 3 == 0) {
            world.DestroyEntity(*itr);
            itr = test_entities.erase(itr);
        } else {
            itr++;
        }
    }

    for (size_t i = 0; i < 24; i++) {
        Entity index_entity = world.CreateEntity<IndexStruct, int, MoveableClass>();
        test_entities.push_back(index_entity);
    }
 }