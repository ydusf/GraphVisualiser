#include "ofApp.h"

#include <cstddef>

//--------------------------------------------------------------
void ofApp::setup(){
  ofBackground(ofColor::black);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
  for(const auto& node : nodes) {
    ofDrawCircle(
      node.x,
      node.y,
      radius
    );
    for(const auto& next_node : nodes) {
      if(node == next_node) continue; 
      ofDrawLine(node.x, node.y, next_node.x, next_node.y);
    }
  }


}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  switch (key) {
    case 'd':
      for(std::size_t i = 0; i < 3; ++i) {
        nodes.push_back({
          ofRandom(radius, ofGetWidth()),
          ofRandom(radius, ofGetHeight()),
        });
      }
      break;
  }
} 

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
