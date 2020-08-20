#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    receiver.setup(PORT);
    font.load("SpaceMono-Regular.ttf", margin_bottom* 0.5);
    rectangle_size = ofGetHeight()/2 - 10;

    
    ofSetVerticalSync(true);

    mainFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
}

//--------------------------------------------------------------
void ofApp::update(){
    handleIncomingOsc();
    float now = ofGetElapsedTimef();
    float dt = now-lastUpdate;
    // if(now - lastToggle > 0.1) {
    //     cycle_active();
    //     lastToggle = now;
    // }

    for(int i = 0; i < service_point_clouds.size(); i++) {
        service_point_clouds[i].update(dt);
    } 
    lastUpdate = now;
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    mainFbo.begin();
    ofSetColor(0, 10);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

    ofEnableBlendMode(OF_BLENDMODE_ALPHA);

    string active_name = "~~~";

    float base_alpha = 255;

    if(vistype == VisType::TOGETHER) {
        int y = 0;
        std::map<std::string, shared_ptr<StatObject>>* map;

        if(visdata == VisData::LOCATION) {
            map = &locationMap;
        } else if (visdata == VisData::SERVICE) {
            map = &serviceMap;
        }

        float active_y;

        std::map<std::string, shared_ptr<StatObject>>::iterator it = map->begin();
        while(it != map->end()) {
            int x = 0;
            auto& stat_obj = it->second;
            // draw
            if(do_filter_services) {
                if(stat_obj->active) {
                    active_name = stat_obj->name;
                    active_y = y;
                    ofSetColor(stat_obj->color);
                } else {
                    ofSetColor(0, base_alpha);
                }
            } else {
                ofSetColor(stat_obj->color);
            }
            
            
            for(int i = 0; i < stat_obj->num_packets; i++) {
                float margin = ofClamp(rectangle_size * 0.1, 1, 10);
                ofDrawRectangle(x, y, floor(rectangle_size) -margin, floor(rectangle_size) -margin);
                x += rectangle_size;
                if(x > ofGetWidth()) {
                    y+= rectangle_size;
                    x = 0;
                }
            }

            it++;
            y += rectangle_size;
        }
        if(y + rectangle_size > (ofGetHeight()-margin_bottom)) {
            num_lines_required += 1;
            rectangle_size  = (ofGetHeight() - margin_bottom) / num_lines_required;
        }

    } else if(vistype == VisType::CHRONOLOGICALLY) {
        int y = 0;
        float x = 0;
        vector<size_t>* list;
        vector<string>* name_list;
        
        
        if(visdata == VisData::LOCATION) {
            list = &location_packets;
            name_list = &locations;
            
        } else if (visdata == VisData::SERVICE) {
            list = &service_packets;
            name_list = &services;
        }
        if(list->size() > 0) {
            active_name = name_list->at(lastActive);
            for(int i = (list->size())-1; i >= 0; i--) {
                size_t index = list->at(i);
                if(index == lastActive) {
                    ofSetColor(255, base_alpha);
                } else {
                    ofSetColor(0, base_alpha);
                }
                float margin = ofClamp(rectangle_size * 0.1, 1, 10);
                ofDrawRectangle(x, y, rectangle_width, floor(rectangle_size));
                x += rectangle_width;
                if(x > ofGetWidth()) {
                    y+= rectangle_size * 1.01;
                    x = 0;
                }
            }
        }
        if(y + rectangle_size > (ofGetHeight() - margin_bottom)) {
            num_lines_required += 1;
            rectangle_size  = (ofGetHeight() - margin_bottom) / num_lines_required;
        }
    } else if(vistype == VisType::LIVE) {
        vector<size_t>* list;
        vector<string>* name_list;

        if(visdata == VisData::LOCATION) {
            list = &location_packets;
            name_list = &locations;
        } else if (visdata == VisData::SERVICE) {
            list = &service_packets;
            name_list = &services;
        }
        int num_lines = locations.size();
        float line_height = (ofGetHeight() - margin_bottom) / num_lines;
        vector<float> xes;
        for(int i = 0; i < num_lines; i++) {
            xes.push_back(0);
        }

        if(packets.size() > 0) {
            active_name = name_list->at(lastActive);
            for(int i = packets.size()-1; i >= 0; i--) {
                size_t index = packets.at(i).location_index;
                int y = index * line_height;
                if(do_filter_services) {
                    if(packets.at(i).service_index == lastActive) {
                        ofSetColor(255, base_alpha);
                    } else {
                        ofSetColor(0, base_alpha);
                    }
                } else {
                    // Get the assigned colour
                    auto service_it = serviceMap.find(services[packets.at(i).service_index]);
                    if(service_it != serviceMap.end()) {
                        ofSetColor(service_it->second->color);
                    }
                }
                
                
                float margin = ofClamp(rectangle_size * 0.1, 1, 10);
                ofDrawRectangle(xes[index], y, rectangle_width, line_height);
                xes[index] += rectangle_width;
            }
        }
    }

    

    ofSetColor(255);
    ofDrawRectangle(0, ofGetHeight() - margin_bottom, ofGetWidth(), margin_bottom);
    ofSetColor(0);
    font.drawString(active_name, 20, ofGetHeight() - margin_bottom*0.25);

    mainFbo.end();

    // Add some glitchy effects
    ofSetColor(255, 255);
    mainFbo.draw(0, 0);

    ofEnableDepthTest();
    ofPushMatrix();
    ofSetColor(255, 255);
    if(visdata == VisData::SERVICE && lastActive < service_point_clouds.size()) {
        cam.begin();
            service_point_clouds[lastActive].draw(ofGetElapsedTimef());
        cam.end();
    }    
    // cam.begin();
    //     for(int i = 0; i < service_point_clouds.size(); i++) {
    //         service_point_clouds[i].draw(ofGetElapsedTimef());
    //     } 
    // cam.end();
    ofPopMatrix();
    ofDisableDepthTest();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key) {
        case 't':
            vistype = VisType::TOGETHER;
            break;
        case 'c':
            vistype = VisType::CHRONOLOGICALLY;
            break;
        case 'l':
            vistype = VisType::LIVE;
            break;
        case 'r':
            reset_packets();
            break;
        case OF_KEY_UP:
            cycle_active();
            break;
    }

}

