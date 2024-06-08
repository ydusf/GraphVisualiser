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
  ofColor label_color;
  std::string label;
  std::vector<std::shared_ptr<Node> > neighbours;

  Node(const std::size_t id, const ofVec2f pos, const float radius, const ofColor color, const std::string label);

  bool operator==(const std::shared_ptr<Node>& node);

  bool within_bounds();

  void update();
  void draw_label();
  };