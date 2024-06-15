#include "../headers/Layout.h"
#include "../headers/Node.h"

#include <memory>
#include <vector>
#include <cstddef>
#include <cassert>

const float GRAVITY = 1.1f;

void Layout::apply_gravity(std::vector<std::unique_ptr<Node>>& nodes, const std::vector<std::size_t>& cell) {
  for(const std::size_t& node_idx : cell) {
    nodes[node_idx]->vel = -nodes[node_idx]->pos * GRAVITY;
  }
};
void Layout::apply_node_repulsion(std::vector<std::unique_ptr<Node>>& nodes, const std::vector<std::size_t>& cell) {
  for(const std::size_t& node_idx : cell) {
    for(const std::unique_ptr<Node>& next_node : nodes) {
      if(nodes[node_idx] == next_node) continue;
      const ofVec2f dir = next_node->pos - nodes[node_idx]->pos;
      const double epsilon = 1e-10; 
      const float length_squared = dir.lengthSquared() + epsilon;
      assert(length_squared > 0);
      const ofVec2f force = dir / length_squared * m_force_multi;
      nodes[node_idx]->vel -= force;
      next_node->vel += force; 
    }
  }
};
void Layout::apply_link_forces(std::vector<std::unique_ptr<Node>>& nodes, const std::vector<std::size_t>& cell) {
  for(const std::size_t& node_idx : cell) {
    for(const std::size_t& neighbour_idx : nodes[node_idx]->neighbours) {
      const ofVec2f dist = nodes[node_idx]->pos - nodes[neighbour_idx]->pos;
      nodes[node_idx]->vel -= dist;
      nodes[neighbour_idx]->vel += dist;
    }
  }
};

void Layout::apply_force_directed_layout(std::vector<std::unique_ptr<Node>>& nodes, const std::vector<std::size_t>& cell) {
  apply_gravity(nodes, cell);
  apply_node_repulsion(nodes, cell);
  apply_link_forces(nodes, cell);
}

void Layout::set_force_multi(float new_force_multi) {
  m_force_multi = new_force_multi;
};