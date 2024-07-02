#pragma once
#include "NewEntity.h"
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

class TestClass1 {
    bool d;
    int* a;
    long long* c;
    double b;
};

void WorldTest()
{
    World world;
    // NewEntity entity = world.CreateEntity<int, float*>();
    // NewEntity entity = world.CreateEntity<int, float&>();
    // NewEntity entity = world.CreateEntity<int, Entity>();
    NewEntity entity_FixedClass_0 = world.CreateEntity<int, FixedClass>();
    NewEntity entity_TestClass0_0 = world.CreateEntity<int, TestClass0>();
    NewEntity entity_FixedClass_1 = world.CreateEntity<int, FixedClass>();
    NewEntity entity_TestClass0_1 = world.CreateEntity<int, TestClass0>();
    NewEntity entity_TestClass1_0 = world.CreateEntity<TestClass1, int>();
    NewEntity entity_TestClass1_1 = world.CreateEntity<int, TestClass1>();
    NewEntity entity_TestClass1_2 = world.CreateEntity<float, int, TestClass0>();
    NewEntity entity_TestClass1_3 = world.CreateEntity<float, int, TestClass1>();
    NewEntity entity_TestClass1_4 = world.CreateEntity<float, int, double, TestClass1>();
    NewEntity entity_TestClass1_5 = world.CreateEntity<float, int, TestClass1>();
    auto float_ptr = world.Get<float>(entity_TestClass1_4);
    *float_ptr = 0.5f;
    auto [float_ptr_, duble_ptr] = world.GetTypes<float, double>(entity_TestClass1_4);

    auto test_class_1_array = world.GetCdArray<TestClass1>();
    auto float_array = world.GetCdArray<float>();

 }