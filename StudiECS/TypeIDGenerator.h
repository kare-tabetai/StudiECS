#pragma once
#include "MurMur3.h"
#include "Type.h"
#include <functional>
#include <string>
#include <regex>

struct TypeIDCounter {
    static inline uint32 counter = 0;
};

// https://skypjack.github.io/2020-03-14-ecs-baf-part-8/
template<class Type>
struct TypeIDGenerator {

    static consteval uint32 id()
    {
        // MEMO: hana::type_c<T>.hash()‚Íconstexpr‘Î‰ž‚µ‚Ä‚¢‚È‚¢‚½‚ßŽg‚¦‚È‚©‚Á‚½
        return murmur3::to_u32hash(__FUNCSIG__, std::size(__FUNCSIG__));
    }

    static uint32 number()
    {
        static const uint32 number = TypeIDCounter::counter;
        TypeIDCounter::counter++;
        return number;
    }

#if _DEBUG
    static std::string type_name() {
        const std::string func_sig_str = __FUNCSIG__;
        std::regex pattern(R"(TypeIDGenerator<([^>]+)>::type_name\(void\))");
        std::smatch match;

        if (std::regex_search(func_sig_str, match, pattern)) {
            return match[1];
        } else {
            return "";
        }
    }
#endif // DEBUG

};
