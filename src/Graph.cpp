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
};

void Link::draw() {
  if(start.expired() || end.expired()) return;
  ofSetColor(color);
  ofDrawLine(start.lock()->pos, end.lock()->pos);
};

Link::~Link() {};
