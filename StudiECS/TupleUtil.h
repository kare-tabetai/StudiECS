#pragma once
#include "Concept.h"
#include <tuple>
#include <utility>

template<CdConcept... CD>
using EntityCdPtrTuple = std::tuple<Entity*,CD*...>;

template<class... T>
using TypeParameterToTuple= std::tuple<T...>;

template<class Tuple>
using TupleAtFirst = std::tuple_element<0, Tuple>::type;

template<class Tuple>
using TupleAtLast = std::tuple_element<std::tuple_size<Tuple>::value - 1, Tuple>::type;

template<class... T>
using TypeParameterAtFirst = TupleAtFirst<TypeParameterToTuple<T...>>;

template<class... T>
using TypeParameterAtLast = TupleAtLast<TypeParameterToTuple<T...>>;

class TupleUtil {
private:
    template<class Func, class Lhs, class Rhs, std::size_t... I>
    static void applyTuplesImpl(Func&& func, Lhs& lhs, Rhs& rhs, std::index_sequence<I...>)
    {
        (... ,func(std::get<I>(lhs), std::get<I>(rhs)));
    }

public:
    /// \note 二つの同じ型のtupleを引数にとって要素ごとにラムダを適用する
    template<class Func, class... Ts>
    static void ApplyTuples(Func&& func, std::tuple<Ts...>& lhs, std::tuple<Ts...>& rhs)
    {
        return applyTuplesImpl(
            std::forward<Func>(func), 
            lhs,
            rhs, 
            std::make_index_sequence<sizeof...(Ts)>()
        );
    }
};

