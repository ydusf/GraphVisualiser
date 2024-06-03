#pragma once

#include "ofMain.h"

struct Node {
  ofVec2f pos, vel;
  float radius;
  ofColor node_color;
  ofColor label_color;
  std::string label;

  Node(ofVec2f pos, float radius);
  Node(ofVec2f pos, float radius, ofColor color);
  Node(ofVec2f pos, float radius, ofColor color, std::string label);

  bool operator==(const std::shared_ptr<Node>& node);

  void update();
  void draw();
  
  ~Node();
};

struct Link {
  std::weak_ptr<Node> start;
  std::weak_ptr<Node> end;
  float width;
  ofColor color;

  Link(std::shared_ptr<Node> start, std::shared_ptr<Node> end);
  Link(std::shared_ptr<Node> start, std::shared_ptr<Node> end, ofColor color);

  void update();
  void draw();

  ~Link();
};