#pragma once
#include "ArrayView.h"
#include "Type.h"
#include <cassert >
#include <optional>
#include <vector>

template<class T>
class SparseSet {
public:
    T& operator[](uint32 sparse_index)
    {
        if (sparse_index < sparse.size()) {
            if (sparse[sparse_index].has_value()) {
                return dense[sparse[sparse_index].value()];
            } else {
                return add(sparse_index);

            }
        } else {
            sparse.resize(sparse_index + 1);
            return add(sparse_index);
        }
    }

    const T& operator[](uint32 sparse_index) const
    {
        assert(sparse_index < sparse.size());
        assert(sparse[sparse_index].has_value());
        return dense[sparse[sparse_index].value()];
    }

    void Erase(uint32 sparse_index)
    {
        assert(sparse_index < sparse.size());
        assert(sparse[sparse_index].has_value());

        uint32 dense_index = sparse[sparse_index].value();
        sparse[sparse_index] = std::nullopt;
        dense.erase(dense.begin()+dense_index);

        for (auto& item : sparse) {
            if (item.has_value() && dense_index <=item.value()) {
                item.value()--;
            }
        }
    }

    void Clear() {
        sparse.clear();
        dense.clear();
    }

    ArrayView<T> GetArray()
    {
        return ArrayView<T>(dense.data(), dense.size());
    }

private:
    T& add(uint32 sparse_index)
    {
        assert(!sparse[sparse_index].has_value());

        dense.push_back(T());
        sparse[sparse_index] = dense.size() - 1;
        return dense.back();
    }

    std::vector<std::optional<uint32>> sparse;
    std::vector<T> dense;
};