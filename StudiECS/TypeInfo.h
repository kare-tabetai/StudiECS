#pragma once
#include <any>
#include <cstddef>
#include <functional>

class TypeInfo {

    template<class T>
    static consteval void Make()
    {
        type_size = sizeof(T);
        align_size = alignof(T);
        default_constructor = [](std::any& obj) {
            auto& casted_obj = std::any_cast<std::reference_wrapper<T>>(obj);
            casted_obj = T();
        };
        copy_constructor = [](std::any& lhs, const std::any& rhs) {
            auto& casted_lhs = std::any_cast<std::reference_wrapper<T>>(lhs);
            const auto& casted_rhs = std::any_cast<std::reference_wrapper<T>>(rhs);
            casted_lhs = casted_rhs;
        };

    }

    std::size_t type_size;
    std::size_t align_size;

    std::function<void(std::any&)> default_constructor;
    std::function<void(std::any&, const std::any&)> copy_constructor;
    std::function<void(std::any&, std::any&&)> move_constructor;
    std::function<std::any&(std::any&, const std::any&)> copy_asign;
    std::function<std::any&(std::any&, std::any&&)> move_asign;
    std::function<void(std::any&)> destructor;
};