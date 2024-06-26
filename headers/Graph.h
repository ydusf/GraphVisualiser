#pragma once

#include "ofMain.h"
#include "Node.h"
#include "Grid.h"
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
  std::unique_ptr<SpatialGrid> spatial_grid;
  
  Graph();

  void setup();
  void update();
  void draw();

private:

  void level_of_detail();
};

#endif // GRAPH_H