#pragma once

#include "pupumath_struct.hpp"
#include "spectrum.hpp"
#include <iosfwd> // forward declare std::istream/ostream
#include <map>
#include <string>
#include <vector>

struct ValueBlock {
  std::string type;
  std::string id;
  std::map<std::string, std::string> svalues;
  std::map<std::string, double> nvalues;
  std::map<std::string, pupumath::vec3> vvalues;
  std::map<std::string, pupumath::mat34> mvalues;
  std::map<std::string, Spectrum> spvalues;
  std::map<std::string, std::vector<pupumath::vec3>> lvvalues;
  std::map<std::string, std::vector<int>> livalues;

  ValueBlock() {}
  ValueBlock(std::string type, std::string id);

  void read_value(std::istream& istream);

  template <typename T>
  bool has(const std::string& name) const;
  template <typename T>
  T get(const std::string& name) const;
  template <typename T>
  void set(const std::string& name, T value);
};

std::ostream& operator<<(std::ostream& ostream, const ValueBlock& block);

std::vector<ValueBlock> read_valueblock_file(std::istream& istream);
