#pragma once

using byte = unsigned char;
using ushort = unsigned short;


class BitBoard44
{
public:
	BitBoard44() {
		init();
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
	void	set_black(int x, int y) { m_black |= xyToMask(x, y); }
	void	set_white(int x, int y) { m_white |= xyToMask(x, y); }
public:
	ushort	m_black;
	ushort	m_white;
};

