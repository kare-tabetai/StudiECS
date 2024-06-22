#pragma once
#include "TypeIDGenerator.h"

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
}