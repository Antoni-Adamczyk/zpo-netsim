//
// Created by antek on 13.01.2026.
//

#include "../include/nodes.hpp"

using ReceiverPair = std::pair<IPackageReceiver* const, double>;

void ReceiverPreferences::add_receiver(IPackageReceiver* r) {
    const double new_weight = 1.0 / static_cast<double>(preferences_.size() + 1);

    // aktualizuje wagi wszystkich istniejących odbiorników
    for (auto& [receiver, weight] : preferences_) {
        weight = new_weight;
    }

    // dodaje nowego odbiorcę z tą samą wagą
    preferences_[r] = new_weight;
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r) {
    // próbuje znaleźć odbiorcę w mapie
    auto it = preferences_.find(r);
    if (it == preferences_.end()) return; // nic nie robi, jeśli nie istnieje

    // usuwa odbiorcę
    preferences_.erase(it);

    // jeśli nie ma już odbiorców, nic nie robi
    const auto num_of_receivers = preferences_.size();
    if (num_of_receivers == 0) return;

    // aktualizuje wagi pozostałych odbiorców
    const double new_weight = 1.0 / static_cast<double>(num_of_receivers);
    for (auto& [receiver, weight] : preferences_) {
        weight = new_weight;
    }
}

void Storehouse::receive_package(Package&& p) {
    if (!d_) return;
    d_->push(std::move(p));         // przenosi paczkę do magazynu
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    if (preferences_.empty()) return nullptr;

    double r = generate_probability_();
    double cumulative = 0.0;

    for (auto& [receiver, weight] : preferences_) {
        cumulative += weight;
        if (r <= cumulative) {
            return receiver;   // wybiera pierwszego, którego skumulowana waga >= r
        }
    }

    // na wszelki wypadek zwraca ostatniego odbiorcę
    return preferences_.rbegin()->first;
}


void PackageSender::send_package() {
    // Sprawdza, czy bufor zawiera paczkę
    if (!buffer_) return;

    // Wybiera odbiorcę paczki
    IPackageReceiver* receiver = receiver_preferences_.choose_receiver();
    if (!receiver) return;

    // Przenosi paczkę z bufora do odbiorcy
    receiver->receive_package(std::move(*buffer_));

    // Opróżnia bufor
    buffer_.reset();
}

void Worker::receive_package(Package &&p) {
    q_->push(std::move(p));
}

void Worker::do_work(Time t) {
    // jeśli bufor pusty i kolejka niepusta to pobiera paczkę
    if (!buffer_ && !q_->empty()) {
        buffer_ = q_->pop();  // przeniesienie paczki do bufora
        t_ = t;               // zapamiętanie momentu rozpoczęcia pracy
    }

    // jeśli paczka w buforze i czas przetwarzania minął to wysyła paczkę
    if (buffer_ && (t - t_ + 1 >= pd_)) {
        push_package(std::move(*buffer_)); // przenosi paczkę do wysyłki
        buffer_.reset();
    }
}

void Ramp::deliver_goods(Time t) {
    // jeśli bufor pusty to tworzy paczkę i rozpoczna liczenie czasu
    if (!buffer_) {
        buffer_ = Package(id_);  // nowa paczka z ID rampy
        t_ = t;                  // zapamiętuje tick rozpoczęcia
    }

    // jeśli minął interwał dostawy to wyśla paczkę
    if (buffer_ && (t - t_ + 1 >= di_)) {
        push_package(std::move(*buffer_)); // przenosi paczkę do wysyłki
        buffer_.reset();
    }
}