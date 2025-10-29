#pragma once

#include <vector>
#include <string>

enum {
	EMPTY = 0, BLACK, WHITE,
};

using Color = unsigned char;

struct Node {
	Color	m_color = EMPTY;
	//std::vector<short>	m_is_connected;		//	0 for îÒòAåã
	std::vector<short>	m_network_black;		//	0 for îÒòAåã
	std::vector<short>	m_network_white;		//	0 for îÒòAåã
};

class HNetBoard
{
public:
	HNetBoard(int width);

public:
	int		xyToIX(int x, int y) const { return x + y*m_width; }
	std::string ixToCoord(int ix) const;
	void	init();
	void	print() const;
	void	print_dist() const;
	void	set_color(int x, int y, Color col) { m_node[xyToIX(x, y)].m_color = col; }
	void	set_color(int ix, Color col) { m_node[ix].m_color = col; }
	void	update_network(int x, int y, Color col) { update_network(xyToIX(x, y), col); }
	void	update_network(int ix, Color col);
	void	set_color_update(int x, int y, Color col) {
				set_color(x, y, col);
				update_network(x, y, col);
			}
	void	set_color_update(int ix, Color col) {
				set_color(ix, col);
				update_network(ix, col);
			}
	int		calc_vert_distance() const;

private:
	const int	m_width;
	const int	TOP_NODE_IX;
	const int	BOTTOM_NODE_IX;
	const int	LEFT_NODE_IX;
	const int	RIGHT_NODE_IX;
	std::vector<Node>	m_node;
	mutable std::vector<short>	m_dist;					//	è„ï” or ç∂ï”Ç©ÇÁÇÃãóó£
};

