#pragma once

#include "ofMain.h"
#include "Node.h"

#include <cstddef>
#include <utility>
#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>

class Quad {
public:
	float x, y, w, h;
	Quad(float x, float y, float w, float h);

	bool contains(const Node& node) const;
	bool intersects(const Quad& range) const;
};

class QuadTree {
public:
	QuadTree(const Quad &boundary, int capacity);

	void insert(const Node& node);
	std::vector<std::size_t> get_nearby_nodes(const Node& curr_node, std::vector<std::unique_ptr<Node>>& nodes, const float& cell_size) const;
	void draw() const;
	void clear();

private:
	std::unique_ptr<Quad> m_boundary;
	std::unique_ptr<QuadTree> m_NE, m_SE, m_SW, m_NW; 
	int m_capacity;
	bool m_divided;
	ofVec2f avg_pos;
	std::vector<std::size_t> m_nodes;

	void subdivide();
	void draw_helper() const;
};	