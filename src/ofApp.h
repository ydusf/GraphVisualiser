#pragma once

#include "ofMain.h"
#include "Graph.h"
#include "Gui.h"

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

		void create_graph(std::string filename);

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
		Gui gui;

		std::vector<std::unique_ptr<Node> > nodes;

		std::unordered_map<
			std::pair<std::size_t, std::size_t>,
			std::vector<std::size_t>, GridHash
		> grid;

		const float GRAVITY = 1.1f;
		const float START_DIST_MULTI = 1.0f;

		ofVec2f mouse_position;
		ofVec2f prev_mouse_position;
		bool panning = false;
		int node_being_dragged_idx = -1;
		float lerp_val = 0.2f;

		ofVboMesh circle_mesh;
		ofVboMesh line_mesh;
		float circle_resolution;
};