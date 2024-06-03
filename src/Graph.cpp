#include "ofApp.h"
#include "Graph.h"

#include <string>

const ofVec2f INITIAL_VELOCITY = {1.0f, 1.0f};
const int SIZE_OF_CHAR = 8;

Node::Node(ofVec2f pos, float radius)
  : pos(pos), vel(INITIAL_VELOCITY), radius(radius) {};

Node::Node(ofVec2f pos, float radius, ofColor color)
  : pos(pos), vel(INITIAL_VELOCITY), radius(radius), color(color) {};

Node::Node(ofVec2f pos, float radius, ofColor color, std::string label)
  : pos(pos), vel(INITIAL_VELOCITY), radius(radius), color(color), label(label) {};

bool Node::operator==(const std::shared_ptr<Node>& node) {
  return pos == pos && vel == vel;
};

void Node::update() {
  pos += vel * ofGetLastFrameTime();
};

void Node::draw() {
  // draw nodes
  ofSetColor(color);
  ofDrawCircle(
    pos,
    radius
  );
  // draw text
  ofSetColor(ofColor{241.0f, 196.0f, 15.0f});
  const int LABEL_OFFSET_X = (label.length() * SIZE_OF_CHAR) / 2;
  const int LABEL_OFFSET_Y = radius * 1.5;
  ofDrawBitmapString(label, pos.x - LABEL_OFFSET_X, pos.y-LABEL_OFFSET_Y);
};
  
Node::~Node() {
  
};

Link::Link(std::shared_ptr<Node> start, std::shared_ptr<Node> end)
  : start(start), end(end) {};

Link::Link(std::shared_ptr<Node> start, std::shared_ptr<Node> end, ofColor color)
  : start(start), end(end), color(color) {};

void Link::update() {
  if(start.expired() || end.expired()) return;
};

void Link::draw() {
  if(start.expired() || end.expired()) return; // link validity check
  ofSetColor(color, 255.0f);
  ofDrawLine(start.lock()->pos, end.lock()->pos);
};

Link::~Link() {};
