#pragma once

using byte = unsigned char;
using ushort = unsigned short;


class BitBoard44
{
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
	ushort xyToMask(int x, int y) {		//	x, y: [0, 3]
		return 0x8000 >> (x+y*4);
	}
	void	set_black(ushort mask) { m_black |= mask; }
	void	set_white(ushort mask) { m_white |= mask; }
	void	set_black(int x, int y) { m_black |= xyToMask(x, y); }
	void	set_white(int x, int y) { m_white |= xyToMask(x, y); }
	bool	did_black_win() const;
	bool	did_white_win() const;

	bool	playout(bool black_next = true) const;	//	return: •‚ÌŸ‚¿‚©H
public:
	ushort	m_black;
	ushort	m_white;
};

