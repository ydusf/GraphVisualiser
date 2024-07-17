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
  };
};

void Layout::resolve_node_repulsion(Node& curr_node, Node& next_node) {
  const ofVec2f dir = next_node.pos - curr_node.pos;
  const float length_squared = dir.lengthSquared() + 1e-10f;
  assert(length_squared > 0);
  const ofVec2f force = dir * (1 / length_squared) * m_force_multi;

  curr_node.vel -= force;
  next_node.vel += force; 
};

void Layout::apply_node_repulsions(std::vector<std::unique_ptr<Node>>& nodes, QuadTree& quad_tree) {
  const float theta = 0.5f;
  for(std::unique_ptr<Node>& curr_node : nodes) {
    quad_tree.traverse(*curr_node, nodes, m_force_multi, theta);
  };
};

void Layout::resolve_link_attraction(Node& curr_node, Node& conn_node) {
  const ofVec2f dist = curr_node.pos - conn_node.pos;
  curr_node.vel -= dist;
  conn_node.vel += dist;
};

void Layout::apply_link_attractions(std::vector<std::unique_ptr<Node>>& nodes) {
  for(std::unique_ptr<Node>& curr_node : nodes) {
    for(const std::size_t& conn : curr_node->edges) {
      resolve_link_attraction(*curr_node, *nodes[conn]);
    };
  };
};

void Layout::apply_force_directed_layout(std::vector<std::unique_ptr<Node>>& nodes, QuadTree& quad_tree) {
  apply_gravity(nodes);
  apply_link_attractions(nodes);
  apply_node_repulsions(nodes, quad_tree);
};

void Layout::set_force_multi(float new_force_multi) {
  m_force_multi = new_force_multi;
};