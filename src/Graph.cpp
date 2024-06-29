#include "../headers/Graph.h"
#include "../headers/Node.h"
#include "../headers/Mesh.h"
#include "../headers/Layout.h"
#include "../headers/BarnesHut.h"

#include <memory>

Graph::Graph()
  : nodes(std::vector<std::unique_ptr<Node>>()),
    mesh(std::make_unique<Mesh>()),
    layout(std::make_unique<Layout>())
  {
    Quad boundary = Quad(0, 0, static_cast<int>(ofGetWidth()), static_cast<int>(ofGetHeight()));
    quad_tree = std::make_unique<QuadTree>(boundary, 4);
  };

void Graph::setup() {
  mesh->setup();
}

void Graph::level_of_detail() {
  
};

void Graph::update() {
  for (const std::unique_ptr<Node>& node : nodes) {
    node->update();
  }

  level_of_detail();

  quad_tree->clear();
  for(std::unique_ptr<Node>& node : nodes) {
    quad_tree->insert(*node);
  }

  layout->apply_force_directed_layout(nodes, *quad_tree);
}

void Graph::draw() {
  mesh->clear_meshes();
  mesh->create_meshes(nodes);
  mesh->draw();
  quad_tree->draw();
}