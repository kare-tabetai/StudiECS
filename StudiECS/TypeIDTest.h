#pragma once
#include "TypeIDGenerator.h"
#include "CdIdGenerator.h"
#include "TypeUtil.h"
#include <iostream>

void TepeIDTest()
{
    std::cout << "id"
              << "\n";
    std::cout << "int:" << TypeIDGenerator<int>::id() << "\n";
    std::cout << "int:" << TypeIDGenerator<int>::id() << "\n";
    std::cout << "float:" << TypeIDGenerator<float>::id() << "\n";
    std::cout << "std::vector<float>:" << TypeIDGenerator<std::vector<float>>::id() << "\n";
    std::cout << "std::tuple<float, int>:" << TypeIDGenerator<std::tuple<float, int>>::id() << "\n";
    std::cout << "std::tuple<float, int>:" << TypeIDGenerator<std::tuple<float, int>>::id() << "\n";
    std::cout << "std::tuple<int, float>:" << TypeIDGenerator<std::tuple<int, float>>::id() << "\n";
    std::cout << "int:" << TypeIDGenerator<int>::id() << "\n";
    std::cout << "int*:" << TypeIDGenerator<int*>::id() << "\n";
    std::cout << "int*:" << TypeIDGenerator<int*>::id() << "\n";
    std::cout << "int&:" << TypeIDGenerator<int&>::id() << "\n";
    std::cout << "int&:" << TypeIDGenerator<int&>::id() << "\n";
    std::cout << "\n";

    std::cout << "number"
              << "\n";
    std::cout << "int:" << TypeIDGenerator<int>::number() << "\n";
    std::cout << "int:" << TypeIDGenerator<int>::number() << "\n";
    std::cout << "float:" << TypeIDGenerator<float>::number() << "\n";
    std::cout << "std::vector<float>:" << TypeIDGenerator<std::vector<float>>::number() << "\n";
    std::cout << "std::tuple<float, int>:" << TypeIDGenerator<std::tuple<float, int>>::number() << "\n";
    std::cout << "std::tuple<float, int>:" << TypeIDGenerator<std::tuple<float, int>>::number() << "\n";
    std::cout << "std::tuple<int, float>:" << TypeIDGenerator<std::tuple<int, float>>::number() << "\n";
    std::cout << "int:" << TypeIDGenerator<int>::number() << "\n";
    std::cout << "int*:" << TypeIDGenerator<int*>::number() << "\n";
    std::cout << "int*:" << TypeIDGenerator<int*>::number() << "\n";
    std::cout << "int&:" << TypeIDGenerator<int&>::number() << "\n";
    std::cout << "int&:" << TypeIDGenerator<int&>::number() << "\n";

    constexpr auto int_type_list = TypeUtil::MakeTypeList<int&,int,int*>();
    constexpr auto CdId_int_type_list = CdIdGenerator<decltype(int_type_list)>::id();
    std::cout << "CdNumber_int_type_list:" << CdIdGenerator<decltype(int_type_list)>::number() << "\n";
    std::cout << "TypeID_int_type_list:" << TypeIDGenerator<decltype(int_type_list)>::number() << "\n";
}