#pragma once

using byte = unsigned char;
using ushort = unsigned short;


class BitBoard44
{
public:
	enum {
		BD_WIDTH = 4,
		BD_HEIGHT = BD_WIDTH,
		EMPTY = 0, BLACK, WHITE,
	};
public:
	BitBoard44() {
		init();
	}
	BitBoard44(const BitBoard44& x)
		: m_black(x.m_black), m_white(x.m_white)
	{
	}
public:
	void	init() {
		m_black = 0;
		m_white = 0;
	}
	void	print() const;
	ushort xyToMask(int x, int y) const {		//	x, y: [0, 3]
		return 0x8000 >> (x+y*4);
	}
	byte	get_color(int x, int y) const { return get_color(xyToMask(x, y)); }
	byte	get_color(ushort mask) const;
	void	set_black(ushort mask) { m_black |= mask; }
	void	set_white(ushort mask) { m_white |= mask; }
	void	set_black(int x, int y) { m_black |= xyToMask(x, y); }
	void	set_white(int x, int y) { m_white |= xyToMask(x, y); }
	void	set_color(int x, int y, bool black)
	{
		if( black ) set_black(x, y);
		else set_white(x, y);
	}
	void	set_empty(int x, int y) { set_empty(xyToMask(x, y)); }
	void	set_empty(ushort mask) {
		m_black &= ~mask;
		m_white &= ~mask;
	}
	bool	did_black_win() const;
	bool	did_white_win() const;

	bool	playout_to_end(bool black_next = true) const;	//	return: •‚ÌŸ‚¿‚©H
	double	playout_to_end(int N, bool black_next = true) const;	//	return: •‚ÌŸ‚¿Šm—¦‚ğ•Ô‚·
public:
	ushort	m_black;
	ushort	m_white;
};

