#include "TypeIDGenerator.h"
#include "TypeUtil.h"
#include <boost/hana.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/remove_if.hpp>
#include <boost/hana/traits.hpp>
#include <boost/hana/tuple.hpp>

constexpr auto test_types = boost::hana::tuple_t<int, double, void, int, char*, char, void, int&>;

constexpr auto sorted_types = TypeUtil::SortTypeList(test_types);
constexpr auto ptred_types = TypeUtil::ToPointerTypeList(test_types);
constexpr auto uniqued_types = TypeUtil::Unique(sorted_types);
constexpr auto removed_cant_cd_types = TypeUtil::RemoveCantCDType(test_types);
constexpr auto sanitized_types = TypeUtil::SanitizeTypeList(test_types);
constexpr auto removed_types = TypeUtil::RemoveTypes(
    test_types,
    boost::hana::tuple_t<void, int>);
constexpr auto connected_types = TypeUtil::AddTypes(
    boost::hana::tuple_t<double, void>,
    boost::hana::tuple_t<float, void, int>);

constexpr bool is_cant_void = TypeUtil::IsCantCDType(boost::hana::type_c<void>);
constexpr bool is_cant_ptr = TypeUtil::IsCantCDType(boost::hana::type_c<int*>);
constexpr bool is_cant_ref = TypeUtil::IsCantCDType(boost::hana::type_c<int&>);

constexpr bool is_true_type = TypeUtil::TypeToBool(
    boost::hana::traits::is_void(boost::hana::type_c<void>));

constexpr bool is_false_type = TypeUtil::TypeToBool(
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