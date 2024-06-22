#include "TypeIDGenerator.h"
#include <boost/hana/equal.hpp>
#include <boost/hana/remove_if.hpp>
#include <boost/hana/traits.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana.hpp>

namespace hana = boost::hana;

// 型のシーケンス
auto Types = hana::tuple_t<int, double, void,int, char, void>;

// void 型を取り除く
auto NoVoid = hana::remove_if(Types, [](auto t) {
    return hana::traits::is_void(t);
}); // -> hana::tuple_t<int, char, double>

// ポインタに変換
auto Ptrs = hana::transform(Types, [](auto t) {
    return hana::traits::add_pointer(t);
}); // -> hana::tuple_t<int*, void*, char*, void*, double*>


auto sort_func = [](auto a, auto b) {
    using TypeA =decltype(a);
    using TypeB =decltype(b);
    constexpr uint32 a_val = TypeIDGenerator<TypeA>::id();
    constexpr uint32 b_val = TypeIDGenerator<TypeB>::id();
    return hana::size_c<a_val> < hana::size_c<b_val>;
};
auto sorted_types = hana::sort(Types, sort_func);

