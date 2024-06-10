#include "ofApp.h"

#include <cstddef>
#include <iostream>
#include <unordered_map>
#include <thread>
#include <fstream>
#include <string>

void ofApp::create_graph(std::string filename) {
  std::ifstream file_nodes("../../../../graph_data/cpp_graph_nodes.txt"); 
  
  if(!file_nodes.is_open()) {
    std::cerr << "Failed to open the file_nodes!" << '\n';
    return;
  }

  std::string line1;
  while(std::getline(file_nodes, line1)) {
    nodes.emplace_back(std::make_unique<Node>(
      std::stoi(line1), ofVec2f{
        ofRandom(-START_DIST_MULTI*ofGetWidth() , START_DIST_MULTI*ofGetWidth()),
        ofRandom(-START_DIST_MULTI*ofGetHeight() , START_DIST_MULTI*ofGetHeight())
      }, gui.radius, line1)
    );
  }

  file_nodes.close();

  std::ifstream file_links("../../../../graph_data/cpp_graph_links.txt"); 
  
  if(!file_links.is_open()) {
    std::cerr << "Failed to open the file_links!" << '\n';
    return;
  }

  std::string line2;
  int first, second;
  while(std::getline(file_links, line2)) {
    // logic to create_graph
    std::istringstream iss(line2);
    if(!(iss >> first >> second)) {
      continue;
    }
    nodes[first]->neighbours.push_back(second);
    gui.link_count++;
  }
  gui.node_count = nodes.size();

  file_links.close();
}

//--------------------------------------------------------------
void ofApp::setup(){
  ofBackground(ofColor::black);
  ofEnableSmoothing();
  ofSetFrameRate(60);
  line_mesh.setMode(OF_PRIMITIVE_LINES);
  line_mesh.enableColors();
  circle_mesh.setMode(OF_PRIMITIVE_TRIANGLES);
  circle_mesh.enableIndices();
  circle_mesh.enableColors();

  create_graph("graph1.txt");
  
  gui.create_gui();
}

std::pair<std::size_t, std::size_t> ofApp::get_grid_cell(const ofVec2f& pos) {
  return std::make_pair(
    static_cast<std::size_t>(pos.x / ofGetWidth() / 4),
    static_cast<std::size_t>(pos.y / ofGetHeight() / 2)
  );
};

void ofApp::populate_grid() {
  grid.clear();
  for(std::size_t i = 0; i < nodes.size(); ++i) {
    const auto& cell = get_grid_cell(nodes[i]->pos);
    grid[cell].push_back(i);
  }
};

void ofApp::apply_gravity(std::vector<std::size_t>& cell) {
  for(const auto& node_idx : cell) {
    nodes[node_idx]->vel = -nodes[node_idx]->pos * GRAVITY;
  }
};
void ofApp::apply_node_repulsion(std::vector<std::size_t>& cell) {
  for(const auto& node_idx : cell) {
    for(const auto& neighbour_idx : nodes[node_idx]->neighbours) {
      const ofVec2f dir = nodes[neighbour_idx]->pos - nodes[node_idx]->pos;
      const float length_squared = dir.lengthSquared();
      if(length_squared == 0 || nodes[node_idx] == nodes[neighbour_idx]) continue;
      const ofVec2f force = dir / length_squared * gui.force;
      nodes[node_idx]->vel -= force;
      nodes[neighbour_idx]->vel += force; 
    }
  }
};
void ofApp::apply_link_forces(std::vector<std::size_t>& cell) {
  for(const auto& node_idx : cell) {
    for(const auto& neighbour_idx : nodes[node_idx]->neighbours) {
      const ofVec2f dist = nodes[node_idx]->pos - nodes[neighbour_idx]->pos;
      nodes[node_idx]->vel -= dist;
      nodes[neighbour_idx]->vel += dist;
    }
  }
};

