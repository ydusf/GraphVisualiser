#include "../headers/Mesh.h"
#include "../headers/Node.h"

#include <memory>
#include <cstddef>
#include <vector>

void Mesh::setup() {
  m_line_mesh.setMode(OF_PRIMITIVE_LINES);
  m_line_mesh.enableColors();
  m_circle_mesh.setMode(OF_PRIMITIVE_TRIANGLES);
  m_circle_mesh.enableIndices();
  m_circle_mesh.enableColors();
};

void Mesh::create_vertex(ofVboMesh& mesh, const ofVec3f& pos, const ofColor& color) {
  mesh.addVertex(pos);
  mesh.addColor(color);
};

void Mesh::create_index(ofVboMesh& mesh, std::size_t centre_idx, std::size_t current_idx) {
  mesh.addIndex(centre_idx);
  mesh.addIndex(centre_idx+current_idx);
  mesh.addIndex(centre_idx+current_idx+1);
}

void Mesh::create_circle(const Node& node) {
  // initially used OF_PRIMITIVE_TRIANGLE_FAN but that attached the circles by the centre
  // instead three vertices are specificed for each triangle using OF_PRIMITIVE_TRIANGLES which avoids binding circles

  // potential optimisation is to try and get OF_PRIMITIVE_TRIANGLE_FAN or OF_PRIMITIVE_TRIANGLE_STRIP to work
  // As this would reduce the number of vertices required by 3x

  // plan:

  // create first triangle of every circle using OF_PRIMITIVE_TRIANGLES
  // create remaining triangles of every circle using OF_PRIMITIVE_TRIANGLE_STRIP
  // reduces vertices from 3N to N+4 which is slighty > than N + 2 but much < 3N

  if(!node.within_bounds()) return;

  const float ANGLE_INCREMENT = TWO_PI / m_circle_resolution;
  const ofVec3f CENTRE = ofVec3f{node.pos.x, node.pos.y, 0.0f};
  std::size_t centre_idx = m_circle_mesh.getNumVertices();

  create_vertex(m_circle_mesh, CENTRE, m_node_color);

  for (std::size_t i = 0; i <= m_circle_resolution; ++i) {
    const float vx1 = node.radius * cos(ANGLE_INCREMENT * i);
    const float vy1 = node.radius * sin(ANGLE_INCREMENT * i);
    create_vertex(m_circle_mesh, ofVec3f{vx1, vy1, 0.0f} + CENTRE, m_node_color);
    create_index(m_circle_mesh, centre_idx, i);
  }

  // for (std::size_t i = 0; i <= m_circle_resolution; ++i) {
  //   const float vx1 = CENTRE.x + node.radius * cos(ANGLE_INCREMENT * i);
  //   const float vy1 = CENTRE.y + node.radius * sin(ANGLE_INCREMENT * i);
  //   const float vx2 = CENTRE.x + node.radius * cos(ANGLE_INCREMENT * (i+1));
  //   const float vy2 = CENTRE.y + node.radius * sin(ANGLE_INCREMENT * (i+1));
  //   create_vertex(m_circle_mesh, CENTRE, m_node_color);
  //   create_vertex(m_circle_mesh, ofVec3f{vx1, vy1, 0.0f}, m_node_color);
  //   create_vertex(m_circle_mesh, ofVec3f{vx2, vy2, 0.0f}, m_node_color);
  // }
}

void Mesh::create_line(const Node& node1, const Node& node2) {
  if(!node1.within_bounds() && !node2.within_bounds()) return;
  create_vertex(m_line_mesh, ofVec3f{node1.pos.x, node1.pos.y, 0.0f}, m_link_color);
  create_vertex(m_line_mesh, ofVec3f{node2.pos.x, node2.pos.y, 0.0f}, m_link_color);
}

void Mesh::create_meshes(std::vector<std::unique_ptr<Node>>& nodes) {
  for(std::size_t node_idx = 0; node_idx < nodes.size(); ++node_idx) {
    Node& curr_node = *nodes[node_idx];
    create_circle(curr_node);
    // curr_node.draw_label(m_label_color);
    for(const std::size_t& conn : curr_node.connections) {
      Node& conn_node = *nodes[conn];
      create_line(curr_node, conn_node);
    }
  }
}

void Mesh::set_circle_resolution(float new_resolution) {
  m_circle_resolution = new_resolution;
};

void Mesh::set_colors(const ofColor& node_color, const ofColor& link_color, const ofColor& label_color) {
  m_node_color = node_color;
  m_link_color = link_color;
  m_label_color = label_color;
};

void Mesh::draw() {
  m_line_mesh.draw();
  m_circle_mesh.draw();
};

void Mesh::clear_meshes() {
  m_circle_mesh.clear();
  m_line_mesh.clear();
};