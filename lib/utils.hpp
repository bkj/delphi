#pragma once

#include <fmt/format.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace delphi::utils {

using std::cout, std::endl, std::vector, std::string, std::unordered_map,
    std::unordered_set;

template <class T> void printVec(vector<T> xs) {
  for (T x : xs) {
    cout << x << ", " << endl;
  }
}

template <class AssociativeContainer, class Value>
bool in(AssociativeContainer container, Value value) {
  return container.count(value) != 0;
}

template <class AssociativeContainer, class Key, class Value>
Value get(AssociativeContainer container, Key key, Value default_value) {
  return in(container, key) ? container[key] : default_value;
}

template <class V, class Iterable> vector<V> list(Iterable xs) {
  vector<V> xs_copy;
  for (auto x : xs) {
    xs_copy.push_back(x);
  }
  return xs_copy;
}

template <class F, class V> vector<V> lmap(F f, vector<V> vec) {
  vector<V> transformed_vector;
  for (V x : vec) {
    transformed_vector.push_back(f(x));
  }
  return transformed_vector;
}

/**
 * Returns the square of a number.
 */
inline double sqr(double x) {return x * x};

/**
 * Returns the sum of a vector of doubles.
 */
double sum(const std::vector<double> &v);

/**
 * Returns the arithmetic mean of a vector of doubles.
 */
double mean(const std::vector<double> &v);

/**
 * Returns the sample standard deviation of a vector of doubles.
 */
double standard_deviation(const double mean, const std::vector<double> &v);

/**
 * Returns the median of a vector of doubles.
 */
double median(const std::vector<double> &xs);

double log_normpdf(double x, double mean, double sd);

/** Load JSON data from a file */
nlohmann::json load_json(std::string filename);

} // namespace delphi::utils
