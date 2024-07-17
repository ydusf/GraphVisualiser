#include "../headers/Node.h"

#include <string>
#include <memory>
#include <cstddef>

namespace {
  const ofVec2f g_initial_velocity{1.0f, 1.0f};
  constexpr int g_size_of_char = 8;
}

Node::Node(std::size_t id, const ofVec2f& pos, float radius, std::string label)
  : pos(pos), vel(g_initial_velocity),
  radius(radius), label(label), m_id(id) 
{};

bool Node::operator==(const Node& node) const {
  return m_id == node.m_id;
};

bool Node::within_bounds() const {
  const ofVec2f bounds = {ofGetWidth() * 0.5f, ofGetHeight() * 0.5f};
  return (pos.x+radius) > -bounds.x && (pos.x-radius) < bounds.x && (pos.y+radius) > -bounds.y && (pos.y-radius) < bounds.y;
};

void Node::update() {
  pos += vel * ofGetLastFrameTime();
};

void Node::draw_label(const ofColor& label_color) const {
  if(!within_bounds()) return;
  ofSetColor(label_color);
  const int LABEL_OFFSET_X = (label.length() * g_size_of_char) * 0.5f;
  const int LABEL_OFFSET_Y = radius * 1.5;
  ofDrawBitmapString(label, pos.x-LABEL_OFFSET_X, pos.y-LABEL_OFFSET_Y);
};

std::size_t Node::get_id() const {
  return m_id;
};  