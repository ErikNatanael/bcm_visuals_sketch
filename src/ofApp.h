#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

#include "StatObject.h"
#include "PointCloud.h"

// listening port
#define PORT 9771

enum struct VisData :uint8_t {SERVICE, LOCATION};
enum struct VisType :uint8_t {TOGETHER, CHRONOLOGICALLY, LIVE};

struct Packet {
	size_t location_index;
	size_t service_index;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void handleIncomingOsc();
		void reset_packets();
		void cycle_active();

		ofxOscReceiver receiver;
		std::map<std::string, shared_ptr<StatObject>> serviceMap;
		vector<string> services;
		vector<size_t> service_packets;
		std::map<std::string, shared_ptr<StatObject>> locationMap;
		vector<string> locations;
		vector<size_t> location_packets;
		vector<glm::vec3> location_3dpositions;
		vector<Packet> packets;
		vector<PointCloud> service_point_clouds;

		VisData visdata = VisData::SERVICE;
		VisType vistype = VisType::CHRONOLOGICALLY;

		ofTrueTypeFont font;

		float lastToggle = 0.0;
		float lastUpdate = 0.0;
		weak_ptr<StatObject> active_obj_ptr;
		size_t lastActive = 0;
		string target_name = "";
		string current_name = "";

		bool do_filter_services = false;

		int num_lines_required = 1;

		float rectangle_size = 80;
		float rectangle_width = 5;

		float margin_bottom = 50;

		ofEasyCam cam;

		ofFbo mainFbo;
};
