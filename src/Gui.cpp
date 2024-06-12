#include "../headers/Gui.h"

#include <string>
#include <cstddef>

const float START_RADIUS = 4.0f, MIN_RADIUS = 1.0f, MAX_RADIUS = 15.0f;
const float START_FORCE = 1000.0f, MIN_FORCE = 10.0f, MAX_FORCE = 200000.0f;

Gui::Gui() : force(START_FORCE), radius(START_RADIUS), link_count(0), node_count(0) {};

void Gui::setup() {
  m_panel.setup();
  m_panel.add(m_force_slider.setup("Force Multiplier", force, MIN_FORCE, MAX_FORCE));
  m_panel.add(m_radius_slider.setup("Node Radius", radius, MIN_RADIUS, MAX_RADIUS));
  add_color_slider(node_color, "Node", m_node_color_label, m_node_color_slider);
  add_color_slider(link_color, "Link", m_link_color_label, m_link_color_slider);
  add_color_slider(label_color, "Label", m_label_color_label, m_label_color_slider);
  m_force_slider.addListener(this, &Gui::force_slider_changed);
  m_radius_slider.addListener(this, &Gui::radius_slider_changed);
}

void Gui::add_color_slider(const ofColor& color, const std::string& label, ofxLabel& color_label, ofxColorSlider& color_slider) {
  color_label.setup(label, "rgb(" + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b) + ")");
  m_panel.add(color_slider.setup(color, 20, 20));
}

void Gui::update_color(ofColor& color, ofxColorSlider& color_slider, ofxLabel& color_label) {
  color = color_slider;
  color_label.setup("rgb(" + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b) + ")");
}

void Gui::radius_slider_changed(float & new_radius) { radius = new_radius; }

void Gui::force_slider_changed(float & new_force) { force = new_force; }

void Gui::update() {
  ofSetWindowTitle("FPS: " + std::to_string(ofGetFrameRate()) + " | Nodes: " + std::to_string(node_count) + " | Links: " + std::to_string(link_count));
  update_color(node_color, m_node_color_slider, m_node_color_label);
  update_color(link_color, m_link_color_slider, m_link_color_label);
  update_color(label_color, m_label_color_slider, m_label_color_label);
}

void Gui::draw() {
  m_panel.draw();  
}