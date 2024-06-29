#pragma once

#include "ofMain.h"

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Node {
public:
  ofVec2f pos, vel;
  float radius;
  std::vector<std::size_t> connections;
  std::string label;

  Node(std::size_t id, const ofVec2f& pos, float radius, std::string label);

  bool operator==(const Node& node) const;

  bool within_bounds() const;

  void update();
  void draw_label(const ofColor& label_color) const;
  std::size_t get_id() const;

private:
  const std::size_t m_id;
};