#include "../headers/Grid.h"
#include "../headers/Node.h"

#include <cstddef>
#include <utility>
#include <memory>
#include <unordered_map>
#include <functional>

std::size_t GridHash::operator()(const std::pair<std::size_t, std::size_t>& key) const {
  return std::hash<std::size_t>()(key.first) ^ (std::hash<std::size_t>()(key.second) << 1);
}

std::pair<std::size_t, std::size_t> SpatialGrid::get_cell(const ofVec2f& pos) {
  // Get rid of magic numbers and replace with constants
  return std::make_pair(
    static_cast<std::size_t>(pos.x / ofGetWidth() / 4),
    static_cast<std::size_t>(pos.y / ofGetHeight() / 2)
  );
};

void SpatialGrid::populate(std::vector<std::unique_ptr<Node>>& nodes) {
  // nodes that are not visible are still added to grid (layout algorithm still applied)
  for(std::size_t i = 0; i < nodes.size(); ++i) {
    const std::pair<std::size_t, std::size_t>& cell = get_cell(nodes[i]->pos);
    grid[cell].push_back(i);
  }
};

void SpatialGrid::clear() {
  grid.clear();
};