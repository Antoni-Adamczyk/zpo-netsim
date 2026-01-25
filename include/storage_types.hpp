//
// Created by antek on 20.01.2026.
//

#ifndef STORAGE_TYPES_HPP
#define STORAGE_TYPES_HPP

#include <list>
#include <string>
#include "package.hpp"

enum class QueueType {
    FIFO,
    LIFO
};

std::string queue_type_to_string(QueueType type);


class IPackageStockpile {
public:
    using const_iterator = std::list<Package>::const_iterator;

    virtual ~IPackageStockpile() = default;

    virtual void push(Package&& package) = 0;

    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;

    virtual std::size_t size() const = 0;
    virtual bool empty() const = 0;
};

class IPackageQueue : public IPackageStockpile {
public:
    virtual Package pop() = 0;
    virtual QueueType get_queue_type() const = 0;
};

class PackageQueue : public IPackageQueue {
public:
    explicit PackageQueue(QueueType type);

    void push(Package&& package) override;
    Package pop() override;

    QueueType get_queue_type() const override;

    const_iterator begin() const override;
    const_iterator end() const override;
    const_iterator cbegin() const override;
    const_iterator cend() const override;

    std::size_t size() const override;
    bool empty() const override;

private:
    QueueType type_;
    std::list<Package> packages_;
};

#endif //STORAGE_TYPES_HPP
