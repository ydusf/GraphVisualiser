#include "Graph.h"

#include <string>
#include <memory>
#include <tuple>
#include <cstddef>

const ofVec2f INITIAL_VELOCITY = {1.0f, 1.0f};
const int SIZE_OF_CHAR = 8;

Node::Node(std::size_t id, const ofVec2f& pos, float radius, const ofColor& color, std::string label)
  : id(id), pos(pos), vel(INITIAL_VELOCITY), radius(radius), node_color(color), label(label) {};

bool Node::operator==(const std::shared_ptr<Node>& node) const {
  return id == node->id;
};

bool Node::within_bounds() const {
  const ofVec2f bounds = {ofGetWidth() / 2.0f, ofGetHeight() / 2.0f};
  return (pos.x+radius) > -bounds.x && (pos.x-radius) < bounds.x && (pos.y+radius) > -bounds.y && (pos.y-radius) < bounds.y;
};

void Node::update() {
  pos += vel * ofGetLastFrameTime();
};

void Node::draw_label(const ofColor& label_color) const {
  ofSetColor(label_color);
  const int LABEL_OFFSET_X = (label.length() * SIZE_OF_CHAR) / 2;
  const int LABEL_OFFSET_Y = radius * 1.5;
  ofDrawBitmapString(label, pos.x - LABEL_OFFSET_X, pos.y-LABEL_OFFSET_Y);
};