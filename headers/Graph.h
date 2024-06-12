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
  std::unique_ptr<SpatialGrid> grid;
  
  Graph();

  void setup();
  void update();
  void draw();
};

#endif // GRAPH_H