//
// Created by antek on 13.01.2026.
//

#ifndef NODES_HPP
#define NODES_HPP

#define WITH_RECEIVER_TYPE 1

#include "types.hpp"
#include "package.hpp"
#include "helpers.hpp"
#include "storage_types.hpp"
#include <optional>
#include <memory>
#include <map>
#include <utility>

using Time = int;
using TimeOffset = unsigned int;

enum class ReceiverType {
    WORKER, STOREHOUSE
};

class IPackageReceiver {
public:
    virtual void receive_package(Package&& p) = 0;

    virtual ElementID get_id() const = 0;

    virtual IPackageStockpile::const_iterator cbegin() const = 0;

    virtual IPackageStockpile::const_iterator cend() const = 0;

    virtual IPackageStockpile::const_iterator begin() const = 0;

    virtual IPackageStockpile::const_iterator end() const = 0;

    virtual ReceiverType get_receiver_type() const = 0;

    virtual ~IPackageReceiver() = default;
};

class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    explicit ReceiverPreferences(ProbabilityGenerator pg = probability_generator) : generate_probability_(
            std::move(pg)) {};

    void add_receiver(IPackageReceiver* receiver);

    void remove_receiver(IPackageReceiver* receiver);

    IPackageReceiver* choose_receiver();

    const preferences_t& get_preferences() const {
        return this->preferences_;
    };

    const_iterator cbegin() const noexcept { return preferences_.cbegin(); };

    const_iterator cend() const noexcept { return preferences_.cend(); };

    const_iterator begin() const noexcept { return preferences_.begin(); };

    const_iterator end() const noexcept { return preferences_.end(); };


private:
    preferences_t preferences_;
    ProbabilityGenerator generate_probability_;
};

class PackageSender {
public:
    PackageSender() = default;

    PackageSender(PackageSender&& movable) = default;

    void send_package();

    const std::optional<Package>& get_sending_buffer() const { return buffer_; };

    void add_receiver(IPackageReceiver* receiver) {
        receiver_preferences_.add_receiver(receiver);
    }

    void remove_receiver(IPackageReceiver* receiver) {
        receiver_preferences_.remove_receiver(receiver);
    }

    const ReceiverPreferences& receiver_preferences() const {
        return receiver_preferences_;
    }

protected:
    void push_package(Package&& p) {
        buffer_ = std::move(p);
    };

    std::optional<Package> buffer_ = std::nullopt;

private:
    ReceiverPreferences receiver_preferences_;
};

class Ramp : public PackageSender {
public:
    Ramp(ElementID id, TimeOffset di)
            : PackageSender(), id_(id), di_(di), t_(0) {}

    void deliver_goods(Time t);

    TimeOffset get_delivery_interval() const { return di_; }

    ElementID get_id() const { return id_; }

private:
    ElementID id_;
    TimeOffset di_;
    Time t_;
};

class Storehouse : public IPackageReceiver {
public:
    Storehouse(ElementID id,
               std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(QueueType::FIFO)) : id_(id),
                                                                                                                d_(std::move(
                                                                                                                        d)) {}

    void receive_package(Package&& p) override;

    ElementID get_id() const override { return id_; }

    ReceiverType get_receiver_type() const override { return ReceiverType::STOREHOUSE; };

    IPackageStockpile::const_iterator cbegin() const override { return d_->cbegin(); }

    IPackageStockpile::const_iterator cend() const override { return d_->cend(); }

    IPackageStockpile::const_iterator begin() const override { return d_->begin(); }

    IPackageStockpile::const_iterator end() const override { return d_->end(); }

    IPackageStockpile* get_queue() const { return d_.get(); }

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};

class Worker : public IPackageReceiver, public PackageSender {
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q)
            : PackageSender(), id_(id), pd_(pd), q_(std::move(q)) {}

    void do_work(Time t);

    TimeOffset get_processing_duration() const { return pd_; }

    Time get_package_processing_start_time() const { return t_; }

    void receive_package(Package&& p) override;

    ElementID get_id() const override { return id_; }

    ReceiverType get_receiver_type() const override { return ReceiverType::WORKER; };

    const std::optional<Package>& get_processing_buffer() const { return buffer_; }

    IPackageStockpile::const_iterator cbegin() const override { return q_->cbegin(); }

    IPackageStockpile::const_iterator cend() const override { return q_->cend(); }

    IPackageStockpile::const_iterator begin() const override { return q_->begin(); }

    IPackageStockpile::const_iterator end() const override { return q_->end(); }

    IPackageQueue* get_queue() const { return q_.get(); }

private:
    ElementID id_;
    TimeOffset pd_;
    Time t_;
    std::unique_ptr<IPackageQueue> q_;
    std::optional<Package> buffer_ = std::nullopt;
};
#endif //NODES_HPP
