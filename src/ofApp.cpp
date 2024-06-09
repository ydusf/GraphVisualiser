#include "ofApp.h"

#include <cstddef>
#include <iostream>
#include <unordered_map>
#include <thread>

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
  gui.add(force_multi_slider.setup("Force Multiplier", force_multi, MIN_FORCE_MULTI, MAX_FORCE_MULTI));
  gui.add(radius_slider.setup("Node Radius", radius, MIN_RADIUS, MAX_RADIUS));
  add_color_slider(gui, node_color, "Node", node_color_label, node_color_slider);
  add_color_slider(gui, link_color, "Link", link_color_label, link_color_slider);
  add_color_slider(gui, label_color, "Label", label_color_label, label_color_slider);
}

void ofApp::add_color_slider(ofxPanel& gui, const ofColor& color, const std::string& label, ofxLabel& color_label, ofxColorSlider& color_slider) {
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
    }
  }
}

void ofApp::update_color(ofColor& color, ofxColorSlider& color_slider, ofxLabel& color_label) {
  color = color_slider;
  color_label.setup("rgb(" + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b) + ")");
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
    for(const auto& neighbour : nodes[node_idx]->neighbours) {
      const ofVec2f dir = neighbour.lock()->pos - nodes[node_idx]->pos;
      const float length_squared = dir.lengthSquared();
      if(length_squared == 0 || nodes[node_idx] == neighbour.lock()) continue;
      const ofVec2f force = dir / length_squared * force_multi;

      nodes[node_idx]->vel -= force;
      neighbour.lock()->vel += force; 
    }
  }
};
void ofApp::apply_link_forces(std::vector<std::size_t>& cell) {
  for(const auto& node_idx : cell) {
    for(const auto& neighbour : nodes[node_idx]->neighbours) {
      if(neighbour.expired()) return;
      const ofVec2f dist = nodes[node_idx]->pos - neighbour.lock()->pos;

      nodes[node_idx]->vel -= dist;
      neighbour.lock()->vel += dist;
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

void ofApp::create_meshes(std::size_t from, std::size_t to) {
  for(std::size_t i = from; i < to; ++i) {
    for(const auto& next_node : nodes[i]->neighbours) {
      if(next_node.expired()) continue;
      create_line(line_mesh, nodes[i], next_node.lock());
    }
    if(!nodes[i]->within_bounds()) continue;
    create_circle(circle_mesh, nodes[i], circle_resolution);
    // nodes[i]->draw_label(label_color);
  }
}

//--------------------------------------------------------------
void ofApp::update(){
  update_gui();  

  // graph updates
  for (const auto& node : nodes) {
    node->update();
  }
  apply_force_directed_layout_multithreaded();

  circle_resolution = std::clamp(radius*1.5f, 4.0f, 25.0f);

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
    nodes.emplace_back(std::make_shared<Node>(
      i, ofVec2f{
        ofRandom(-START_DIST_MULTI*ofGetWidth() , START_DIST_MULTI*ofGetWidth()),
        ofRandom(-START_DIST_MULTI*ofGetHeight() , START_DIST_MULTI*ofGetHeight())
      }, radius, ofColor{52.0f, 152.0f, 219.0f}, std::to_string(i))
    );
  }
  for(std::size_t i = prev_node_count; i < nodes.size(); ++i) {
    for(std::size_t j = 0; j < 5; ++j) {
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

// Fix force-directed layout algorithm during multithreading - potentially using a spatial grid
// Optimise circle creation by using OF_PRIMITIVE_TRIANGLE_STRIP / OF_PRIMITIVE_TRIANGLE_FAN
// Create zoom functionality
// Optimise label drawings by creating them on a mesh and then drawing once - maybe not possible.
