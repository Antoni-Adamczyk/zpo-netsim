#ifndef PACKAGE_HXX
#define PACKAGE_HXX

#include "types.hxx"
#include <set>

class Package {
    public:
    Package();
    ~Package();
    explicit Package(ElementID id) : id_(id) {} // jeden elemet

    Package(const Package&) = delete;
    Package& operator=(const Package&) = delete;

    Package(Package&& other) noexcept;
    Package& operator=(Package&& other) noexcept;


    ElementID get_id() const;

    private:
    ElementID id_;
    static std::set<ElementID> assigned_IDs;
    static std::set<ElementID> freed_IDs;

    void assign_ID();
    void free_ID();
};
#endif 