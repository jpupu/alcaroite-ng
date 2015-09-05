#include "ValueBlock.hpp"
#include <istream>
#include <ostream>

ValueBlock::ValueBlock(std::string type, std::string id) : type(type), id(id) {}

template <typename T>
T get_value(const std::string& name, const std::map<std::string, T>& map,
            std::string type_name, std::string id)
{
  auto val = map.find(name);
  if (val == map.end()) {
    throw std::runtime_error("Value " + type_name + " '" + id + "::" + name +
                             "' not found");
  }
  return val->second;
}

template <>
std::string ValueBlock::get(const std::string& name) const
{
  return get_value(name, svalues, "string", id);
}

template <>
double ValueBlock::get(const std::string& name) const
{
  return get_value(name, nvalues, "double", id);
}

template <>
pupumath::vec3 ValueBlock::get(const std::string& name) const
{
  return get_value(name, vvalues, "vector", id);
}

template <>
pupumath::mat34 ValueBlock::get(const std::string& name) const
{
  return get_value(name, mvalues, "matrix", id);
}

template <>
Spectrum ValueBlock::get(const std::string& name) const
{
  return get_value(name, spvalues, "spectrum", id);
}

template <>
std::vector<pupumath::vec3> ValueBlock::get(const std::string& name) const
{
  return get_value(name, lvvalues, "vector list", id);
}

template <>
std::vector<int> ValueBlock::get(const std::string& name) const
{
  return get_value(name, livalues, "int list", id);
}

template <typename T>
void set_value(const std::string& name, T value, std::map<std::string, T>& map,
               std::string type_name, std::string id)
{
  if (map.find(name) != map.end()) {
    throw std::runtime_error("Value " + type_name + " '" + id + "::" + name +
                             "' redefined");
  }
  map[name] = value;
}

template <>
void ValueBlock::set(const std::string& name, std::string value)
{
  set_value(name, value, svalues, "string", id);
}

template <>
void ValueBlock::set(const std::string& name, double value)
{
  set_value(name, value, nvalues, "double", id);
}

template <>
void ValueBlock::set(const std::string& name, pupumath::vec3 value)
{
  set_value(name, value, vvalues, "vector", id);
}

template <>
void ValueBlock::set(const std::string& name, pupumath::mat34 value)
{
  set_value(name, value, mvalues, "matrix", id);
}

template <>
void ValueBlock::set(const std::string& name, Spectrum value)
{
  set_value(name, value, spvalues, "spectrum", id);
}

template <>
void ValueBlock::set(const std::string& name, std::vector<pupumath::vec3> value)
{
  set_value(name, value, lvvalues, "vector list", id);
}

template <>
void ValueBlock::set(const std::string& name, std::vector<int> value)
{
  set_value(name, value, livalues, "int list", id);
}

void ValueBlock::read_value(std::istream& istream)
{
  std::string name, val;
  istream >> name >> val;

  if ((val[0] >= '0' && val[0] <= '9') || val[0] == '-' || val[0] == '.') {
    double v = std::stod(val);
    set(name, v);
  }
  else if (val == "vec") {
    pupumath::vec3 v;
    istream >> v[0] >> v[1] >> v[2];
    set(name, v);
  }
  else if (val == "mat") {
    pupumath::mat34 v;
    for (int i = 0; i < 12; ++i) {
      istream >> v[i];
    }
    set(name, v);
  }
  else if (val == "spectrum") {
    Spectrum v;
    for (int i = 0; i < Spectrum::count; ++i) {
      istream >> v.samples[i];
    }
    set(name, v);
  }
  else if (val == "veclist") {
    int count;
    istream >> count;
    std::vector<pupumath::vec3> vs(count);
    for (auto& v : vs) {
      istream >> v[0] >> v[1] >> v[2];
    }
    set(name, vs);
  }
  else if (val == "intlist") {
    int count;
    istream >> count;
    std::vector<int> vs(count);
    for (auto& v : vs) {
      istream >> v;
    }
    set(name, vs);
  }
  else {
    set(name, val);
  }
}

std::ostream& operator<<(std::ostream& ostream, const ValueBlock& block)
{
  ostream << "(block) " << block.type << " : " << block.id << '\n';
  for (const auto& v : block.svalues) {
    ostream << "::(string) " << v.first << " " << v.second << "\n";
  }
  for (const auto& v : block.nvalues) {
    ostream << "::(double) " << v.first << " " << v.second << "\n";
  }
  for (const auto& v : block.vvalues) {
    ostream << "::(vector) " << v.first << " " << v.second.x << ','
            << v.second.y << ',' << v.second.z << "\n";
  }
  for (const auto& v : block.mvalues) {
    ostream << "::(matrix) " << v.first << " " << v.second[0] << "..."
            << "\n";
  }
  ostream << ".\n\n";

  return ostream;
}

std::vector<ValueBlock> read_valueblock_file(std::istream& istream)
{
  ValueBlock* block = nullptr;
  std::vector<ValueBlock> blocks;

  if (!istream.good()) {
    throw std::runtime_error("attempting to read a bad file");
  }

  while (true) {
    std::string token;
    istream >> token;
    if (!istream.good()) break;

    if (token == "::") {
      if (!block) {
        throw std::runtime_error("value outside block");
      }
      block->read_value(istream);
    }
    else if (token == ".") {
      if (!block) {
        throw std::runtime_error("closing mark outside block");
      }
      block = nullptr;
    }
    else {
      std::string name;
      istream >> name;
      blocks.push_back(ValueBlock(token, name));
      block = &blocks.back();
    }
  }

  if (block) {
    throw std::runtime_error("eof while block open");
  }

  return blocks;
}
