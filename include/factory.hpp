//
// Created by antek on 13.01.2026.
//

#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "nodes.hpp"
#include "types.hpp"
#include <list>
#include <algorithm>
#include <map>
#include <stdexcept>

enum class NodeColor {
    UNVISITED, VISITED, VERIFIED
};

enum class ElementType {
    RAMP, WORKER, STOREHOUSE, LINK
};

struct ParsedLineData {
    ElementType element_type;
    std::map<std::string, std::string> parameters;
};

template<class Node>
class NodeCollection {
public:
    using container_t = std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    void add(Node&& node) {
        container_.emplace_back(std::move(node));
    }

    void remove_by_id(ElementID id) {
        container_.remove_if([id](const Node& elem) {
            return elem.get_id() == id;
        });
    }

    iterator find_by_id(ElementID id) {
        return std::find_if(container_.begin(), container_.end(),
            [id](const Node& elem) {
                return elem.get_id() == id;
            });
    }

    const_iterator find_by_id(ElementID id) const {
        return std::find_if(container_.begin(), container_.end(),
            [id](const Node& elem) {
                return elem.get_id() == id;
            });
    }

    iterator begin() { return container_.begin(); }
    iterator end() { return container_.end(); }

    const_iterator begin() const { return container_.cbegin(); }
    const_iterator end() const { return container_.cend(); }

    const_iterator cbegin() const { return container_.cbegin(); }
    const_iterator cend() const { return container_.cend(); }

private:
    container_t container_;
};

class Factory {
public:
    void add_ramp(Ramp&& ramp) {
        ramps_.add(std::move(ramp));
    }

    void remove_ramp(ElementID id) {
        ramps_.remove_by_id(id);
    }

    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) {
        return ramps_.find_by_id(id);
    }

    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const {
        return ramps_.find_by_id(id);
    }

    NodeCollection<Ramp>::const_iterator ramp_cbegin() const {
        return ramps_.cbegin();
    }

    NodeCollection<Ramp>::const_iterator ramp_cend() const {
        return ramps_.cend();
    }

    // Worker operations
    void add_worker(Worker&& worker) {
        workers_.add(std::move(worker));
    }

    void remove_worker(ElementID id);

    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) {
        return workers_.find_by_id(id);
    }

    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const {
        return workers_.find_by_id(id);
    }

    NodeCollection<Worker>::const_iterator worker_cbegin() const {
        return workers_.cbegin();
    }

    NodeCollection<Worker>::const_iterator worker_cend() const {
        return workers_.cend();
    }

    void add_storehouse(Storehouse&& storehouse) {
        storehouses_.add(std::move(storehouse));
    }

    void remove_storehouse(ElementID id);

    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) {
        return storehouses_.find_by_id(id);
    }

    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const {
        return storehouses_.find_by_id(id);
    }

    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const {
        return storehouses_.cbegin();
    }

    NodeCollection<Storehouse>::const_iterator storehouse_cend() const {
        return storehouses_.cend();
    }

    // Network operations
    bool is_consistent() const;
    void do_deliveries(Time time);
    void do_package_passing();
    void do_work(Time time);

private:
    template<class Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID id);

    NodeCollection<Ramp> ramps_;
    NodeCollection<Worker> workers_;
    NodeCollection<Storehouse> storehouses_;
};

template<class Node>
void Factory::remove_receiver(NodeCollection<Node>& collection, ElementID id) {
    auto it = collection.find_by_id(id);
    if (it == collection.end()) {
        return;
    }

    IPackageReceiver* receiver_ptr = &(*it);

    for (auto& ramp : ramps_) {
        ramp.remove_receiver(receiver_ptr);
    }

    for (auto& worker : workers_) {
        worker.remove_receiver(receiver_ptr);
    }

    collection.remove_by_id(id);
}

bool has_reachable_storehouse(const PackageSender* sender,
                              std::map<const PackageSender*, NodeColor>& node_colors);

ParsedLineData parse_line(const std::string& line);

Factory load_factory_structure(std::istream& is);

void save_factory_structure(Factory& factory, std::ostream& os);

#endif //FACTORY_HPP