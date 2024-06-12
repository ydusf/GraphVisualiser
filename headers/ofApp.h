#pragma once

#include "ofMain.h"
#include "Gui.h"
#include "Graph.h"

#include <vector>
#include <memory>
#include <cstddef>

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

	void create_graph(std::string filename);
	
	void update_mouse_position();
	void find_node_being_dragged();
	void drag();
	void pan();

	void create_nodes_and_links();

private:
	Gui gui;
	Graph graph;
	
	const float START_DIST_MULTI = 1.0f;

	ofVec2f mouse_position;
	ofVec2f prev_mouse_position;
	bool panning = false;
	int node_being_dragged_idx = -1;
	float lerp_val = 0.2f;
};