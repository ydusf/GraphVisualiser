#pragma once

#include "ofMain.h"
#include "Node.h"

#include <cstddef>
#include <utility>
#include <memory>
#include <unordered_map>
#include <functional>

struct GridHash {
  std::size_t operator()(const std::pair<std::size_t, std::size_t>& key) const;
};

class SpatialGrid {
public:
  std::unordered_map<
		std::pair<std::size_t, std::size_t>,
		std::vector<std::size_t>, GridHash
	> grid;

  SpatialGrid();
  
  std::pair<std::size_t, std::size_t> get_cell(const ofVec2f& pos);
	void populate(std::vector<std::unique_ptr<Node>>& nodes);
  void clear();
};