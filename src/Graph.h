#pragma once

#include "ofMain.h"

#include <cstddef>
#include <tuple>
#include <memory>
#include <string>

struct Node {
  const std::size_t id;
  ofVec2f pos, vel;
  float radius;
  ofColor node_color;
  std::string label;
  std::vector<std::weak_ptr<Node> > neighbours;

  Node(std::size_t id, const ofVec2f& pos, float radius, const ofColor& color, std::string label);

  bool operator==(const std::shared_ptr<Node>& node) const;
  bool compare(const std::shared_ptr<Node>& first, const std::shared_ptr<Node>& second) const;

  bool within_bounds() const;

  void update();
  void draw_label(const ofColor& label_color) const;
  };