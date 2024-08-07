#include "../headers/ofApp.h"
#include "../headers/Layout.h"

#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

const std::string NODES_FILE_PATH = "../../../../graph_data/cpp_graph_nodes.txt";
const std::string LINKS_FILE_PATH = "../../../../graph_data/cpp_graph_links.txt";

void ofApp::create_graph(std::string filename) {
  std::ifstream file_nodes(NODES_FILE_PATH); 
  
  if(!file_nodes.is_open()) {
    std::cerr << "Failed to open the file_nodes!" << '\n';
    return;
  }

  std::string line1;
  while(std::getline(file_nodes, line1)) {
    graph.nodes.emplace_back(std::make_unique<Node>(
      std::stoi(line1), ofVec2f{
        ofRandom(-START_DIST_MULTI*ofGetWidth() , START_DIST_MULTI*ofGetWidth()),
        ofRandom(-START_DIST_MULTI*ofGetHeight() , START_DIST_MULTI*ofGetHeight())
      }, gui.radius, line1)
    );
  }

  file_nodes.close();

  std::ifstream file_links(LINKS_FILE_PATH); 
  
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
    graph.nodes[first]->edges.push_back(second);
    graph.nodes[second]->edges.push_back(first);
    gui.link_count++;
  }
  gui.node_count = graph.nodes.size();

  file_links.close();
}

//--------------------------------------------------------------
void ofApp::setup(){
  ofBackground(ofColor::black);
  ofEnableSmoothing();
  ofSetFrameRate(0);

  // create_graph("file");
  gui.setup();
  graph.setup();

}

//--------------------------------------------------------------
void ofApp::update(){
  gui.update();  

  for(const std::unique_ptr<Node>& node : graph.nodes) {
    node->radius = gui.radius;
  }
  
  graph.mesh->set_circle_resolution(std::clamp(gui.radius*1.5f, 4.0f, 25.0f));
  graph.mesh->set_colors(gui.node_color, gui.link_color, gui.label_color);
  graph.layout->set_force_multi(gui.force);

  graph.update();

  // user interaction
  if(node_being_dragged_idx != -1) drag();
  if(panning) pan();
}

//--------------------------------------------------------------
void ofApp::draw(){  
  gui.draw();

  // centralise visualisation
  ofTranslate(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f);

  graph.draw();
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
      if(graph.nodes.size() < 1) break;
      graph.nodes.clear();
      break;
  }
}

void ofApp::create_nodes_and_links() {
  for(std::size_t i = 0; i < 25; ++i) {
    graph.add_node(
      gui.node_count + i,
      ofVec2f{
        ofRandom(-START_DIST_MULTI*ofGetWidth(), START_DIST_MULTI*ofGetWidth()),
        ofRandom(-START_DIST_MULTI*ofGetHeight(), START_DIST_MULTI*ofGetHeight())
      },
      gui.radius,
      std::to_string(gui.node_count + i)
    );
  }
  for(std::size_t i = gui.node_count; i < graph.nodes.size(); ++i) {
    for(std::size_t j = 0; j < ofRandom(0, 10); ++j) {
      std::size_t random_idx;
      
      if(ofRandom(0, 1) > 0.9) {
        random_idx = static_cast<int>(ofRandom(0, graph.nodes.size())); 
      } else {
        const int upper_bound = std::clamp(i * 1.15f, 0.0f, (float)graph.nodes.size());
        const int lower_bound = std::clamp(i * 0.85f, 0.0f, (float)graph.nodes.size());
        random_idx = static_cast<int>(ofRandom(static_cast<int>(lower_bound), static_cast<int>(upper_bound)));
      }

      if(random_idx == i) continue;

      graph.add_edge(i, random_idx);
      gui.link_count+=2;
    }
  }
  gui.node_count = graph.nodes.size();
};

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

void ofApp::update_mouse_position() {
  prev_mouse_position = mouse_position;
  mouse_position = {ofGetMouseX() - ofGetWidth() * 0.5f, ofGetMouseY() - ofGetHeight() * 0.5f};
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

void ofApp::pan() {
  update_mouse_position();
  const ofVec2f PAN_OFFSET =  mouse_position - prev_mouse_position;
  for(const std::unique_ptr<Node>& node : graph.nodes) {
    node->pos += PAN_OFFSET;
  };
}

void ofApp::find_node_being_dragged() {
  update_mouse_position();
  for(std::size_t i = 0; i < graph.nodes.size(); ++i) {
    if(graph.nodes[i]->pos.distance(mouse_position) >= graph.nodes[i]->radius) continue;
    node_being_dragged_idx = i;
    break;
  }
}

void ofApp::drag() {
  update_mouse_position();
  graph.nodes[node_being_dragged_idx]->pos.interpolate(mouse_position, lerp_val);
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
    float min_dist = mouse_position.distance(graph.nodes[0]->pos);
    std::size_t closest_node = 0;

    for(std::size_t i = 1; i < graph.nodes.size(); ++i) {
      const float new_dist = mouse_position.distance(graph.nodes[i]->pos);
      if(new_dist < min_dist) {
        min_dist = new_dist;
        closest_node = i;
      }
    } 

    graph.nodes.emplace_back(std::make_unique<Node>(
      graph.nodes.size(), 
      ofVec2f{mouse_position}, 
      gui.radius, 
      std::to_string(graph.nodes.size())
    ));
    if(graph.nodes.size() < 2) return;
    graph.nodes[graph.nodes.size()-1]->edges.push_back(closest_node);
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
  gui.force = std::clamp(scrollY > 0 ? gui.force *= 0.9f : gui.force *= 1.1f, 1.0f, 2500.0f);
  gui.radius = std::clamp(scrollY > 0 ? gui.radius *= 0.95f : gui.radius *= 1.05f, 1.0f, 15.0f);
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