void ofApp::apply_force_directed_layout(std::vector<std::size_t>& cell) {
  apply_gravity(cell);
  apply_node_repulsion(cell);
  apply_link_forces(cell);
}

void ofApp::apply_force_directed_layout_multithreaded() {
  populate_grid();
  std::vector<std::thread> threads;
  for(auto& cell : grid) {
    threads.emplace_back(&ofApp::apply_force_directed_layout, this, std::ref(cell.second));
  }
  for (auto& thread : threads) {
    thread.join();
  }
}

void ofApp::create_circle(ofVboMesh& mesh, const std::unique_ptr<Node>& node, std::size_t resolution) {
  // initially used OF_PRIMITIVE_TRIANGLE_FAN but that attached the circles by the centre
  // instead three vertices are specificed for each triangle using OF_PRIMITIVE_TRIANGLES which avoids binding circles

  // potential optimisation is to try and get OF_PRIMITIVE_TRIANGLE_FAN or OF_PRIMITIVE_TRIANGLE_STRIP to work
  // As this would reduce the number of vertices required by 3x

  if(!node->within_bounds()) return;

  const float ANGLE_INCREMENT = TWO_PI / resolution;
  const ofVec3f CENTRE = ofVec3f{node->pos.x, node->pos.y, 0.0f};

  for (std::size_t i = 0; i <= resolution; ++i) {
    const float vx1 = CENTRE.x + node->radius * cos(ANGLE_INCREMENT * i);
    const float vy1 = CENTRE.y + node->radius * sin(ANGLE_INCREMENT * i);
    const float vx2 = CENTRE.x + node->radius * cos(ANGLE_INCREMENT * (i+1));
    const float vy2 = CENTRE.y + node->radius * sin(ANGLE_INCREMENT * (i+1));
    mesh.addVertex(CENTRE);
    mesh.addColor(gui.node_color);
    mesh.addVertex(ofVec3f{vx1, vy1, 0.0f});
    mesh.addColor(gui.node_color);
    mesh.addVertex(ofVec3f{vx2, vy2, 0.0f});
    mesh.addColor(gui.node_color);
  }
}

void ofApp::create_line(ofVboMesh &mesh, const std::unique_ptr<Node>& node1, const std::unique_ptr<Node>& node2) {
  if(!node1->within_bounds() && !node2->within_bounds()) return;
  mesh.addVertex(ofVec3f(node1->pos.x, node1->pos.y, 0.0f));
  mesh.addColor(gui.link_color);
  mesh.addVertex(ofVec3f(node2->pos.x, node2->pos.y, 0.0f));
  mesh.addColor(gui.link_color);
}

void ofApp::create_meshes(std::size_t from, std::size_t to) {
  for(std::size_t i = from; i < to; ++i) {
    create_circle(circle_mesh, nodes[i], circle_resolution);
    // nodes[i]->draw_label(gui.label_color);
    for(const auto& j : nodes[i]->neighbours) {
      create_line(line_mesh, nodes[i], nodes[j]);
    }
  }
}

//--------------------------------------------------------------
void ofApp::update(){
  gui.update_gui();  

  for(const auto& node : nodes) {
    node->radius = gui.radius;
  }

  // graph updates
  for (const auto& node : nodes) {
    node->update();
  }
  apply_force_directed_layout_multithreaded();

  circle_resolution = std::clamp(gui.radius*1.5f, 4.0f, 25.0f);

  // user interaction
  if(node_being_dragged_idx != -1) drag();
  if(panning) pan();
}

//--------------------------------------------------------------
void ofApp::draw(){  
  gui.draw();

  // centralise visualisation
  ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

  // clear graph
  circle_mesh.clear();
  line_mesh.clear();

  create_meshes(0, nodes.size());

  // display graph
  line_mesh.draw();
  circle_mesh.draw();
};

//--------------------------------------------------------------
void ofApp::exit(){
  std::cout << "Graph Visualisation Closed..." << '\n';
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  switch (key) {
    case 'd':
      create_nodes_and_links();
      break;
    case 'r':
      if(nodes.size() < 1) break;
      nodes.clear();
      break;
  }
}

