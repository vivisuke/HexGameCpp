#pragma once

#include <vector>

using byte = unsigned char;
using ushort = unsigned short;
using uint64 = uint64_t;
using int64 = int64_t;

class BitBoard8
{
public:
	enum {
		BD_WIDTH = 8,
		BD_HEIGHT = BD_WIDTH,
		BD_SIZE = BD_WIDTH * BD_HEIGHT,
		UPPER_EDGE = BD_SIZE, LOWER_EDGE, LEFT_EDGE, RIGHT_EDGE, UT_SIZE,
		EMPTY = 0, BLACK, WHITE,
	};
public:
	BitBoard8() {
		init();
	}
	BitBoard8(const BitBoard8& x)
		: m_black(x.m_black), m_white(x.m_white)
	{
		m_uf_parent = x.m_uf_parent;
	}
public:
	void	init() {
		m_black = 0;
		m_white = 0;
		m_uf_parent.resize(UT_SIZE);
		for(auto& x: m_uf_parent) x = -1;
	}
	void	print() const;
	uint64 xyToMask(int x, int y) const {		//	x, y: [0, 3]
		return (uint64)1 << (x+y*BD_WIDTH);
	}
	byte	get_color(int x, int y) const { return get_color(xyToMask(x, y)); }
	byte	get_color(uint64 mask) const;
	void	set_black(uint64 mask) { m_black |= mask; }
	void	set_white(uint64 mask) { m_white |= mask; }
	void	set_black(int x, int y) { m_black |= xyToMask(x, y); }
	void	set_white(int x, int y) { m_white |= xyToMask(x, y); }
	void	set_color(int x, int y, bool black)
	{
		if( black ) set_black(x, y);
		else set_white(x, y);
	}
	void	set_empty(int x, int y) { set_empty(xyToMask(x, y)); }
	void	set_empty(uint64 mask) {
		m_black &= ~mask;
		m_white &= ~mask;
	}
	void	playout(bool black_next);
public:
	uint64	m_black;
	uint64	m_white;
	std::vector<short>	m_uf_parent;		//	ƒZƒ‹ ix ‚Ì gid
};

