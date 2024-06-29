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
  void apply_node_repulsion(std::vector<std::unique_ptr<Node>>& nodes, QuadTree& spatial_grid);
  void apply_link_forces(std::vector<std::unique_ptr<Node>>& nodes);
};