#pragma once
#include "TypeInfo.h"
#include <array>
#include <memory>
#include <iostream>

class TypeInfoTestClass {
public:
    TypeInfoTestClass() {
        std::cout << "DefaultConstruct:"
                  << "\n";
        val = 53;
    }
    TypeInfoTestClass(int _val)
    {
        std::cout << "ValueConstruct:" << _val
                  << "\n";
        val = _val;
    }
    ~TypeInfoTestClass()
    {
        std::cout << "Destruct:" << val
                  << "\n";
        val = -1;
    }
    TypeInfoTestClass(const TypeInfoTestClass& source)
    {
        std::cout << "CopyConstruct:" << val << "," << source.val
                  << "\n";
        val = source.val;
    }
    TypeInfoTestClass(TypeInfoTestClass&& source) noexcept
    {
        std::cout << "MoveConstruct:" << val << "," << source.val
                  << "\n";
        val = source.val;
        source.val = -2;
    }
    TypeInfoTestClass& operator=(const TypeInfoTestClass& source)
    {
        std::cout << "CopyAsign:" << val << "," << source.val
                  << "\n";
        val = source.val;
        return (*this);
    }
    TypeInfoTestClass& operator=(TypeInfoTestClass&& source)noexcept
    {
        std::cout << "MoveAsign:" << val << "," << source.val
                  << "\n";
        val = source.val;
        source.val = -2;
        return (*this);
    }

    int val;
};

template<class T>
void TypeCopyTest(const T& cons = T(), const T& asign = T())
{
    const TypeInfo int_info = TypeInfo::Make<T>();
    std::array<std::byte, 1024> strage = {};

    int_info.Construct(&strage);
    [[maybe_unused]] T& constructed = *(T*)(&strage);
    int_info.Destruct(&strage);

    int_info.CopyConstruct(&cons, &strage);
    [[maybe_unused]] T& _constructed = *(T*)(&strage);

    int_info.CopyAsign(&asign, &strage);
    [[maybe_unused]] T& asigned = *(T*)(&strage);
    int_info.Destruct(&strage);
}

template<class T>
void TypeMoveTest(T&& cons = T(), T&& asign = T())
{
    const TypeInfo int_info = TypeInfo::Make<T>();
    std::array<std::byte, 1024> strage = {};

    int_info.Construct(&strage);
    [[maybe_unused]] T& constructed = *(T*)(&strage);
    int_info.Destruct(&strage);

    int_info.MoveConstruct(&cons, &strage);
    [[maybe_unused]] T& _constructed = *(T*)(&strage);

    int_info.MoveAsign(&asign, &strage);
    [[maybe_unused]] T& asigned = *(T*)(&strage);
    int_info.Destruct(&strage);
}

void TypeInfoTest()
{
    TypeCopyTest<int>(5,3);
    TypeCopyTest<std::shared_ptr<int>>(std::make_shared<int>(15), std::make_shared<int>(104));
    TypeCopyTest<TypeInfoTestClass>(TypeInfoTestClass(85), TypeInfoTestClass(3));

    TypeMoveTest<std::unique_ptr<int>>(std::make_unique<int>(15), std::make_unique<int>(104));
    TypeMoveTest<std::shared_ptr<int>>(std::make_shared<int>(15), std::make_shared<int>(104));
    TypeMoveTest<TypeInfoTestClass>(TypeInfoTestClass(85), TypeInfoTestClass(3));
}