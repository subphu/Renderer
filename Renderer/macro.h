#pragma once

#include <include.h>

typedef	int8_t	 s8;
typedef	int16_t	 s16;
typedef	int32_t	 s32;
typedef	int64_t	 s64;
typedef	uint8_t	 u8;
typedef	uint16_t u16;
typedef	uint32_t u32;
typedef	uint64_t u64;
typedef	float	 f32;
typedef	double	 d64;

typedef VkExtent2D UInt2;
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

typedef std::string string;

template <typename T, s32 N>
using array = std::array<T, N>;
template <typename T>
using vector = std::vector<T>;

#define U32(v)	static_cast<u32>(v)
#define F32(v)	static_cast<f32>(v)

#define PRINT1(   v1             ) std::cout << v1
#define PRINT2(   v1, v2         ) PRINT1( v1         ) << " " << v2
#define PRINT3(   v1, v2, v3     ) PRINT2( v1, v2     ) << " " << v3
#define PRINT4(   v1, v2, v3, v4 ) PRINT3( v1, v2, v3 ) << " " << v4
									    
#define PRINTLN1( v1             ) PRINT1( v1             ) << std::endl
#define PRINTLN2( v1, v2         ) PRINT2( v1, v2         ) << std::endl
#define PRINTLN3( v1, v2, v3     ) PRINT3( v1, v2, v3     ) << std::endl
#define PRINTLN4( v1, v2, v3, v4 ) PRINT4( v1, v2, v3, v4 ) << std::endl

#define LOG(v) std::cout << "LOG::" << v << std::endl
#define ERR(v) std::cout << "ERR::" << v << std::endl

#define RUNTIME_ERROR(m) throw std::runtime_error(m)

#define ERR_IF_FALSE(    v, m ) if(!v)                RUNTIME_ERROR(m)
#define ERR_IF_ZERO(     v, m ) if(!v)                RUNTIME_ERROR(m)
#define ERR_IF_NEGATIVE( v, m ) if(v<0)               RUNTIME_ERROR(m)
#define ERR_IF_NULLPTR(  v, m ) if(v==nullptr)        RUNTIME_ERROR(m)
#define ERR_IF_VKNULL(   v, m ) if(v==VK_NULL_HANDLE) RUNTIME_ERROR(m)
#define ERR_IF_VKERROR(  v, m ) if(v!=VK_SUCCESS)     RUNTIME_ERROR(m)

#define PI 3.14159265358979323846

#define VEC4_0 { 0.f, 0.f, 0.f, 0.f }
#define VEC4_1 { 1.f, 1.f, 1.f, 1.f }
