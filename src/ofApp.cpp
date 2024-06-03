#include "ofApp.h"

#include <cstddef>
#include <iostream>
#include <unordered_set>

//--------------------------------------------------------------
void ofApp::setup(){
  ofBackground(ofColor::black);
  ofEnableSmoothing();
  ofSetFrameRate(60);
  ofSetVerticalSync(0);
}

void ofApp::apply_forces() {
  // gravity
  for (auto& node : nodes) {
    ofVec2f gravity = node->pos * -1 * GRAVITY;
    node->vel = gravity;
  }

  // node-node repulsion
  for(auto& curr_node : nodes) {
    for(auto& next_node : nodes) {
      const ofVec2f dir = next_node->pos - curr_node->pos;
      const ofVec2f force = dir / (dir.lengthSquared()) * FORCE_MULTI;
      curr_node->vel -= force;
      next_node->vel += force;
    }
  }

  // link forces
  for (const auto& link : links) {
    const ofVec2f dis = link->start.lock()->pos - link->end.lock()->pos;
    link->start.lock()->vel -= dis;
    link->end.lock()->vel += dis;
  }
}

//--------------------------------------------------------------
void ofApp::update(){
  ofSetWindowTitle("FPS: " + std::to_string(ofGetFrameRate()) + " | Nodes: " + std::to_string(nodes.size()) + " | Links: " + std::to_string(links.size()));
  apply_forces();
  for (const auto& node : nodes) {
    node->update();
  }
  for(const auto& link : links) {
    link->update();
  }

  // move position of node being dragged
  if(node_being_dragged == nullptr) return;
  node_being_dragged->pos.interpolate(mouse_position, lerp_val);
  if(lerp_val < 0.95) lerp_val += 0.02;
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
  for (const auto& node : nodes) {
    node->draw();
  }
  for(const auto& link : links) {
    link->draw();
  }
};

//--------------------------------------------------------------
void ofApp::exit(){
  std::cout << "Graph Visualisation Closed..." << '\n';
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  const int radius = 8.0f;
  switch (key) {
    case 'd':
      for(std::size_t i = 0; i < 25; ++i) {
        auto new_node = std::make_shared<Node>(
          ofVec2f{
            ofRandom(radius, ofGetWidth()-radius),
            ofRandom(radius, ofGetHeight()/2)
          }, radius, ofColor{147.0f,206.0f,195.0f});
        nodes.push_back(new_node);
      }
      break;
    case 'l':
      for(std::size_t i = 0; i < nodes.size(); ++i) {
        const int random_idx = static_cast<int>(ofRandom(0, nodes.size()));
        if(i == random_idx) continue;
        links.push_back(std::make_shared<Link>(nodes[i], nodes[random_idx]));
      }
      break;
    case 'r':
      if(nodes.size() < 1) break;
      nodes.erase(nodes.begin());
      break;
  }
} 

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
  mouse_position = {ofGetMouseX() - ofGetWidth() / 2.0f, ofGetMouseY() - ofGetHeight() / 2.0f};

  if(node_being_dragged != nullptr && node_being_dragged->pos.distance(mouse_position) < node_being_dragged->radius) return;

  for(const auto& node : nodes) {
    if(node->pos.distance(mouse_position) < node->radius) {
      node_being_dragged = node;
      break;
    }
  }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
  node_being_dragged = nullptr;
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){
  // zoom functionality
  for(auto& node : nodes) {
    const float min_radius = 2.0f, max_radius = 16.0f;
    node->radius = std::clamp(glm::abs(node->radius - (scrollY * 0.1f)), min_radius, max_radius);
    FORCE_MULTI = std::clamp(FORCE_MULTI - scrollY, 10.0f, 5000.0f);
  }
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
