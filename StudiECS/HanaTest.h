#pragma once
#include <type_traits>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/remove_if.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/equal.hpp>

namespace mpl = boost::mpl;

// �^�̃V�[�P���X
using Types = mpl::vector<int, void, char, void, double>;

// void �^����菜��
using NoVoid = mpl::remove_if<Types, std::is_void<mpl::_1>>::type;
// -> mpl::vector<int, char, double>

static_assert(boost::mpl::equal<NoVoid, mpl::vector<int, char, double>>::value);

// �|�C���^�ɕϊ�
using Ptrs = mpl::transform<Types, std::add_pointer<mpl::_1>>::type;
// -> mpl::vector<int*, void*, char*, void*, double*>

static_assert(boost::mpl::equal<Ptrs, mpl::vector<int*, void*, char*, void*, double*>>::value);
