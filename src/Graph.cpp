#include "../headers/Graph.h"
#include "../headers/Node.h"
#include "../headers/Mesh.h"
#include "../headers/Layout.h"
#include "../headers/BarnesHut.h"

#include <memory>
#include <unordered_set>

Graph::Graph()
  : nodes(std::vector<std::unique_ptr<Node>>()),
    mesh(std::make_unique<Mesh>()),
    layout(std::make_unique<Layout>())
  {
    Quad boundary = Quad(0, 0, static_cast<int>(ofGetWidth()), static_cast<int>(ofGetHeight()));
    quad_tree = std::make_unique<QuadTree>(boundary, 15);
  };

void Graph::setup() {
  mesh->setup();
}

void Graph::add_node(std::size_t id, ofVec2f pos, int radius, std::string label) {
  nodes.emplace_back(std::make_unique<Node>(id, pos, radius, label));
};

void Graph::add_edge(std::size_t from, std::size_t to) {
  nodes[from]->edges.push_back(to);
  nodes[to]->edges.push_back(from);
};

void Graph::level_of_detail() {
  
};

void Graph::cluster_nodes() {
  
};

void Graph::update() {
  for (const std::unique_ptr<Node>& node : nodes) {
    node->update();
  }

  level_of_detail();

  quad_tree->clear();
  for(std::unique_ptr<Node>& node : nodes) {
    // if(!node->within_bounds()) continue;
    quad_tree->insert(*node);
  }

  cluster_nodes();

  layout->apply_force_directed_layout(nodes, *quad_tree);
}

void Graph::draw() {
  mesh->clear_meshes();
  mesh->create_meshes(nodes);
  mesh->draw();
  // quad_tree->draw();
}