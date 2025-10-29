#pragma once

#include <vector>

using Color = unsigned char;
using NodeIX = short;

#define	EMPTY		0
#define	BLACK		1
#define	WHITE		2

struct Node {
	std::vector<NodeIX>	m_nx_list;		//	�A����m�[�h�C���f�b�N�X�i0 org�j
	Color	m_color = EMPTY;
	bool	m_searched = false;
};

class NEBoard
{
	//enum {
	//	EMPTY = 0, BLACK, WHITE,
	//};
public:
	NEBoard(int width);
public:
	NodeIX	xyToIX(int x, int y) const { return x + y*m_width; }
	void	init_nx_list();
	void	init();
	void	print_node_edge() const;
private:
	const int		m_width;
	const NodeIX	TOP_NODE_IX;
	const NodeIX	LEFT_NODE_IX;
	const NodeIX	RIGHT_NODE_IX;
	const NodeIX	BOTTOM_NODE_IX;
	std::vector<Node> m_cell;
};

