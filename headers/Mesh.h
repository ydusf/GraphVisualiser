#pragma once

#include "ofMain.h"
#include "Node.h"

#include <memory>
#include <cstddef>

class Mesh {
public:
  void setup();
  void create_meshes(std::vector<std::unique_ptr<Node>>& nodes);
  void draw();
  void clear_meshes();
  void set_circle_resolution(float new_resolution);
  void set_colors(const ofColor& node_color, const ofColor& link_color, const ofColor& label_color);
  
private:
  ofVboMesh m_circle_mesh;
  ofVboMesh m_line_mesh;
  ofColor m_link_color;
  ofColor m_node_color;
  ofColor m_label_color;
  std::size_t m_circle_resolution;

  void create_vertex(ofVboMesh& mesh, const ofVec3f& pos, const ofColor& color);
  void create_index(ofVboMesh& mesh, std::size_t centre_idx, std::size_t current_idx);
  void create_circle(const Node& node);
  void create_line(const Node& node1, const Node& node2);
};