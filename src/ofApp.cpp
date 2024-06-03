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

  gui.setup();
  gui.add(force_multi_slider.setup("Force Multiplier", 500.0f, 1.0f, 3000.0f));
  gui.add(radius_slider.setup("Node Radius", 4.0f, 4.0f, 32.0f));
  gui.add(node_count_label.setup("Node Count", std::to_string(nodes.size())));
  gui.add(link_count_label.setup("Link Count", std::to_string(links.size())));
  gui.add(node_color_label.setup("Node", "rgb(" + std::to_string(node_color.r) + ", " + std::to_string(node_color.g) + ", " + std::to_string(node_color.b) + ")"));
  gui.add(node_color_slider.setup(node_color, 20, 20));
  gui.add(link_color_label.setup("Link", "rgb(" + std::to_string(link_color.r) + ", " + std::to_string(link_color.g) + ", " + std::to_string(link_color.b) + ")"));
  gui.add(link_color_slider.setup(link_color, 20, 20));
  gui.add(label_color_label.setup("Label", "rgb(" + std::to_string(label_color.r) + ", " + std::to_string(label_color.g) + ", " + std::to_string(label_color.b) + ")"));
  gui.add(label_color_slider.setup(label_color, 20, 20));
}

void ofApp::apply_forces() {
  // gravity
  for (auto& node : nodes) {
    node->vel = node->pos * -1 * GRAVITY;
  }

  // node-node repulsion
  for(auto& curr_node : nodes) {
    for(auto& next_node : nodes) {
      const ofVec2f dir = next_node->pos - curr_node->pos;
      const ofVec2f force = dir / (dir.lengthSquared()) * force_multi;
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
  // gui updates
  ofSetWindowTitle("FPS: " + std::to_string(ofGetFrameRate()) + " | Nodes: " + std::to_string(nodes.size()) + " | Links: " + std::to_string(links.size()));
  force_multi = force_multi_slider;
  radius = radius_slider;
  node_color = node_color_slider;
  link_color = link_color_slider;
  label_color = label_color_slider;
  node_count_label.setup("Node Count", std::to_string(nodes.size()));
  link_count_label.setup("Link Count", std::to_string(links.size()));
  node_color_label.setup("Node", "rgb(" + std::to_string(node_color.r) + ", " + std::to_string(node_color.g) + ", " + std::to_string(node_color.b) + ")");
  link_color_label.setup("Link", "rgb(" + std::to_string(link_color.r) + ", " + std::to_string(link_color.g) + ", " + std::to_string(link_color.b) + ")");
  label_color_label.setup("Label", "rgb(" + std::to_string(label_color.r) + ", " + std::to_string(label_color.g) + ", " + std::to_string(label_color.b) + ")");
  
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
  if(prev_link_color != link_color) {
    prev_link_color = link_color;
    for(const auto& link : links) {
      link->color = link_color;
    }
  }

  // graph updates
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
  // gui draw
  gui.draw();

  // graph draw (links first so nodes are above)
  ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
  for(const auto& link : links) {
    link->draw();
  }
  for (const auto& node : nodes) {
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
      for(std::size_t i = 0; i < 400; ++i) {
        const auto& new_node = std::make_shared<Node>(
          ofVec2f{
            ofRandom(-START_DIST_MULTI*ofGetWidth() , START_DIST_MULTI*ofGetWidth()),
            ofRandom(-START_DIST_MULTI*ofGetHeight() , START_DIST_MULTI*ofGetHeight())
          }, radius, ofColor{52.0f, 152.0f, 219.0f}, std::to_string(i));
        nodes.push_back(new_node);
      }
      for(std::size_t i = 0; i < nodes.size(); ++i) {
        for(std::size_t j = 0; j < 3; ++j) {
          
          const int random_idx = static_cast<int>(ofRandom(0, nodes.size()));
          if(i == random_idx) continue;
          links.push_back(std::make_shared<Link>(
            nodes[i], nodes[random_idx],
            ofColor{44, 62, 80}
          ));
        }
      }
      break;
    case 'r':
      if(nodes.size() < 1) break;
      nodes.clear();
      links.clear();
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

  // node still being dragged
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
  const float MIN_FORCE_MULTI = 200.0f, MAX_FORCE_MULTI = 2000.0f;
  scrollY = std::clamp(scrollY, -1.0f, 1.0f);
  force_multi = std::clamp(force_multi - (scrollY * 1000.0f), MIN_FORCE_MULTI, MAX_FORCE_MULTI);
  for(auto& node : nodes) {
    const float MIN_RADIUS = 2.0f, MAX_RADIUS = 16.0f;
    node->radius = std::clamp(glm::abs(node->radius - (scrollY * 0.1f)), MIN_RADIUS, MAX_RADIUS);
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
