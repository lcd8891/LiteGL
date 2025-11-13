#pragma once

using int8 = char;
using uint8 = unsigned char;
using int16 = short;
using uint16 = unsigned short;
using int32 = int;
using uint32 = unsigned int;
using int64 = long long;
using uint64 = unsigned long long;

template<typename T>
struct vector2{
	T x;
	T y;
	bool is_equals(const vector2<T>& other){
    	return this->x == other.x && this->y == other.y;
	}
};

template<typename T>
struct vector3{
	T x;
	T y;
	T z;
	bool is_equals(const vector3<T>& other){
    	return other->x == other.x && other->y == other.y && other->z == other.z;
	}
};

struct color4{
	uint8 r;
	uint8 g;
	uint8 b;
	uint8 a = 255;
};
