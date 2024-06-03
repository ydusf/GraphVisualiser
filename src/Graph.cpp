#include "ofApp.h"
#include "Graph.h"

const ofVec2f INITIAL_VELOCITY = {1.0f, 1.0f};
const float CELL_SIZE = 24.0f;

Node::Node(ofVec2f pos, float radius)
  : pos(pos), vel(INITIAL_VELOCITY), radius(radius) {};

Node::Node(ofVec2f pos, float radius, ofColor color)
  : pos(pos), vel(INITIAL_VELOCITY), radius(radius), color(color) {};

bool Node::operator==(const std::shared_ptr<Node>& node) {
  return pos == pos && vel == vel;
};

void Node::update() {
  pos += vel * ofGetLastFrameTime();
};

void Node::draw() {
  // if(ofRandom(0, 1) > 0.98) {
  //   color = ofColor(ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255));
  // };
  ofSetColor(color);
  ofDrawCircle(
    pos,
    radius
  );
};

void Node::update_grid_cell() {
  get<0>(grid_cell) = static_cast<int>(pos.x / CELL_SIZE);
  get<1>(grid_cell) = static_cast<int>(pos.y / CELL_SIZE);
};

std::tuple<int, int> Node::get_grid_cell() {
  return grid_cell;
};

void Node::update_neighbour_cells() {
  neighbour_cells.clear();
  auto& [x, y] = grid_cell;
  for(int dx = -1; dx <= 1; ++dx) {
    for(int dy = -1; dy <= 1; ++dy) {
      if(dx == 0 && dy == 0) continue; // skip current cell
      neighbour_cells.emplace_back(std::make_tuple(x + dx, y + dy));
    }
  }
};
  
Node::~Node() {
  
};

Link::Link(std::shared_ptr<Node> start, std::shared_ptr<Node> end)
  : start(start), end(end) {};

Link::Link(std::shared_ptr<Node> start, std::shared_ptr<Node> end, float max_distance)
  : start(start), end(end), max_distance(max_distance) {};


Link::Link(std::shared_ptr<Node> start, std::shared_ptr<Node> end, float width, ofColor color)
  : start(start), end(end), width(width), color(color) {};

void Link::update() {
  if(start.expired() || end.expired()) return;
  // const float defined_distance = 20.0f;

  // const ofVec2f dir = end.lock()->pos - start.lock()->pos;
  // const float actual_distance = start.lock()->pos.distance(end.lock()->pos);
  // const float diff = actual_distance - defined_distance;
  // const float k = 1.0f;
  // start.lock()->vel -= k * diff * (dir / actual_distance) * ofGetLastFrameTime();
  // end.lock()->vel += k * diff * (dir / actual_distance) * ofGetLastFrameTime();
};

void Link::draw() {
  if(start.expired() || end.expired()) return;
  ofSetColor(color);
  ofDrawLine(start.lock()->pos, end.lock()->pos);
};

Link::~Link() {};
