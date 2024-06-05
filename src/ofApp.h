#pragma once

#include "ofMain.h"
#include "Graph.h"
#include "ofxGui.h"

#include <vector>
#include <memory>
#include <tuple>
#include <unordered_set>

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;

		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;
		void update_gui();
		void apply_forces();
		void update_mouse_position();
		void pan();
		void find_node_being_dragged();
		void drag();
	
	ofVec2f mouse_position;
	ofVec2f prev_mouse_position;
	bool panning = false;
	std::vector<std::shared_ptr<Node> > nodes;
	std::vector<std::shared_ptr<Link> > links;
	std::shared_ptr<Node> node_being_dragged;

	const float GRAVITY = 1.1f;
	const float START_DIST_MULTI = 1.0f;
	const float MIN_RADIUS = 2.0f, MAX_RADIUS = 32.0f;
	const float MIN_FORCE_MULTI = 200.0f, MAX_FORCE_MULTI = 2000.0f;
	float force_multi = 1000.0f;
	float lerp_val = 0.2f;

	float radius = 4.0f;
	float prev_radius = radius;
	ofColor node_color;
	ofColor prev_node_color = node_color;
	ofColor link_color;
	ofColor prev_link_color = link_color;
	ofColor label_color;
	ofColor prev_label_color = label_color;


	ofxPanel gui;

	ofxFloatSlider force_multi_slider;
	ofxFloatSlider radius_slider;
	ofxColorSlider node_color_slider;
	ofxColorSlider link_color_slider;
	ofxColorSlider label_color_slider;

	ofxLabel node_count_label;
	ofxLabel link_count_label;
	ofxLabel node_color_label;
	ofxLabel link_color_label;
	ofxLabel label_color_label;
};