void ofApp::create_nodes_and_links() {
  for(std::size_t i = 0; i < 250; ++i) {
    nodes.emplace_back(std::make_unique<Node>(
      i, ofVec2f{
        ofRandom(-START_DIST_MULTI*ofGetWidth() , START_DIST_MULTI*ofGetWidth()),
        ofRandom(-START_DIST_MULTI*ofGetHeight() , START_DIST_MULTI*ofGetHeight())
      }, gui.radius, std::to_string(i))
    );
  }
  for(std::size_t i = gui.node_count; i < nodes.size(); ++i) {
    for(std::size_t j = 0; j < 3; ++j) {
      std::size_t random_idx = static_cast<int>(ofRandom(0, nodes.size()));
      while(random_idx == i) {
        random_idx = static_cast<int>(ofRandom(0, nodes.size()));
      };
      nodes[i]->neighbours.push_back(random_idx);
      gui.link_count++;
    }
  }
  gui.node_count = nodes.size();
};

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

void ofApp::update_mouse_position() {
  prev_mouse_position = mouse_position;
  mouse_position = {ofGetMouseX() - ofGetWidth() / 2.0f, ofGetMouseY() - ofGetHeight() / 2.0f};
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

void ofApp::pan() {
  update_mouse_position();
  const ofVec2f PAN_OFFSET =  mouse_position - prev_mouse_position;
  for(const auto& node : nodes) {
    node->pos += PAN_OFFSET;
  };
}

void ofApp::find_node_being_dragged() {
  update_mouse_position();
  for(std::size_t i = 0; i < nodes.size(); ++i) {
    if(nodes[i]->pos.distance(mouse_position) >= nodes[i]->radius) continue;
    node_being_dragged_idx = i;
    break;
  }
}

void ofApp::drag() {
  update_mouse_position();
  nodes[node_being_dragged_idx]->pos.interpolate(mouse_position, lerp_val);
  if(lerp_val < 0.95) lerp_val += 0.02;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
  update_mouse_position();

  if(button == OF_MOUSE_BUTTON_LEFT) {
    if(node_being_dragged_idx != -1) return;

    find_node_being_dragged();

    // node not found so must be panning
    if(node_being_dragged_idx == -1) panning = true;
  }

  if(button == OF_MOUSE_BUTTON_RIGHT) {
    float min_dist = mouse_position.distance(nodes[0]->pos);
    std::size_t closest_node = 0;

    for(std::size_t i = 1; i < nodes.size(); ++i) {
      const float new_dist = mouse_position.distance(nodes[i]->pos);
      if(new_dist < min_dist) {
        min_dist = new_dist;
        closest_node = i;
      }
    } 

    nodes.emplace_back(std::make_unique<Node>(
      nodes.size(), 
      ofVec2f{mouse_position}, 
      gui.radius, 
      std::to_string(nodes.size())
    ));
    if(nodes.size() < 2) return;
    nodes[nodes.size()-1]->neighbours.push_back(closest_node);
  }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
  // reset mouse-related variables
  panning = false;
  node_being_dragged_idx = -1;
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){
  // // zoom functionality
  // scrollY = std::clamp(scrollY, -1.0f, 1.0f);
  // force = std::clamp(force * scrollY, MIN_FORCE, MAX_FORCE);
  // for(auto& node : nodes) {
  //   node->radius = std::clamp(glm::abs(node->radius - (scrollY * 0.1f)), MIN_RADIUS, MAX_RADIUS);
  // }
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

// Fix force-directed layout algorithm during multithreading - potentially using a spatial grid
// Optimise circle creation by using OF_PRIMITIVE_TRIANGLE_STRIP / OF_PRIMITIVE_TRIANGLE_FAN
// Create zoom functionality
// Optimise label drawings by creating them on a mesh and then drawing once - maybe not possible.
