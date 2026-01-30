#ifndef HELPERS_HXX_
#define HELPERS_HXX_

#include <functional>
#include <random>

#include "types.hxx"

extern std::random_device rd;
extern std::mt19937 rng;

extern double default_probability_generator();

using ProbabilityGenerator = std::function<double()>;

extern std::function<double()> probability_generator;

#endif /* HELPERS_HXX_ */
