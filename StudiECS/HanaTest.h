#include "TypeIDGenerator.h"
#include "TypeUtil.h"
#include <boost/hana/equal.hpp>
#include <boost/hana/remove_if.hpp>
#include <boost/hana/traits.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana.hpp>

constexpr auto types = boost::hana::tuple_t<int, double, void, int, char*, char, void, int&>;

constexpr auto sorted_types = TypeUtil::SortTypeList(types);
constexpr auto ptr_types = TypeUtil::ToPointerTuple(types);
constexpr auto sanitized_types = TypeUtil::SanitizeTuple(types);

constexpr bool is_cant_void = TypeUtil::IsCantCDType(boost::hana::type_c<void>);
constexpr bool is_cant_ptr = TypeUtil::IsCantCDType(boost::hana::type_c<int*>);
constexpr bool is_cant_ref = TypeUtil::IsCantCDType(boost::hana::type_c<int&>);

constexpr bool is_true_type = 
TypeUtil::TypeToBool(
	boost::hana::traits::is_void(boost::hana::type_c<void>)
);

constexpr bool is_false_type = TypeUtil::TypeToBool(
    boost::hana::traits::is_void(boost::hana::type_c<int>));

constexpr auto make_tuple_types = TypeUtil::MakeTuple<int, void, float, double, void>();