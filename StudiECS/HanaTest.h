#pragma once
#include "TypeUtil.h"
#include "Util.h"
#include "boost_hana.h"

namespace HanaTest {

constexpr auto test_types = boost::hana::tuple_t<int, double, void, int, char*, char, void, int&>;

constexpr auto pushed_entity = Util::PushFrontEntity(test_types);

constexpr auto sorted_types = TypeUtil::SortTypeList(test_types);
constexpr auto ptred_types = TypeUtil::ToPointerTypeList(test_types);
constexpr auto uniqued_types = TypeUtil::Unique(sorted_types);
constexpr auto sanitized_types = Util::SanitizeTypeList(test_types);
constexpr auto removed_types = TypeUtil::RemoveTypes(
    test_types,
    boost::hana::tuple_t<void, int>);
constexpr auto connected_types = TypeUtil::AddTypes(
    boost::hana::tuple_t<double, void>,
    boost::hana::tuple_t<float, void, int>);



constexpr bool is_front_type_entity = TypeUtil::IsFrontType(boost::hana::type_c<Entity>, sanitized_types);

constexpr bool is_true_type = TypeUtil::HanaTypeToBool(
    boost::hana::traits::is_void(boost::hana::type_c<void>));

constexpr bool is_false_type = TypeUtil::HanaTypeToBool(
    boost::hana::traits::is_void(boost::hana::type_c<int>));

constexpr auto make_tuple_types = TypeUtil::MakeTypeList<int, void, float, double, void>();

constexpr bool has_not_any_type = TypeUtil::HasAnyTypes(
    test_types,
    boost::hana::tuple_t<bool, void*>);

constexpr auto has_any_type = TypeUtil::HasAnyTypes(
    boost::hana::tuple_t<void, int, int, void*>,
    boost::hana::tuple_t<bool, int&, int, void*>);

constexpr auto has_all_type = TypeUtil::HasAllTypes(
    boost::hana::tuple_t<void, int, int, void*>,
    boost::hana::tuple_t<int, void*>);

constexpr auto has_not_all_type = TypeUtil::HasAllTypes(
    boost::hana::tuple_t<void, int, int, void*>,
    boost::hana::tuple_t<int, void*, float>);

}
