#pragma once

#include "ofMain.h"
#include "Node.h"
#include "BarnesHut.h"
#include "Layout.h"
#include "Mesh.h"

#include <memory>

#ifndef GRAPH_H
#define GRAPH_H

class Graph {
public:
  std::vector<std::unique_ptr<Node> > nodes;
  std::unique_ptr<Mesh> mesh;  
  std::unique_ptr<Layout> layout;
  std::unique_ptr<QuadTree> quad_tree;
  
  Graph();

  void setup();
  void add_node(std::size_t id, ofVec2f pos, int radius, std::string label);
  void add_edge(std::size_t from, std::size_t to);
  void update();
  void draw();

private:
  void level_of_detail();
  void cluster_nodes();
};

#endif // GRAPH_H