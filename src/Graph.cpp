#include "../headers/Graph.h"
#include "../headers/Node.h"
#include "../headers/Mesh.h"
#include "../headers/Layout.h"
#include "../headers/Grid.h"

#include <memory>

Graph::Graph()
  : nodes(std::vector<std::unique_ptr<Node>>()),
    mesh(std::make_unique<Mesh>()),
    layout(std::make_unique<Layout>()),
    spatial_grid(std::make_unique<SpatialGrid>())
  
  {};

void Graph::setup() {
  mesh->setup();
}

void Graph::update() {
  for (const std::unique_ptr<Node>& node : nodes) {
    node->update();
  }

  spatial_grid->clear();
  spatial_grid->populate(nodes);

  for(auto&[cell, cell_nodes] : spatial_grid->grid) {
    if(cell_nodes.empty()) continue;
    layout->apply_force_directed_layout(nodes, cell_nodes);
  }
}

void Graph::draw() {
  mesh->clear_meshes();
  mesh->create_meshes(nodes);
  mesh->draw();
}