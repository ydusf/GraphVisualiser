#pragma once

#include "ofMain.h"

struct Node {
  ofVec2f pos, vel;
  float radius;
  ofColor color;
  std::tuple<int, int> grid_cell;
  std::vector<std::tuple<int, int> > neighbour_cells;

  Node(ofVec2f pos, float radius);
  Node(ofVec2f pos, float radius, ofColor color);

  bool operator==(const std::shared_ptr<Node>& node);

  void update();
  void draw();
  void update_grid_cell();
  std::tuple<int, int> get_grid_cell();
  void update_neighbour_cells();
  
  ~Node();
};

struct Link {
  std::weak_ptr<Node> start;
  std::weak_ptr<Node> end;
  float width;
  ofColor color;
  float max_distance;

  Link(std::shared_ptr<Node> start, std::shared_ptr<Node> end);
  Link(std::shared_ptr<Node> start, std::shared_ptr<Node> end, float max_distance);
  Link(std::shared_ptr<Node> start, std::shared_ptr<Node> end, float width, ofColor color);

  void update();
  void draw();

  ~Link();
};