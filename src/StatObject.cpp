#include "StatObject.h"

StatObject::StatObject(string name_, size_t index_in_vector_, ofColor color_) {
    name = name_;
    color = color_;
    num_packets = 0;
    total_packet_size = 0;
    leaking_num_packets = 0;
    leaking_packet_size = 0;
    active = false;
    index_in_vector = index_in_vector_;
}

void StatObject::addOccurrence(int size) {
    num_packets += 1;
    total_packet_size += size;
    leaking_num_packets += 1;
    leaking_packet_size += size;
}

void StatObject::update(float decay) {
    leaking_num_packets *= decay;
    leaking_packet_size *= decay;
}