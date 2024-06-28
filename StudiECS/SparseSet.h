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
        if (sparse_index < m_sparse.size()) {
            if (m_sparse[sparse_index].has_value()) {
                return m_dense[m_sparse[sparse_index].value()];
            } else {
                return add(sparse_index);
            }
        } else {
            m_sparse.resize(sparse_index + 1);
            return add(sparse_index);
        }
    }

    const T& operator[](uint32 sparse_index) const
    {
        assert(sparse_index < m_sparse.size());
        assert(m_sparse[sparse_index].has_value());
        return m_dense[m_sparse[sparse_index].value()];
    }

    bool Has(uint32 sparse_index) const
    {
        if (m_sparse.size() <= sparse_index) {
            return false;
        } else {
            return m_sparse[sparse_index].has_value();
        }
    }

    void Erase(uint32 sparse_index)
    {
        assert(sparse_index < m_sparse.size());
        assert(m_sparse[sparse_index].has_value());

        uint32 dense_index = m_sparse[sparse_index].value();
        m_sparse[sparse_index] = std::nullopt;
        m_dense.erase(m_dense.begin() + dense_index);

        for (auto& item : m_sparse) {
            if (item.has_value() && dense_index <= item.value()) {
                item.value()--;
            }
        }
    }

    void Reserve(uint32 size) {
        m_sparse.reserve();
        m_dense.reserve();
    }

    void Clear()
    {
        m_sparse.clear();
        m_dense.clear();
    }

    ArrayView<T> GetArray()
    {
        return ArrayView<T>(m_dense.data(), m_dense.size());
    }

private:
    T& add(uint32 sparse_index)
    {
        assert(!m_sparse[sparse_index].has_value());

        m_dense.push_back(T());
        m_sparse[sparse_index] = static_cast<uint32>(m_dense.size()) - 1;
        return m_dense.back();
    }

    std::vector<std::optional<uint32>> m_sparse;
    std::vector<T> m_dense;
};