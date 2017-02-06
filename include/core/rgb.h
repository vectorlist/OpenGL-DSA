#pragma once

#include <stdint.h>
#include <iostream>
#include <memory>

struct rgb
{
	rgb() : r(0),g(0),b(0) {}
	rgb(uint8_t r, uint8_t g, uint8_t b)
		: r(r), g(g), b(b) {}

	uint8_t r,g,b;
	friend std::ostream& operator<<(std::ostream &o, const rgb &d);
};

struct rgba
{
	rgba() : r(0), g(0), b(0), a(255) {}
	explicit rgba(int x) : r(x), g(x), b(x), a(x) {}
	rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
		: r(r), g(g), b(b), a(a) {}

	uint8_t r, g, b, a;
	friend std::ostream& operator<<(std::ostream &o, const rgba &d);
};

inline std::ostream& operator<<(std::ostream &o, const rgb &d)
{
	o << "rgb(" << int(d.r) << ", " << int(d.g) << ", " << int(d.b) << ")";
	return o;
}

inline std::ostream& operator<<(std::ostream &o, const rgba &d)
{
	o << "rgba(" << int(d.r) << ", " << int(d.g) << ", " << int(d.b)
		<< ", " << int(d.a) << ")";
	return o;
}

typedef std::unique_ptr<rgb[]> rgb_ptr;
typedef std::unique_ptr<rgba[]> rgba_ptr;