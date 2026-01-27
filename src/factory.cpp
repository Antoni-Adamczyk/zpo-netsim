//
// Created by antek on 13.01.2026.
//

#include "factory.hpp"
#include <stdexcept>
#include <sstream>
#include <string>
#include <iostream>

void Factory::remove_worker(ElementID id) {
    remove_receiver(workers_, id);
}

void Factory::remove_storehouse(ElementID id) {
    remove_receiver(storehouses_, id);
}

bool has_reachable_storehouse(const PackageSender* sender,
                              std::map<const PackageSender*, NodeColor>& node_colors) {
    if (node_colors[sender] == NodeColor::VERIFIED) {
        return true;
    }

    node_colors[sender] = NodeColor::VISITED;

    if (sender->receiver_preferences().get_preferences().empty()) {
        return false;
    }


    for (const auto& [receiver, probability] : sender->receiver_preferences().get_preferences()) {
        if (receiver->get_receiver_type() == ReceiverType::STOREHOUSE) {
            node_colors[sender] = NodeColor::VERIFIED;
            return true;
        }

        auto worker_ptr = dynamic_cast<Worker*>(receiver);
        if (!worker_ptr) continue;

        auto next_sender = dynamic_cast<PackageSender*>(worker_ptr);
        if (next_sender == sender) {
            continue;
        }

        if (node_colors[next_sender] == NodeColor::UNVISITED) {
            if (has_reachable_storehouse(next_sender, node_colors)) {
                node_colors[sender] = NodeColor::VERIFIED;
                return true;
                }
            } else if (node_colors[next_sender] == NodeColor::VERIFIED) {
                node_colors[sender] = NodeColor::VERIFIED;
                return true;
            }
        }

    node_colors[sender] = NodeColor::VERIFIED;
    return false;
    }

bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> node_colors;

    for (const auto& ramp : ramps_) {
        node_colors[&ramp] = NodeColor::UNVISITED;
    }

    for (const auto& worker : workers_) {
        node_colors[&worker] = NodeColor::UNVISITED;
    }

    for (const auto& ramp : ramps_) {
        if (!has_reachable_storehouse(&ramp, node_colors)) {
            return false;
        }
    }

    return true;
}

void Factory::do_deliveries(Time time) {
    for (auto& ramp : ramps_) {
        ramp.deliver_goods(time);
    }
}

void Factory::do_package_passing() {
    for (auto& ramp : ramps_) {
        ramp.send_package();
    }

    for (auto& worker : workers_) {
        worker.send_package();
    }
}

void Factory::do_work(Time time) {
    for (auto& worker : workers_) {
        worker.do_work(time);
    }
}

ParsedLineData parse_line(const std::string& line) {
    ParsedLineData result;
    std::istringstream iss(line);
    std::string element_type_str;

    iss >> element_type_str;

    if (element_type_str == "LOADING_RAMP") {
        result.element_type = ElementType::RAMP;
    } else if (element_type_str == "WORKER") {
        result.element_type = ElementType::WORKER;
    } else if (element_type_str == "STOREHOUSE") {
        result.element_type = ElementType::STOREHOUSE;
    } else if (element_type_str == "LINK") {
        result.element_type = ElementType::LINK;
    }

    std::string token;
    while (iss >> token) {
        size_t pos = token.find('=');
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);
            result.parameters[key] = value;
        }
    }

    return result;
}

