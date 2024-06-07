#pragma once

#include "ofMain.h"

#include <cstddef>
#include <tuple>
#include <memory>
#include <string>

struct Node {
  std::size_t id;
  ofVec2f pos, vel;
  float radius;
  ofColor node_color;
  ofColor label_color;
  std::string label;
  std::vector<std::shared_ptr<Node> > neighbours;

  Node(std::size_t id, ofVec2f pos, float radius);
  Node(std::size_t id, ofVec2f pos, float radius, ofColor color);
  Node(std::size_t id, ofVec2f pos, float radius, ofColor color, std::string label);

  bool operator==(const std::shared_ptr<Node>& node);

  bool within_bounds();

  void update();
  void draw_label();
  
  ~Node();
};