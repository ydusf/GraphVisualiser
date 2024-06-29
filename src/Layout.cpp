#include "../headers/Layout.h"
#include "../headers/Node.h"
#include "../headers/BarnesHut.h"

#include <memory>
#include <vector>
#include <cstddef>
#include <cassert>
#include <future>

const float GRAVITY = 1.1f;

void Layout::apply_gravity(std::vector<std::unique_ptr<Node>>& nodes) {
  for(std::unique_ptr<Node>& node : nodes) {
    node->vel = -node->pos * GRAVITY;
  }
};

void Layout::resolve_node_repulsion(Node& curr_node, Node& next_node) {
  if(curr_node == next_node) return;
  const ofVec2f dir = next_node.pos - curr_node.pos;
  const float length_squared = (dir.lengthSquared() + 1e-10f);
  assert(length_squared > 0);
  const ofVec2f force = dir * (1 / length_squared) * m_force_multi;

  curr_node.vel -= force;
  next_node.vel += force; 
};

void Layout::apply_node_repulsion(std::vector<std::unique_ptr<Node>>& nodes, QuadTree& quad_tree) {
  for(std::unique_ptr<Node>& node : nodes) {
    Node& curr_node = *node;
    for(const std::size_t& node_id : quad_tree.get_nearby_nodes(curr_node, nodes, m_force_multi * 0.4f)) {
      Node& next_node = *nodes[node_id];
      resolve_node_repulsion(curr_node, next_node);
    };
  };
};
void Layout::apply_link_forces(std::vector<std::unique_ptr<Node>>& nodes) {
  for(std::unique_ptr<Node>& node : nodes) {
    Node& curr_node = *node;
    for(const std::size_t& conn : curr_node.connections) {
      Node& conn_node = *nodes[conn];
      const ofVec2f dist = curr_node.pos - conn_node.pos;
      curr_node.vel -= dist;
      conn_node.vel += dist;
    }
  }
};

void Layout::apply_force_directed_layout(std::vector<std::unique_ptr<Node>>& nodes, QuadTree& quad_tree) {
  apply_gravity(nodes);
  apply_link_forces(nodes);
  apply_node_repulsion(nodes, quad_tree);
}

void Layout::set_force_multi(float new_force_multi) {
  m_force_multi = new_force_multi;
};