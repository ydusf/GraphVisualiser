#include "../headers/BarnesHut.h"
#include "../headers/Node.h"
#include "ofMain.h"

#include <cstddef>
#include <utility>
#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>

// https://en.wikipedia.org/wiki/Quadtree

Quad::Quad(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {};

bool Quad::contains(const Node& node) const {
  return (
    node.pos.x >= x - w &&
    node.pos.x <= x + w &&
    node.pos.y >= y - h &&
    node.pos.y <= y + h
  );
};

bool Quad::intersects(const Quad& range) const {
  return !(
    range.x - range.w > x + w ||
    range.x + range.w < x - w ||
    range.y - range.h > y + h ||
    range.y + range.h < y - h
  );
}

QuadTree::QuadTree(const Quad &boundary, int capacity)
  : m_boundary(std::make_unique<Quad>(boundary)),
    m_capacity(capacity),
    m_divided(false)
  {};

void QuadTree::subdivide() {
  float x = m_boundary->x;
  float y = m_boundary->y;
  float half_w = m_boundary->w * 0.5f;
  float half_h = m_boundary->h * 0.5f;

  Quad NE = Quad(x + half_w, y - half_h, half_w, half_h);
  Quad NW = Quad(x - half_w, y - half_h, half_w, half_h);
  Quad SE = Quad(x + half_w, y + half_h, half_w, half_h);
  Quad SW = Quad(x - half_w, y + half_h, half_w, half_h);

  m_NE = std::make_unique<QuadTree>(NE, m_capacity);
  m_NW = std::make_unique<QuadTree>(NW, m_capacity);
  m_SE = std::make_unique<QuadTree>(SE, m_capacity);
  m_SW = std::make_unique<QuadTree>(SW, m_capacity);

  m_divided = true;
};

void QuadTree::insert(const Node& node) {
  if(!m_boundary->contains(node)) return;

  if(m_nodes.size() < m_capacity) {
    m_nodes.push_back(node.get_id());
    avg_pos = (avg_pos * (m_nodes.size()-1) + node.pos) / m_nodes.size();
  } else {
    if(!m_divided) {
      subdivide();
    }

    m_NE->insert(node);
    m_NW->insert(node);
    m_SE->insert(node);
    m_SW->insert(node);
  };
};

std::vector<std::size_t> QuadTree::get_nearby_nodes(const Node& curr_node, std::vector<std::unique_ptr<Node>>& nodes, const float& cell_size) const {
  std::vector<std::size_t> nodes_found;

  const float half_cell_size = cell_size * 0.5f;

  const Quad range(curr_node.pos.x - half_cell_size, curr_node.pos.y - half_cell_size, cell_size, cell_size);

  if(!m_boundary->intersects(range)) return nodes_found;
  
  for(const std::size_t& node_id : m_nodes) {
    if(!range.contains(*nodes[node_id])) continue;
    nodes_found.push_back(node_id);
  };

  if(!m_divided) return nodes_found;

  auto nearby_nodes = m_NE->get_nearby_nodes(curr_node, nodes, cell_size);
  nodes_found.insert(nodes_found.end(), nearby_nodes.begin(), nearby_nodes.end());

  nearby_nodes = m_NW->get_nearby_nodes(curr_node, nodes, cell_size);
  nodes_found.insert(nodes_found.end(), nearby_nodes.begin(), nearby_nodes.end());

  nearby_nodes = m_SE->get_nearby_nodes(curr_node, nodes, cell_size);
  nodes_found.insert(nodes_found.end(), nearby_nodes.begin(), nearby_nodes.end());

  nearby_nodes = m_SW->get_nearby_nodes(curr_node, nodes, cell_size);
  nodes_found.insert(nodes_found.end(), nearby_nodes.begin(), nearby_nodes.end());

  return nodes_found;
  
};

void QuadTree::clear() {
  m_nodes.clear();

  if (m_divided) {
    m_NE->clear();
    m_NW->clear();
    m_SE->clear();
    m_SW->clear();

    m_NE.reset();
    m_NW.reset();
    m_SE.reset();
    m_SW.reset();

    m_divided = false;
  }
};  

void QuadTree::draw_helper() const {
  ofVec2f top_left{m_boundary->x - m_boundary->w, m_boundary->y - m_boundary->h};
  ofVec2f top_right{m_boundary->x + m_boundary->w, m_boundary->y - m_boundary->h};
  ofVec2f bottom_left{m_boundary->x - m_boundary->w, m_boundary->y + m_boundary->h};
  ofVec2f bottom_right{m_boundary->x + m_boundary->w, m_boundary->y + m_boundary->h};

  ofDrawLine(top_left, top_right);
  ofDrawLine(top_left, bottom_left);
  ofDrawLine(bottom_left, bottom_right);
  ofDrawLine(top_right, bottom_right);
}

void QuadTree::draw() const {
  draw_helper();

  if (m_divided) {
    m_NE->draw();
    m_NW->draw();
    m_SE->draw();
    m_SW->draw();
  }
}