#pragma once

#include "ofMain.h"
#include "Node.h"

#include <memory>
#include <cstddef>

class Mesh {
public:

  Mesh();

  void setup();
  void create_meshes(std::vector<std::unique_ptr<Node>>& nodes);
  void draw();
  void clear_meshes();
  void set_circle_resolution(float new_resolution);
  
private:
  ofVboMesh m_circle_mesh;
  ofVboMesh m_line_mesh;
  ofColor m_link_color;
  ofColor m_node_color;
  ofColor m_label_color;
  std::size_t m_circle_resolution;

  void create_circle(const std::unique_ptr<Node>& node);
  void create_line(const std::unique_ptr<Node>& node1, const std::unique_ptr<Node>& node2);
};