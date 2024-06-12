#include "../headers/Mesh.h"
#include "../headers/Node.h"

#include <memory>
#include <cstddef>
#include <vector>

Mesh::Mesh() {};

void Mesh::setup() {
  m_line_mesh.setMode(OF_PRIMITIVE_LINES);
  m_line_mesh.enableColors();
  m_circle_mesh.setMode(OF_PRIMITIVE_TRIANGLES);
  m_circle_mesh.enableIndices();
  m_circle_mesh.enableColors();
};

void Mesh::create_circle(const std::unique_ptr<Node>& node) {
  // initially used OF_PRIMITIVE_TRIANGLE_FAN but that attached the circles by the centre
  // instead three vertices are specificed for each triangle using OF_PRIMITIVE_TRIANGLES which avoids binding circles

  // potential optimisation is to try and get OF_PRIMITIVE_TRIANGLE_FAN or OF_PRIMITIVE_TRIANGLE_STRIP to work
  // As this would reduce the number of vertices required by 3x

  if(!node->within_bounds()) return;

  const float ANGLE_INCREMENT = TWO_PI / m_circle_resolution;
  const ofVec3f CENTRE = ofVec3f{node->pos.x, node->pos.y, 0.0f};

  for (std::size_t i = 0; i <= m_circle_resolution; ++i) {
    const float vx1 = CENTRE.x + node->radius * cos(ANGLE_INCREMENT * i);
    const float vy1 = CENTRE.y + node->radius * sin(ANGLE_INCREMENT * i);
    const float vx2 = CENTRE.x + node->radius * cos(ANGLE_INCREMENT * (i+1));
    const float vy2 = CENTRE.y + node->radius * sin(ANGLE_INCREMENT * (i+1));
    m_circle_mesh.addVertex(CENTRE);
    m_circle_mesh.addColor(m_node_color);
    m_circle_mesh.addVertex(ofVec3f{vx1, vy1, 0.0f});
    m_circle_mesh.addColor(m_node_color);
    m_circle_mesh.addVertex(ofVec3f{vx2, vy2, 0.0f});
    m_circle_mesh.addColor(m_node_color);
  }
}

void Mesh::create_line(const std::unique_ptr<Node>& node1, const std::unique_ptr<Node>& node2) {
  if(!node1->within_bounds() && !node2->within_bounds()) return;
  m_line_mesh.addVertex(ofVec3f(node1->pos.x, node1->pos.y, 0.0f));
  m_line_mesh.addColor(m_link_color);
  m_line_mesh.addVertex(ofVec3f(node2->pos.x, node2->pos.y, 0.0f));
  m_line_mesh.addColor(m_link_color);
}

void Mesh::create_meshes(std::vector<std::unique_ptr<Node>>& nodes) {
  for(std::size_t node_idx = 0; node_idx < nodes.size(); ++node_idx) {
    create_circle(nodes[node_idx]);
    // nodes[i]->draw_label(label_color);
    for(const std::size_t& neighbour_idx : nodes[node_idx]->neighbours) {
      create_line(nodes[node_idx], nodes[neighbour_idx]);
    }
  }
}

void Mesh::set_circle_resolution(float new_resolution) {
  m_circle_resolution = new_resolution;
};

void Mesh::draw() {
  m_line_mesh.draw();
  m_circle_mesh.draw();
};

void Mesh::clear_meshes() {
  m_circle_mesh.clear();
  m_line_mesh.clear();
};