#pragma once
#include "Entity.h"
#include "World.h"

class FixedClass {
    FixedClass& operator=(const FixedClass&) = delete;
    //FixedClass& operator=(FixedClass&&) = delete;
    int a;
};
constexpr bool kMoveConstractible = std::is_move_constructible_v<FixedClass>;

void WorldTest()
{
    World world;
    //Entity entity = world.CreateEntity<int, float*>();
    //Entity entity = world.CreateEntity<int, float&>();
    // Entity entity = world.CreateEntity<int, Entity>();
    Entity entity = world.CreateEntity<int, FixedClass>();
    //Entity entity = world.CreateEntity<int, float>();
}