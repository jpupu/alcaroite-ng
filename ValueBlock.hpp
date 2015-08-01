#pragma once

#include <iosfwd> // forward declare std::istream/ostream
#include <map>
#include <string>
#include <vector>

namespace pupumath_plain {
struct vec3;
struct mat34;
}
using pupumath_plain::vec3;
using pupumath_plain::mat34;

struct ValueBlock {
  std::string type;
  std::string id;
  std::map<std::string, std::string> svalues;
  std::map<std::string, double> nvalues;
  std::map<std::string, vec3> vvalues;
  std::map<std::string, mat34> mvalues;

  ValueBlock() {}
  ValueBlock(std::string type, std::string id);

  void read_value(std::istream& istream);

  template <typename T>
  T get(const std::string& name) const;
  template <typename T>
  void set(const std::string& name, T value);
};

std::ostream& operator<<(std::ostream& ostream, const ValueBlock& block);

std::vector<ValueBlock> read_valueblock_file(std::istream& istream);
