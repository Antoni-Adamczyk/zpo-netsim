//
// Created by antek on 13.01.2026.
//
#include <iostream>

#include "factory.hpp"
#include "../include/reports.hpp"
#include "package.hpp"
#include "storage_types.hpp"

int main() {
  Package p1;
  Package p2;

  PackageQueue q(QueueType::LIFO);
  q.push(std::move(p1));
  q.push(Package());
  q.push(Package());

  while (!q.empty()) {
    Package p = q.pop();
    std::cout << p.get_id() << " ";
  }
  std::cout << "\n\n";

  return 0;
}