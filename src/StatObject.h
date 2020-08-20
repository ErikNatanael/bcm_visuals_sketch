#pragma once

#include "ofMain.h"

class StatObject {
public:
    StatObject(string name_, size_t index_in_vector_, ofColor color_);

    void addOccurrence(int size);
    void update(float decay);

    string name;
    uint32_t num_packets;
    uint64_t total_packet_size;
    double leaking_num_packets;
    double leaking_packet_size;

    ofColor color;
    bool active;
    size_t index_in_vector = 0;
};

