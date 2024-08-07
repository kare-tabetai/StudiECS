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

void AddCDTest(World& world) {
    auto add_test_entity = world.CreateEntity<int>();
    auto* int_add_test_entity = world.Get<int>(add_test_entity);
    assert(int_add_test_entity);
    *int_add_test_entity = 5;

    world.AddCD<float>(add_test_entity);
    auto* flt_add_test_entity = world.Get<float>(add_test_entity);
    assert(flt_add_test_entity);
    *flt_add_test_entity = 5.5;

    world.AddCD<double>(add_test_entity);
    auto* dbl_add_test_entity = world.Get<double>(add_test_entity);
    assert(dbl_add_test_entity);
    *dbl_add_test_entity = 5.5;

    world.AddCD<Size1KB>(add_test_entity);
    auto* size_1kb_add_test_entity = world.Get<Size1KB>(add_test_entity);
    assert(size_1kb_add_test_entity);

    world.DestroyEntity(add_test_entity);
}

void AddRemoveCDTest(World& world)
{
    auto test_entity = world.CreateEntity<int>();
    auto* int_cd = world.Get<int>(test_entity);
    assert(int_cd);
    *int_cd = 5;

    world.AddCD<double>(test_entity);
    auto* dbl_cd = world.Get<double>(test_entity);
    assert(dbl_cd);
    *dbl_cd = 5.5;

    world.RemoveCD<int>(test_entity);
    int_cd = world.Get<int>(test_entity);
    assert(!int_cd);

    world.AddCD<int>(test_entity);
    int_cd = world.Get<int>(test_entity);
    assert(int_cd);

    world.RemoveCD<double>(test_entity);
    dbl_cd = world.Get<double>(test_entity);
    assert(!dbl_cd);

    world.DestroyEntity(test_entity);
}

void WorldTest()
{
    std::vector<Entity> entities;
    World world;
    entities.emplace_back(world.CreateEntity<int, float, MoveableClass>());
    entities.emplace_back(world.CreateEntity<int, TestClass0>());
    entities.emplace_back(world.CreateEntity<int, MoveableClass>());
    entities.emplace_back(world.CreateEntity<int, TestClass0>());
    entities.emplace_back(world.CreateEntity<TestClass1, int>());
    entities.emplace_back(world.CreateEntity<int, TestClass1>());
    entities.emplace_back(world.CreateEntity<float, int, TestClass0>());
    entities.emplace_back(world.CreateEntity<float, int, TestClass1>());
    entities.emplace_back(world.CreateEntity<float, int, double, TestClass1>());
    entities.emplace_back(world.CreateEntity<float, int, TestClass1>());

    auto float_ptr = world.Get<float>(entities.front());
    *float_ptr = 4.8f;
    auto [_float_ptr_, _int_ptr] = world.GetTypes<float, int>(entities.front());
    std::cout << *float_ptr << "\n";

    auto int_arrays = world.GetCdArray<int>();
    for (auto int_array : int_arrays) {
        for (auto& int_ref : int_array) {
            int_ref = -35;
        }
    }

    auto float_arrays = world.GetCdArray<float>();
    for (auto float_array : float_arrays) {
        for (auto& float_ref : float_array) {
            float_ref = -5.555f;
        }
    }

    constexpr size_t kEntitySize = 2048;
    std::vector<Entity> iim_entities;
    for (size_t i = 0; i < kEntitySize; i++) {
        Entity index_entity = world.CreateEntity<IndexStruct, int, MoveableClass>();
        iim_entities.push_back(index_entity);
        [[maybe_unused]] auto index_struct_ptr = world.Get<IndexStruct>(index_entity)->index = i;
    }

    assert(world.IsValid(iim_entities.front()));
    world.DestroyEntity(iim_entities.front());
    assert(!world.IsValid(iim_entities.front()));
    world.DestroyEntity(iim_entities.front());

    if (!iim_entities.empty()) {
        auto index = iim_entities.size() / 2;
        auto destroy_entiy = iim_entities[index];
        world.DestroyEntity(destroy_entiy);
        iim_entities.erase(iim_entities.begin() + index);
    }
    if (!iim_entities.empty()) {
        auto index = iim_entities.size() / 3;
        world.DestroyEntity(iim_entities[index]);
        iim_entities.erase(iim_entities.begin() + index);
    }

    for (auto itr = iim_entities.begin(); itr != iim_entities.end();) {
        world.DestroyEntity(*itr);
        itr = iim_entities.erase(itr);
    }

    for (size_t i = 0; i < 24; i++) {
        Entity index_entity = world.CreateEntity<IndexStruct, int, MoveableClass>();
        iim_entities.push_back(index_entity);
    }

    for (auto itr = entities.begin(); itr != entities.end();) {
        world.DestroyEntity(*itr);
        itr = entities.erase(itr);
    }

    for (auto itr = iim_entities.begin(); itr != iim_entities.end();) {
        world.DestroyEntity(*itr);
        itr = iim_entities.erase(itr);
    }

    AddCDTest(world);
    AddCDTest(world);
    
}