Factory load_factory_structure(std::istream& is) {
    Factory factory;
    std::string line;

    while (std::getline(is, line)) {
        if (line.empty() || line[0] == ';') {
            continue;
        }

        ParsedLineData data = parse_line(line);

        switch (data.element_type) {
            case ElementType::RAMP: {
                ElementID id = std::stoi(data.parameters["id"]);
                TimeOffset di = std::stoi(data.parameters["delivery-interval"]);
                factory.add_ramp(Ramp(id, di));
                break;
            }
            case ElementType::WORKER: {
                ElementID id = std::stoi(data.parameters["id"]);
                TimeOffset pd = std::stoi(data.parameters["processing-time"]);

                std::unique_ptr<IPackageQueue> queue;
                if (data.parameters["queue-type"] == "FIFO") {
                    queue = std::make_unique<PackageQueue>(QueueType::FIFO);
                } else if (data.parameters["queue-type"] == "LIFO") {
                    queue = std::make_unique<PackageQueue>(QueueType::LIFO);
                }

                factory.add_worker(Worker(id, pd, std::move(queue)));
                break;
            }
            case ElementType::STOREHOUSE: {
                ElementID id = std::stoi(data.parameters["id"]);
                factory.add_storehouse(Storehouse(id));
                break;
            }
            case ElementType::LINK: {
                std::string src = data.parameters["src"];
                std::string dest = data.parameters["dest"];

                size_t src_dash = src.find('-');
                std::string src_type = src.substr(0, src_dash);
                ElementID src_id = std::stoi(src.substr(src_dash + 1));

                size_t dest_dash = dest.find('-');
                std::string dest_type = dest.substr(0, dest_dash);
                ElementID dest_id = std::stoi(dest.substr(dest_dash + 1));

                PackageSender* sender = nullptr;
                if (src_type == "ramp") {
                    auto it = factory.find_ramp_by_id(src_id);
                    if (it == factory.ramp_cend()) {
                        throw std::logic_error("Invalid ramp id in LINK");
                    }
                    sender = &(*it);
                } else if (src_type == "worker") {
                    auto it = factory.find_worker_by_id(src_id);
                    if (it == factory.worker_cend()) {
                        throw std::logic_error("Invalid worker id in LINK");
                    }
                    sender = &(*it);
                }

                IPackageReceiver* receiver = nullptr;
                if (dest_type == "worker") {
                    auto it = factory.find_worker_by_id(dest_id);
                    if (it == factory.worker_cend()) {
                        throw std::logic_error("Invalid worker id in LINK");
                    }
                    receiver = &(*it);
                } else if (dest_type == "store") {
                    auto it = factory.find_storehouse_by_id(dest_id);
                    if (it == factory.storehouse_cend()) {
                        throw std::logic_error("Invalid storehouse id in LINK");
                    }
                    receiver = &(*it);
                }

                if (sender && receiver) {
                    sender->add_receiver(receiver);
                }
                break;
            }
        }
    }

    return factory;
}

void save_factory_structure(Factory& factory, std::ostream& os) {
    os << "; == LOADING RAMPS ==\n\n";
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        os << "LOADING_RAMP id=" << it->get_id()
           << " delivery-interval=" << it->get_delivery_interval() << "\n";
    }

    os << "\n; == WORKERS ==\n\n";
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        os << "WORKER id=" << it->get_id()
           << " processing-time=" << it->get_processing_duration()
           << " queue-type=";

        if (dynamic_cast<PackageQueue*>(it->get_queue())->get_queue_type() == QueueType::FIFO) {
            os << "FIFO";
        } else {
            os << "LIFO";
        }
        os << "\n";
    }

    os << "\n; == STOREHOUSES ==\n\n";
    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        os << "STOREHOUSE id=" << it->get_id() << "\n";
    }

    os << "\n; == LINKS ==\n\n";

    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        for (const auto& [receiver, prob] : it->receiver_preferences().get_preferences()) {
            os << "LINK src=ramp-" << it->get_id() << " dest=";
            if (receiver->get_receiver_type() == ReceiverType::WORKER) {
                os << "worker-" << receiver->get_id();
            } else {
                os << "store-" << receiver->get_id();
            }
            os << "\n";
        }
    }

    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        for (const auto& [receiver, prob] : it->receiver_preferences().get_preferences()) {
            os << "LINK src=worker-" << it->get_id() << " dest=";
            if (receiver->get_receiver_type() == ReceiverType::WORKER) {
                os << "worker-" << receiver->get_id();
            } else {
                os << "store-" << receiver->get_id();
            }
            os << "\n";
        }
    }

    os << "\n";
}




