#pragma once

#include "ofMain.h"

#include <cstddef>
#include <memory>
#include <string>

class Node {
public:
  ofVec2f pos, vel;
  float radius;
  std::string label;
  std::vector<std::size_t> neighbours;
  std::vector<std::unique_ptr<Node> > combined;

  Node(std::size_t id, const ofVec2f& pos, float radius, std::string label);

  bool operator==(const std::unique_ptr<Node>& node) const;

  bool within_bounds() const;

  void update();
  void draw_label(const ofColor& label_color) const;

private:
  const std::size_t m_id;
};