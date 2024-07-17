#pragma once

#include "ofMain.h"
#include "Node.h"
#include "BarnesHut.h"

#include <memory>
#include <cstddef>

class Layout {
public:
  void apply_force_directed_layout(std::vector<std::unique_ptr<Node>>& nodes, QuadTree& spatial_grid);
  void set_force_multi(float new_force_multi);

private:
  float m_force_multi;

  void apply_gravity(std::vector<std::unique_ptr<Node>>& nodes);
  void resolve_node_repulsion(Node& curr_node, Node& next_node);
  void apply_node_repulsions(std::vector<std::unique_ptr<Node>>& nodes, QuadTree& spatial_grid);
  void resolve_link_attraction(Node& curr_node, Node& conn_node);
  void apply_link_attractions(std::vector<std::unique_ptr<Node>>& nodes);
};