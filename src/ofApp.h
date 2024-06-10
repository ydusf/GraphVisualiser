#pragma once

#include "ofMain.h"
#include "Graph.h"
#include "ofxGui.h"

#include <vector>
#include <memory>
#include <tuple>
#include <cstddef>
#include <thread>

struct GridHash {
  std::size_t operator()(const std::pair<std::size_t, std::size_t>& key) const {
		return std::hash<std::size_t>()(key.first) ^ (std::hash<std::size_t>()(key.second) << 1);
	}
};

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

		void create_gui();
		void add_color_slider(ofxPanel& gui, const ofColor& color, const std::string& label, ofxLabel& color_label, ofxColorSlider& color_slider);
		void update_gui();
		void update_color(ofColor& color, ofxColorSlider& color_slider, ofxLabel& color_label);

		std::pair<std::size_t, std::size_t> get_grid_cell(const ofVec2f& pos);
		void populate_grid();

		void apply_gravity(std::vector<std::size_t>& cell);
		void apply_node_repulsion(std::vector<std::size_t>& cell);
		void apply_link_forces(std::vector<std::size_t>& cell);
		void apply_force_directed_layout(std::vector<std::size_t>& cell);
		void apply_force_directed_layout_multithreaded();
		
		void update_mouse_position();
		void find_node_being_dragged();
		void drag();
		void pan();

		void create_nodes_and_links();

		void create_meshes(std::size_t from, std::size_t to);
		void create_circle(ofVboMesh &mesh, const std::unique_ptr<Node>& node, std::size_t resolution);
		void create_line(ofVboMesh &mesh, const std::unique_ptr<Node>& node1, const std::unique_ptr<Node>& node2);

	private:
		const float num_threads = std::thread::hardware_concurrency();
		std::unordered_map<
			std::pair<std::size_t, std::size_t>,
			std::vector<std::size_t>, GridHash
		> grid;

		ofVec2f mouse_position;
		ofVec2f prev_mouse_position;
		int prev_node_count;
		bool panning = false;

		std::vector<std::unique_ptr<Node> > nodes;
		int node_being_dragged_idx = -1;

		const float GRAVITY = 1.1f;
		const float START_DIST_MULTI = 1.0f;
		const float MIN_RADIUS = 1.0f, MAX_RADIUS = 15.0f;
		const float MIN_FORCE_MULTI = 10.0f, MAX_FORCE_MULTI = 200000.0f;
		float force_multi = 1000.0f;
		float lerp_val = 0.2f;

		float radius = 4.0f;
		float prev_radius = radius;
		std::size_t link_count = 0;

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

		ofColor node_color;
		ofColor prev_node_color = node_color;
		ofColor link_color = {255.0f, 255.0f, 255.0f, 65.0f};
		ofColor label_color;
		ofColor prev_label_color = label_color;

		ofVboMesh circle_mesh;
		ofVboMesh line_mesh;
		float circle_resolution;
};