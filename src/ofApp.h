#pragma once

#include "ofMain.h"
#include "Graph.h"
#include "Grid.h"

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
		void apply_forces();
	
	ofVec2f mouse_position;
	std::vector<std::shared_ptr<Node> > nodes;
	std::vector<std::shared_ptr<Link> > links;
	std::shared_ptr<Node> node_being_dragged;
	Grid grid;

	const float GRAVITY = 1.1;
	float FORCE_MULTI = 1000;
	float lerp_val = 0.2;
};
