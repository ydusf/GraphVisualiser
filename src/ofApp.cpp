#include "ofApp.h"

#include <cstddef>
#include <iostream>
#include <unordered_set>

//--------------------------------------------------------------
void ofApp::setup(){
  ofBackground(ofColor::black);
  ofEnableSmoothing();
  ofSetFrameRate(0);
  ofSetVerticalSync(0);

  create_gui();
}

void ofApp::create_gui() {
  gui.setup();
  gui.add(force_multi_slider.setup("Force Multiplier", 500.0f, MIN_FORCE_MULTI, MAX_FORCE_MULTI));
  gui.add(radius_slider.setup("Node Radius", 4.0f, MIN_RADIUS, MAX_RADIUS));
  add_color_slider(gui, node_color, "Node", node_color_label, node_color_slider);
  add_color_slider(gui, link_color, "Link", link_color_label, link_color_slider);
  add_color_slider(gui, label_color, "Label", label_color_label, label_color_slider);
}

void ofApp::add_color_slider(ofxPanel& gui, ofColor& color, const std::string& label, ofxLabel& color_label, ofxColorSlider& color_slider) {
  color_label.setup(label, "rgb(" + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b) + ")");
  gui.add(color_slider.setup(color, 20, 20));
}

void ofApp::update_gui() {
  std::size_t link_num = 0;
  for(const auto& node : nodes) {
    for(const auto& neighbour_idx : node->neighbours) {
      link_num++;
    }
  } 
  ofSetWindowTitle("FPS: " + std::to_string(ofGetFrameRate()) + " | Nodes: " + std::to_string(nodes.size()) + " | Links: " + std::to_string(link_num));
  force_multi = force_multi_slider;
  radius = radius_slider;
  update_color(node_color, node_color_slider, node_color_label);
  update_color(link_color, link_color_slider, link_color_label);
  update_color(label_color, label_color_slider, label_color_label);

  if(radius != prev_radius) {
    prev_radius = radius;
    for(const auto& node : nodes) {
      node->radius = radius;
    }
  }
  if(prev_node_color != node_color || prev_label_color != label_color) {
    prev_node_color = node_color;
    prev_label_color = label_color;
    for(const auto& node : nodes) {
      node->node_color = node_color;
      node->label_color = label_color;
    }
  }
}

void ofApp::update_color(ofColor& color, ofxColorSlider& color_slider, ofxLabel& color_label) {
  color = color_slider;
  color_label.setup("rgb(" + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b) + ")");
}

void ofApp::apply_force_directed_layout() {

  for(const auto& curr_node : nodes) {
    // gravity
    curr_node->vel = curr_node->pos * -1 * GRAVITY;
    for(const auto& next_node : nodes) {
      // node-node repulsion
      const ofVec2f dir = next_node->pos - curr_node->pos;
      const ofVec2f force = dir / (dir.lengthSquared()) * force_multi;
      curr_node->vel -= force;
      next_node->vel += force; 
    }
  }

  // link forces
  for(const auto& node : nodes) {
    for(const auto& neighbour : node->neighbours) {
      const ofVec2f dis = node->pos - neighbour->pos;
      node->vel -= dis;
      neighbour->vel += dis;
    }
  }
}

//--------------------------------------------------------------
void ofApp::update(){
  update_gui();  

  // graph updates
  apply_force_directed_layout();
  for (const auto& node : nodes) {
    node->update();
  }

  // user interaction
  if(node_being_dragged) drag();
  if(panning) pan();
}

//--------------------------------------------------------------
void ofApp::draw(){  
  gui.draw();

  // centralise visualisation
  ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

  for(const auto& node : nodes) {
    ofSetColor(link_color);
    node->draw_links();
    node->draw();
  }
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
  for(std::size_t i = 0; i < 400; ++i) {
    const auto& new_node = std::make_shared<Node>(
      i,
      ofVec2f{
        ofRandom(-START_DIST_MULTI*ofGetWidth() , START_DIST_MULTI*ofGetWidth()),
        ofRandom(-START_DIST_MULTI*ofGetHeight() , START_DIST_MULTI*ofGetHeight())
      }, radius, ofColor{52.0f, 152.0f, 219.0f}, std::to_string(i));
    nodes.push_back(new_node);
  }
  for(std::size_t i = 0; i < nodes.size(); ++i) {
    for(std::size_t j = 0; j < 3; ++j) {
      const std::size_t random_idx = static_cast<int>(ofRandom(0, nodes.size()));
      if(i == random_idx) continue;
      nodes[i]->neighbours.push_back(nodes[random_idx]);
    }
  }
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
  for(const auto& node : nodes) {
    if(node->pos.distance(mouse_position) >= node->radius) continue;
    node_being_dragged = node;
    break;
  }
}

void ofApp::drag() {
  update_mouse_position();
  node_being_dragged->pos.interpolate(mouse_position, lerp_val);
  if(lerp_val < 0.95) lerp_val += 0.02;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){  
  if(node_being_dragged) return;

  find_node_being_dragged();

  // node not found so must be panning
  if(!node_being_dragged) panning = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
  // reset mouse-related variables
  panning = false;
  node_being_dragged = nullptr;
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){
  // // zoom functionality
  // scrollY = std::clamp(scrollY, -1.0f, 1.0f);
  // force_multi = std::clamp(force_multi - (scrollY * 1000.0f), MIN_FORCE_MULTI, MAX_FORCE_MULTI);
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