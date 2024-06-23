#pragma once
#include<memory>

class ArchetypeInfo;

struct ArchetypeBrunch {
    std::weak_ptr<ArchetypeInfo> add;
    std::weak_ptr<ArchetypeInfo> remove;
};