void ofApp::reset_packets() {
    location_packets.clear();
    service_packets.clear();
    services.clear();
    locations.clear();
    serviceMap.clear();
    locationMap.clear();
}

void ofApp::cycle_active() {
    if(visdata == VisData::LOCATION && locations.size() > 0) {
        auto& name = locations[lastActive];
        auto location_it = locationMap.find(name);
        if(location_it != locationMap.end()) {
            location_it->second->active = false;
        }
        lastActive += 1;
        if(lastActive >= locations.size()) lastActive = 0;
        current_name = locations[lastActive];
        location_it = locationMap.find(current_name);
        if(location_it != locationMap.end()) {
            location_it->second->active = true;
        }
    } else if (visdata == VisData::SERVICE && services.size() > 0) {
        auto& name = services[lastActive];
        auto service_it = serviceMap.find(name);
        if(service_it != serviceMap.end()) {
            service_it->second->active = false;
        }
        lastActive += 1;
        if(lastActive >= services.size()) lastActive = 0;
        current_name = services[lastActive];
        service_it = serviceMap.find(current_name);
        if(service_it != serviceMap.end()) {
            service_it->second->active = true;
        }
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

// based on the OF osc receiver example
void ofApp::handleIncomingOsc() {
  // check for waiting messages
	while(receiver.hasWaitingMessages()){

		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(m);

		if(m.getAddress() == "/bcm-packet"){
            auto location = m.getArgAsString(0);
            auto service = m.getArgAsString(2);
            auto len = m.getArgAsFloat(1);

            Packet packet;

            // Find location
            auto location_it = locationMap.find(location);
            if(location_it != locationMap.end()) {
                location_it->second->addOccurrence(len);
                location_packets.push_back(location_it->second->index_in_vector);
                packet.location_index = location_it->second->index_in_vector;
            } else {
                // location doesn't exist, add it
                auto stat_object = StatObject(location, locations.size(), ofColor(ofRandom(128, 255), ofRandom(128, 255), ofRandom(128, 255)));
                location_packets.push_back(locations.size());
                packet.location_index = locations.size();
                stat_object.addOccurrence(len);
                locationMap.insert(std::make_pair(location, make_shared<StatObject>(stat_object)));
                locations.push_back(location);
                location_3dpositions.push_back(randomSphere(ofRandom(500.0)));
            }

            // Find service
            auto service_it = serviceMap.find(service);
            if(service_it != serviceMap.end()) {
                service_it->second->addOccurrence(len);
                service_packets.push_back(service_it->second->index_in_vector);
                packet.service_index = service_it->second->index_in_vector;
                // packet.location_index
                service_point_clouds[service_it->second->index_in_vector].addRandomPointToWhiteCloseTo(location_3dpositions[packet.location_index]);
            } else {
                // location doesn't exist, add it
                auto stat_object = StatObject(service, services.size(), ofColor(ofRandom( 128, 255 ), ofRandom( 128, 255 ), ofRandom( 128, 255 )));
                service_packets.push_back(services.size());
                packet.service_index = services.size();
                stat_object.addOccurrence(len);
                serviceMap.insert(std::make_pair(service, make_shared<StatObject>(stat_object)));
                services.push_back(service);
                service_point_clouds.push_back(PointCloud());
                int i = service_point_clouds.size()-1;
                service_point_clouds[i].init(ofGetWidth(), ofGetHeight());
                service_point_clouds[i].addRandomPointToWhiteCloseTo(location_3dpositions[packet.location_index]);
            }
            packets.push_back(packet);
            if(visdata == VisData::LOCATION) {
                target_name = location;
            } else if (visdata == VisData::SERVICE) {
                target_name = service;
            }
        }
	}
}