#pragma once

#include <vector>

using byte = unsigned char;
using ushort = unsigned short;


class BitBoard4
{
public:
	enum {
		BD_WIDTH = 4,
		BD_HEIGHT = BD_WIDTH,
		BD_SIZE = BD_WIDTH * BD_HEIGHT,
		UPPER_EDGE = BD_SIZE, LOWER_EDGE, LEFT_EDGE, RIGHT_EDGE, UT_SIZE,
		EMPTY = 0, BLACK, WHITE,
	};
public:
	BitBoard4() {
		init();
	}
	BitBoard4(const BitBoard4& x)
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
	bool	did_black_win_uf() const;
	bool	did_white_win_uf() const;

	bool	playout_to_end(bool black_next = true) const;	//	return: 黒の勝ちか？
	double	playout_to_end(int N, bool black_next = true) const;	//	return: 黒の勝ち確率を返す
	bool	playout_smart(bool black_next = true, bool = false) const;	//	return: 黒の勝ちか？
	double	playout_smart(int N, bool black_next = true) const;	//	return: 黒の勝ち確率を返す
	//bool	playout_smart_uf(bool black_next = true, bool = false) const;	//	return: 黒の勝ちか？
	double	PMC_score(int N, bool black_next = true, bool = false) const;	//	return: 純粋MC法により黒の平均スコアを返す
public:
	ushort	m_black;
	ushort	m_white;
	std::vector<short>	m_uf_parent;		//	セル ix の gid
};

