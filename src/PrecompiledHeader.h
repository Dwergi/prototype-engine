#pragma once

#include <unordered_map>
#include <utility>
#include <vector>
#include <iostream>

typedef unsigned int uint;

#include "Assert.h"
#include "Utility.h"
#include "AutoList.h"
#include "TypeInfo.h"
#include "Member.h"

//#define USE_EIGEN

#ifdef USE_EIGEN
#define EIGEN_MATRIXBASE_PLUGIN "EigenExtensions.h"

#include "Eigen/Eigen"
#include "Eigen/StdVector"

typedef Eigen::Vector2f Vector2f;
typedef Eigen::Vector2d Vector2d;
typedef Eigen::Vector3f Vector3f;
typedef Eigen::Vector3d Vector3d;
typedef Eigen::Vector4f Vector4f;
typedef Eigen::Vector4d Vector4d;

typedef Eigen::Matrix3f Matrix3;
typedef Eigen::Matrix4f Matrix4;

template< typename T >
class aligned_vector
	: public std::vector< T, Eigen::aligned_allocator< T > >
{

};

#else
#include "Vector.h"
#endif
