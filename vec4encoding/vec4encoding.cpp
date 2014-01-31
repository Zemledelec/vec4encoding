// ConsoleApplication2.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <cmath>
#include <conio.h>
#include <iostream>
#include <io.h>
#include <string>

struct Vec4 {
	float x, y, z, w;
	Vec4() { x = y = z = w = 0; };
	Vec4(float x, float y, float z, float w = 0.0) : x(x), y(y), z(z), w(w) { };

	inline Vec4 yzww()const {
		return Vec4(y, z, w, w);
	};

	inline Vec4& operator = (const Vec4& a) {
		x = a.x;
		y = a.y;
		z = a.z;
		w = a.w;
		return *this;
	};

	inline Vec4 operator * (const Vec4& a) {
		return Vec4(x*a.x, y*a.y, z*a.z, w*a.w);
	};

	inline Vec4& operator -= (const Vec4& a){
		x -= a.x;
		y -= a.y;
		z -= a.z;
		w -= a.w;
		return *this;
	};
};

const Vec4 ddv(1.0, 1.0f / 255.0f, 1.0f / 65025.0f, 1.0f / 160581375.0f);
const Vec4 sddv(1.0 / 255.0, 1.0 / 255.0, 1.0 / 255.0, 0.0);
const float LOG2 = log(2);

inline float dot(const Vec4& a, const Vec4& b) {
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
};

inline float mod(float x, float y) {
	return x - y * floor(x / y);
};

inline float step(float edge, float x) {
	return x < edge ? 0.0f : 1.0f;
};

inline Vec4 step(float edge, const Vec4& x) {
	return Vec4(x.x < edge ? 0.0f : 1.0f,
		x.x < edge ? 0.0f : 1.0f,
		x.x < edge ? 0.0f : 1.0f,
		x.x < edge ? 0.0f : 1.0f);
};

inline float frac(float v) {
	return v - floor(v);
};

inline Vec4 frac(const Vec4& v) {
	return Vec4(frac(v.x), frac(v.y), frac(v.z), frac(v.w));
};

inline Vec4 mult(float scalar, const Vec4& v) {
	return Vec4(v.x*scalar, v.y*scalar, v.z*scalar, v.w*scalar);
};

inline float _log2(float x) {
	return (float)(log(x) / LOG2);
};

inline float _exp2(float x) {
	return (float)pow(2, x);
};

Vec4 EncodeFloatRGBA(float v) {
	Vec4 enc(1.0f*v, 255.0f*v, 65025.0f*v, 160581375.0f*v);
	enc = frac(enc);
	enc -= enc.yzww() * sddv;
	return enc;
};

float DecodeFloatRGBA(const Vec4& rgba) {
	return dot(rgba, ddv);
};

// s - sign
// e - exponent
// m - mantissa

//suitable for 24 bit float
Vec4 encode32(float f) {
	float F = abs(f);
	float s = step(0.0, -f);
	float e = floor(log2(F));
	float m = (exp2(-e) * F);
	e = floor(log2(F) + 127.0) + floor(log2(m));
	return Vec4(
		128.0f * s + floor(e*exp2(-1.0)),
		128.0f * mod(e, 2.0f) + mod(floor(m*128.0f), 128.0f),
		floor(mod(floor(m*exp2(23.0f - 8.0f)), exp2(8.0f))),
		floor(exp2(23.0f)*mod(m, exp2(-15.0f)))
		);
};

Vec4 encode32_2(float f) {
	float F = abs(f);
	//if (F == 0) {
	//	return Vec4();
	//}
	float s = step(0.0f, -f);
	float e = floor(log2(F));
	float m = F / exp2(e);

	//if (m < 1) {
	//	e -= 1;
	//}
	e += 127;

	Vec4 rgba;
	rgba.x = 128.0f * s + floor(e*exp2(-1.0f));
	rgba.y = 128.0f * mod(e, 2.0f) + mod(floor(m*128.0f), 128.0f);
	rgba.z = floor(mod(floor(m*exp2(23.0f - 8.0f)), exp2(8.0f)));
	rgba.w = floor(exp2(23.0f)*mod(m, exp2(-15.0f)));
	return mult(1.0f / 255.0f, rgba);
};

float decode32(const Vec4& rgba) {
	float s = 1.0f - step(128.0f, rgba.x)*2.0;
	float e = 2.0f * mod(rgba.x, 128.0f) + step(128.0f, rgba.y) - 127.0f;
	float m = mod(rgba.y, 128.0f)*65536.0f + rgba.z * 256.0f + rgba.w + float(0x800000);
	return s * exp2(e) * (m * exp2(-23.0f));
};

//better precision 32 bit
float decode32_2(const Vec4& v) {
	Vec4 rgba = mult(255.0f, v);
	float s = 1.0f - step(128.0f, rgba.x)*2.0f;
	float e = 2.0f * mod(rgba.x, 128.0f) + step(128.0f, rgba.y) - 127.0f;
	//if (e == -127)
	//	return 0;
	float m = mod(rgba.y, 128.0f)*65536.0f + rgba.z * 256.0f + rgba.w + float(0x800000);
	return s * exp2(e - 23.0f) * m;
};

int _tmain(int argc, _TCHAR* argv[])
{
	//Vec4 x = EncodeFloatRGBA(0.0);
	//float e = DecodeFloatRGBA(x);

	std::cout << "begin.";

	//float step = 0.001f;
	//Vec4 res;
	//for (float val = -0.0; val < 10000.0; val += step) {
	//	encode32(val, &res);
	//	float e = decode32(res);
	//	float eps = val - e;

	//	if ((res.x > 255 || res.y > 255 || res.z > 255 || res.w > 255) ||
	//		(res.x < 0 || res.y < 0 || res.z < 0 || res.w < 0) ||
	//		eps > 0.003 || eps < -0.003) {
	//		std::cout << "val=" << std::to_string(val) << ", (" << res.x << "," << res.y << "," << res.z << "," << res.w << "), e=" << std::to_string(e) << ", eps=" << eps << "\n";
	//	}
	//}
	Vec4 enc = encode32(1002.233f);
	float x = decode32(enc);


	Vec4 enc2 = encode32_2(1002.233f);
	float x2 = decode32_2(enc2);

	std::cout << "done.";

	_getch();
	return 0;
}