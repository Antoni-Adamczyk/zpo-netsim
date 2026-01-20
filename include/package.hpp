//
// Created by antek on 13.01.2026.
//

#ifndef PACKAGE_HPP
#define PACKAGE_HPP
#include <set>

#include "types.hpp"

class Package {
    public:
    Package();
    ~Package();

    ElementID get_id() const;

    private:
    ElementID id_;
    static std::set<ElementID> assigned_IDs;
    static std::set<ElementID> freed_IDs;

    void assign_ID();
    void free_ID();
};
#endif //PACKAGE_HPP
