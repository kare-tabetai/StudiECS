#pragma once
#include "Type.h"

class TypeIDCounter {
public:
    static TypeID Generate() {
        count++;
        return count;
    }
    private:
    static inline TypeID count = 0;
};

template<class T>
class TypeIDHolder {
    static TypeID Get() {
        if (type_id == 0) {
            type_id = TypeIDCounter::Generate();
        } else {
            return type_id;
        }
    }

private:
    static inline TypeID type_id = 0;
};

