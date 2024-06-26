/*!
@file
Defines `boost::hana::m_id`.

Copyright Louis Dionne 2013-2022
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_FUNCTIONAL_ID_HPP
#define BOOST_HANA_FUNCTIONAL_ID_HPP

#include <boost/hana/config.hpp>


namespace boost { namespace hana {
    //! @ingroup group-functional
    //! The identity function -- returns its argument unchanged.
    //!
    //! ### Example
    //! @include example/functional/m_id.cpp
#ifdef BOOST_HANA_DOXYGEN_INVOKED
    constexpr auto id = [](auto&& x) -> decltype(auto) {
        return forwarded(x);
    };
#else
    struct id_t {
        template <typename T>
        constexpr T operator()(T&& t) const {
            return static_cast<T&&>(t);
        }
    };

    BOOST_HANA_INLINE_VARIABLE constexpr id_t m_id{};
#endif
}} // end namespace boost::hana

#endif // !BOOST_HANA_FUNCTIONAL_ID_HPP
