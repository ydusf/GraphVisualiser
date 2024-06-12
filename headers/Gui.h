#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include <cstddef>

class Gui {
public:
  float force;
  float radius;
  std::size_t link_count;
  std::size_t node_count;

  ofColor node_color;
  ofColor link_color;
  ofColor label_color;

  Gui();

  void setup();
  void update();
  void draw();

private:
  ofxPanel m_panel;
  ofxFloatSlider m_force_slider;
  ofxFloatSlider m_radius_slider;
  ofxColorSlider m_node_color_slider;
  ofxColorSlider m_link_color_slider;
  ofxColorSlider m_label_color_slider;

  ofxLabel m_node_count_label;
  ofxLabel m_link_count_label;
  ofxLabel m_node_color_label;
  ofxLabel m_link_color_label;
  ofxLabel m_label_color_label;

  void add_color_slider(const ofColor& color, const std::string& label, ofxLabel& color_label, ofxColorSlider& color_slider);
  void update_color(ofColor& color, ofxColorSlider& color_slider, ofxLabel& color_label);
  void radius_slider_changed(float & new_radius);
  void force_slider_changed(float & new_force);
};