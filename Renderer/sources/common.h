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
typedef char	 c8;
typedef unsigned char uc8;

typedef VkExtent2D UInt2;
typedef VkExtent3D UInt3;
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

typedef std::string string;

typedef std::chrono::high_resolution_clock ChronoTime;
typedef std::chrono::duration<float, std::chrono::seconds::period> TimeDif;
typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimeVal;

typedef VkExtent2D UInt2D;

template <typename T, s32 N> using array = std::array<T, N>;
template <typename T> using vector = std::vector<T>;
template <typename T> struct Size { T width, height, depth; };

#define F32(v)	static_cast<f32>(v)
#define S32(v)	static_cast<s32>(v)
#define U32(v)	static_cast<u32>(v)
#define SIZE_T(v)	static_cast<size_t>(v)

#define PRINT(    v1             ) std::cout << v1
#define PRINT2(   v1, v2         ) PRINT( v1         ) << " " << v2
#define PRINT3(   v1, v2, v3     ) PRINT2( v1, v2     ) << " " << v3
#define PRINT4(   v1, v2, v3, v4 ) PRINT3( v1, v2, v3 ) << " " << v4

#define PRINTLN(  v1             ) PRINT(  v1             ) << std::endl
#define PRINTLN2( v1, v2         ) PRINT2( v1, v2         ) << std::endl
#define PRINTLN3( v1, v2, v3     ) PRINT3( v1, v2, v3     ) << std::endl
#define PRINTLN4( v1, v2, v3, v4 ) PRINT4( v1, v2, v3, v4 ) << std::endl

#define LOG(v) std::cout << "LOG::" << v << std::endl
#define WAR(v) std::cout << "WAR::" << v << std::endl
#define ERR(v) std::cout << "ERR::" << v << std::endl

#define RUNTIME_ERROR(m) throw std::runtime_error(m)

#define ASSERT_FALSE(    v, m ) if(!v)                RUNTIME_ERROR(m)
#define ASSERT_ZERO(     v, m ) if(!v)                RUNTIME_ERROR(m)
#define ASSERT_NEGATIVE( v, m ) if(v<0)               RUNTIME_ERROR(m)
#define ASSERT_NULLPTR(  v, m ) if(v==nullptr)        RUNTIME_ERROR(m)
#define ASSERT_VKNULL(   v, m ) if(v==VK_NULL_HANDLE) RUNTIME_ERROR(m)
#define ASSERT_VKERROR(  v, m ) if(v!=VK_SUCCESS)     RUNTIME_ERROR(m)

#define PI 3.14159265358979323846

#define VEC4_0 { 0.f, 0.f, 0.f, 0.f }
#define VEC4_1 { 1.f, 1.f, 1.f, 1.f }
#define VEC4_BLACK { 0.f, 0.f, 0.f, 1.f }

struct Cleaner {
	std::stack<std::function<void()>> stack;

	void push( std::function<void()>&& function ) {
		stack.push( function );
	};

	void flush( string name = "" ) {
		printf( "Clean::%s::%lu process \n", name.c_str(), static_cast<u32>(stack.size()) );
		while (!stack.empty()) {
			std::function<void()> cleaning = stack.top();
			cleaning();
			stack.pop();
		}
	};
};
