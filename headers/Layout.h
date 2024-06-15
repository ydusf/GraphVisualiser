#pragma once

#include "ofMain.h"
#include "Node.h"

#include <memory>
#include <cstddef>

class Layout {
public:
  void apply_force_directed_layout(std::vector<std::unique_ptr<Node>>& nodes, const std::vector<std::size_t>& cell);
  void set_force_multi(float new_force_multi);

private:
  float m_force_multi;

  void apply_gravity(std::vector<std::unique_ptr<Node>>& nodes, const std::vector<std::size_t>& cell);
  void apply_node_repulsion(std::vector<std::unique_ptr<Node>>& nodes, const std::vector<std::size_t>& cell);
  void apply_link_forces(std::vector<std::unique_ptr<Node>>& nodes, const std::vector<std::size_t>& cell);
};