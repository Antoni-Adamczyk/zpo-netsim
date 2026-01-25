//
// Created by antek on 20.01.2026.
//

#include "../include/storage_types.hpp"

#include <stdexcept>

std::string queue_type_to_string(QueueType type) {
    switch (type) {
        case QueueType::FIFO:
            return "FIFO";
        case QueueType::LIFO:
            return "LIFO";
    }
    return "";
}

PackageQueue::PackageQueue(QueueType type)
    : type_(type) {}


void PackageQueue::push(Package&& package) {
    packages_.emplace_back(std::move(package));
}


Package PackageQueue::pop() {
    if (packages_.empty()) {
        throw std::logic_error("PackageQueue::pop() on empty queue");
    }

    Package result =
        (type_ == QueueType::FIFO)
            ? std::move(packages_.front())
            : std::move(packages_.back());

    if (type_ == QueueType::FIFO) {
        packages_.pop_front();
    } else {
        packages_.pop_back();
    }

    return result;
}


QueueType PackageQueue::get_queue_type() const {
    return type_;
}


PackageQueue::const_iterator PackageQueue::begin() const {
    return packages_.begin();
}

PackageQueue::const_iterator PackageQueue::end() const {
    return packages_.end();
}

PackageQueue::const_iterator PackageQueue::cbegin() const {
    return packages_.cbegin();
}

PackageQueue::const_iterator PackageQueue::cend() const {
    return packages_.cend();
}

std::size_t PackageQueue::size() const {
    return packages_.size();
}

bool PackageQueue::empty() const {
    return packages_.empty();
}