/*!
@file
Defines `boost::hana::duplicate`.

Copyright Louis Dionne 2013-2022
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_DUPLICATE_HPP
#define BOOST_HANA_DUPLICATE_HPP

#include <boost/hana/fwd/duplicate.hpp>

#include <boost/hana/concept/comonad.hpp>
#include <boost/hana/config.hpp>
#include <boost/hana/core/dispatch.hpp>
#include <boost/hana/extend.hpp>
#include <boost/hana/functional/id.hpp>


namespace boost { namespace hana {
    //! @cond
    template <typename W_>
    constexpr decltype(auto) duplicate_t::operator()(W_&& w) const {
        using W = typename hana::tag_of<W_>::type;
        using Duplicate = BOOST_HANA_DISPATCH_IF(duplicate_impl<W>,
            hana::Comonad<W>::value
        );

    #ifndef BOOST_HANA_CONFIG_DISABLE_CONCEPT_CHECKS
        static_assert(hana::Comonad<W>::value,
        "hana::duplicate(w) requires 'w' to be a Comonad");
    #endif

        return Duplicate::apply(static_cast<W_&&>(w));
    }
    //! @endcond

    template <typename W, bool condition>
    struct duplicate_impl<W, when<condition>> : default_ {
        template <typename X>
        static constexpr decltype(auto) apply(X&& x)
        { return hana::extend(static_cast<X&&>(x), hana::m_id); }
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_DUPLICATE_HPP
