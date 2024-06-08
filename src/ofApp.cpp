#include "ofApp.h"

#include <cstddef>
#include <iostream>
#include <unordered_set>
#include <thread>

//--------------------------------------------------------------
void ofApp::setup(){
  ofBackground(ofColor::black);
  ofEnableSmoothing();
  ofSetFrameRate(0);
  ofSetVerticalSync(0);
  line_mesh.setMode(OF_PRIMITIVE_LINES);
  line_mesh.enableColors();
  circle_mesh.setMode(OF_PRIMITIVE_TRIANGLES);
  circle_mesh.enableIndices();
  circle_mesh.enableColors();

  create_gui();
}

void ofApp::create_circle(ofVboMesh& mesh, const std::shared_ptr<Node>& node, std::size_t resolution) {
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
    mesh.addColor(node->node_color);
    mesh.addVertex(ofVec3f{vx1, vy1, 0.0f});
    mesh.addColor(node->node_color);
    mesh.addVertex(ofVec3f{vx2, vy2, 0.0f});
    mesh.addColor(node->node_color);
  }

  // const std::size_t strt_vrtx = mesh.getNumVertices();

  // mesh.addVertex(CENTRE);

  // for(std::size_t i = 1; i < resolution; ++i) {
  //   const float vx = CENTRE.x + node->radius * cos(ANGLE_INCREMENT * i);
  //   const float vy = CENTRE.y + node->radius * sin(ANGLE_INCREMENT * i);
  //   mesh.addVertex(ofVec3f{vx, vy, CENTRE.z});
  //   mesh.addColor(node->node_color);
  // }
  
  // for(std::size_t i = strt_vrtx; i < strt_vrtx + resolution; ++i) {
  //   const std::size_t scnd_idx = i+1;
  //   const std::size_t thrd_idx = (i+scnd_idx) % resolution;
  //   mesh.addIndex(strt_vrtx);
  //   mesh.addIndex(scnd_idx);
  //   mesh.addIndex(thrd_idx);
  // }
}

void ofApp::create_line(ofVboMesh &mesh, const std::shared_ptr<Node>& node1, const std::shared_ptr<Node>& node2) {
  if(!node1->within_bounds() && !node2->within_bounds()) return;
  mesh.addVertex(ofVec3f(node1->pos.x, node1->pos.y, 0.0f));
  mesh.addColor(link_color);
  mesh.addVertex(ofVec3f(node2->pos.x, node2->pos.y, 0.0f));
  mesh.addColor(link_color);
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
  ofSetWindowTitle("FPS: " + std::to_string(ofGetFrameRate()) + " | Nodes: " + std::to_string(nodes.size()) + " | Links: " + std::to_string(link_count));
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

void ofApp::apply_force_directed_layout(std::size_t from, std::size_t to) {
  for(std::size_t i = from; i < to; ++i) {
    // gravity
    nodes[i]->vel = nodes[i]->pos * -1 * GRAVITY;
    for(std::size_t j = from; j < to; ++j) {
      // node-node repulsion
      const ofVec2f dir = nodes[j]->pos - nodes[i]->pos;
      const ofVec2f force = dir / (dir.lengthSquared()) * force_multi;
      nodes[i]->vel -= force;
      nodes[j]->vel += force; 
    }
  }

  // link forces
  for(std::size_t i = from; i < to; ++i) {
    for(const auto& neighbour : nodes[i]->neighbours) {
      const ofVec2f dist = nodes[i]->pos - neighbour->pos;
      nodes[i]->vel -= dist;
      neighbour->vel += dist;
    }
  }
}

void ofApp::apply_force_directed_layout_multithreaded() {
  const std::size_t num_threads = std::thread::hardware_concurrency();
  std::vector<std::pair<std::size_t, std::size_t> > sections;
  const std::size_t section_size = nodes.size() / num_threads;

  for (std::size_t i = 0; i < num_threads; ++i) {
    std::size_t start = i * section_size;
    std::size_t end = (i == num_threads - 1) ? nodes.size() : (i + 1) * section_size;
    sections.push_back({start, end});
  }

  std::vector<std::thread> threads;

  for (const auto& section : sections) {
    threads.emplace_back(&ofApp::apply_force_directed_layout, this, section.first, section.second);
  }

  for (auto& thread : threads) {
    thread.join();
  }
}

//--------------------------------------------------------------
void ofApp::update(){
  update_gui();  

  // graph updates
  apply_force_directed_layout_multithreaded();
  // apply_force_directed_layout(0, nodes.size());
  for (const auto& node : nodes) {
    node->update();
  }

  circle_resolution = std::clamp(radius*1.5f, 5.0f, 25.0f);

  // user interaction
  if(node_being_dragged) drag();
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

  // generate graph
  for(const auto& node : nodes) {
    for(const auto& next_node : node->neighbours) {
      create_line(line_mesh, node, next_node);
    }
    if(!node->within_bounds()) continue;
    create_circle(circle_mesh, node, circle_resolution);
    // node->draw_label();
  }

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
  for(std::size_t i = 0; i < 100; ++i) {
    nodes.emplace_back(std::make_shared<Node>(
      i, ofVec2f{
        ofRandom(-START_DIST_MULTI*ofGetWidth() , START_DIST_MULTI*ofGetWidth()),
        ofRandom(-START_DIST_MULTI*ofGetHeight() , START_DIST_MULTI*ofGetHeight())
      }, radius, ofColor{52.0f, 152.0f, 219.0f}, std::to_string(i))
    );
  }
  for(std::size_t i = prev_node_count; i < nodes.size(); ++i) {
    for(std::size_t j = 0; j < 2; ++j) {
      std::size_t random_idx = static_cast<int>(ofRandom(0, nodes.size()));
      while(random_idx == i) {
        random_idx = static_cast<int>(ofRandom(0, nodes.size()));
      };
      nodes[i]->neighbours.push_back(nodes[random_idx]);
      link_count++;
    }
  }
  prev_node_count = nodes.size();
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
  update_mouse_position();

  if(button == OF_MOUSE_BUTTON_LEFT) {
    if(node_being_dragged) return;

    find_node_being_dragged();

    // node not found so must be panning
    if(!node_being_dragged) panning = true;
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

    const auto& new_node = std::make_shared<Node>(
      nodes.size(), ofVec2f{mouse_position}, radius,
      ofColor{52.0f, 152.0f, 219.0f}, std::to_string(nodes.size())
    );
    nodes.push_back(new_node);
    if(nodes.size() < 2) return;
    new_node->neighbours.push_back(nodes[closest_node]);
  }
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

// 62 fps | 1300 nodes | 2600 links
