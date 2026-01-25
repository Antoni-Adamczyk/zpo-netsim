//
// Created by antek on 13.01.2026.
//

#include "../include/package.hpp"

std::set<ElementID> Package::assigned_IDs;
std::set<ElementID> Package::freed_IDs;

Package::Package() {
    assign_ID();
}

Package::~Package() {
    if (id_ != 0) {
        free_ID();
    }
}

Package::Package(Package&& other) noexcept {
    id_ = other.id_;
    other.id_ = 0;   // 0 = brak ID
}

Package& Package::operator=(Package&& other) noexcept {
    if (this != &other) {
        free_ID();
        id_ = other.id_;
        other.id_ = 0;
    }
    return *this;
}

ElementID Package::get_id() const{
    return id_;
}

void Package::assign_ID() {
    if (!freed_IDs.empty()) {
        auto it = freed_IDs.begin();
        id_ = *it;
        freed_IDs.erase(it);
    }
    else {
        if (assigned_IDs.empty())
            id_=1;
        else
            id_=*assigned_IDs.rbegin()+1;
    }
    assigned_IDs.insert(id_);
}

void Package::free_ID(){
    assigned_IDs.erase(id_);
    freed_IDs.insert(id_);
}