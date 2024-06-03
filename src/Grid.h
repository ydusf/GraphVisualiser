#pragma once

#include "Graph.h"

#include <tuple>
#include <cstddef>
#include <unordered_map>
#include <memory>

#ifndef GRID_H
#define GRID_H

struct GridHash {
	std::size_t operator()(const std::tuple<int, int>& tuple) const;
};

class Grid {
public:
  void populate_grid(std::vector<std::shared_ptr<Node>>& nodes);
  void check_boundary_collision(std::shared_ptr<Node>& node);
  void resolve_collision(std::shared_ptr<Node>& node1, std::shared_ptr<Node>& node2);
  void handle_collisions(std::vector<std::shared_ptr<Node>>& nodes);

private:
  std::unordered_map<std::tuple<int, int>, std::vector<std::shared_ptr<Node> >, GridHash> grid;
};

#endif // GRID_H



