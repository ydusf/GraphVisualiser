#include "Grid.h"

const float COLLISION_DAMPENING = 1.0f;

std::size_t GridHash::operator()(const std::tuple<int, int>& tuple) const {
  auto hash1 = std::hash<int>{}(std::get<0>(tuple));
  auto hash2 = std::hash<int>{}(std::get<1>(tuple));
  return hash1 ^ (hash2 << 1);
};

void Grid::populate_grid(std::vector<std::shared_ptr<Node>>& nodes) {
  grid.clear();

  for(const auto& node : nodes) {
    const auto cell = node->get_grid_cell();
    grid[cell].push_back(node);
  }
};

void Grid::check_boundary_collision(std::shared_ptr<Node>& node) {
  const ofVec2f upper_bounds = ofVec2f{static_cast<float>(ofGetWidth()), static_cast<float>(ofGetHeight())} - node->radius;
  const ofVec2f lower_bounds = ofVec2f{node->radius, node->radius};  

  if(node->pos.x <= lower_bounds.x || node->pos.x >= upper_bounds.x)  {
    node->pos.x = std::clamp(node->pos.x, lower_bounds.x, upper_bounds.x);
    node->vel.x *= -1 * COLLISION_DAMPENING;  
  }
  if(node->pos.y <= lower_bounds.y || node->pos.y >= upper_bounds.y) {
    node->pos.y = std::clamp(node->pos.y, lower_bounds.y, upper_bounds.y);
    node->vel.y *= -1 * COLLISION_DAMPENING;  
  }
};

void Grid::resolve_collision(std::shared_ptr<Node>& node1, std::shared_ptr<Node>& node2) {
  ofVec3f delta = node1->pos - node2->pos;
  const float distance = delta.length();
  const float min_distance = (node1->radius + node2->radius);

  if (distance >= min_distance) return;

  const ofVec3f normal = delta.normalize();
  const ofVec3f relative_velocity = node1->vel - node2->vel;
  const float vel_along_normal = relative_velocity.dot(normal);

  if (vel_along_normal > 0) return;

  float j = -(1 + 0.5f) * vel_along_normal;
  j /= 1 / node1->radius + 1 / node2->radius;

  const ofVec3f impulse = normal * j;
  const float penetration_depth = min_distance - distance;
  const ofVec3f correction = normal * (penetration_depth / (1 / node1->radius + 1 / node2->radius));

  node1->vel += impulse * (1 / node1->radius);
  node2->vel -= impulse * (1 / node2->radius);
  node1->pos += correction * (1 / node1->radius);
  node2->pos -= correction * (1 / node2->radius);
};

void Grid::handle_collisions(std::vector<std::shared_ptr<Node>>& nodes) {
  populate_grid(nodes);

  for(auto& curr_node : nodes) {
    check_boundary_collision(curr_node);
    // const auto curr_cell = curr_node->get_grid_cell();

    // // check nodes in same cell
    // for(auto& next_node : grid[curr_cell]) {
    //   if(curr_node == next_node) continue; // skip same particle
    //   resolve_collision(curr_node, next_node);
    // }

    // // check nodes in neighbouring cells
    // for(const auto& cell : curr_node->neighbour_cells) {
    //   if(grid.find(cell) == grid.end()) continue; // cell does not exist
    //   for(auto& neighbour_node : grid[cell]) {
    //     resolve_collision(curr_node, neighbour_node);
    //   };
    // }
  }
};