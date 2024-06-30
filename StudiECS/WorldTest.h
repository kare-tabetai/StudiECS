#pragma once
#include "Entity.h"
#include "World.h"

class FixedClass {
    FixedClass& operator=(const FixedClass&) = delete;
    //FixedClass& operator=(FixedClass&&) = delete;
    int a;
};
constexpr bool kMoveConstractible = std::is_move_constructible_v<FixedClass>;

class TestClass0 {
    int a;
    double b;
    long long c;
    bool d;
};

void WorldTest()
{
    World world;
    //Entity entity = world.CreateEntity<int, float*>();
    //Entity entity = world.CreateEntity<int, float&>();
    // Entity entity = world.CreateEntity<int, Entity>();
    Entity entity_FixedClass_0 = world.CreateEntity<int, FixedClass>();
    Entity entity_TestClass0_0 = world.CreateEntity<int, TestClass0>();
    Entity entity_FixedClass_1 = world.CreateEntity<int, FixedClass>();
    Entity entity_TestClass0_1 = world.CreateEntity<int, TestClass0>();
    //Entity entity = world.CreateEntity<int, float>();

}