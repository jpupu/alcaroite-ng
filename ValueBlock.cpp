#include "ValueBlock.hpp"

#include "pupumath_plain.hpp"

#include <istream>
#include <ostream>

ValueBlock::ValueBlock(std::string type, std::string id) : type(type), id(id) {}

template <>
std::string ValueBlock::get(const std::string& name) const
{
  auto val = svalues.find(name);
  if (val == svalues.end()) {
    throw std::runtime_error("string value " + id + ":" + name +
                             " not defined");
  }
  return val->second;
}

template <>
double ValueBlock::get(const std::string& name) const
{
  auto val = nvalues.find(name);
  if (val == nvalues.end()) {
    throw std::runtime_error("double value " + id + ":" + name +
                             " not defined");
  }
  return val->second;
}

template <>
vec3 ValueBlock::get(const std::string& name) const
{
  auto val = vvalues.find(name);
  if (val == vvalues.end()) {
    throw std::runtime_error("vector value " + id + ":" + name +
                             " not defined");
  }
  return val->second;
}

template <>
mat34 ValueBlock::get(const std::string& name) const
{
  auto val = mvalues.find(name);
  if (val == mvalues.end()) {
    throw std::runtime_error("matrix value " + id + ":" + name +
                             " not defined");
  }
  return val->second;
}

template <>
void ValueBlock::set(const std::string& name, std::string value)
{
  if (svalues.find(name) != svalues.end()) {
    throw std::runtime_error("string value " + id + ":" + name + " redefined");
  }
  svalues[name] = value;
}

template <>
void ValueBlock::set(const std::string& name, double value)
{
  if (nvalues.find(name) != nvalues.end()) {
    throw std::runtime_error("double value " + id + ":" + name + " redefined");
  }
  nvalues[name] = value;
}

template <>
void ValueBlock::set(const std::string& name, vec3 value)
{
  if (vvalues.find(name) != vvalues.end()) {
    throw std::runtime_error("vector value " + id + ":" + name + " redefined");
  }
  vvalues[name] = value;
}

template <>
void ValueBlock::set(const std::string& name, mat34 value)
{
  if (mvalues.find(name) != mvalues.end()) {
    throw std::runtime_error("matrix value " + id + ":" + name + " redefined");
  }
  mvalues[name] = value;
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
    vec3 v;
    istream >> v[0] >> v[1] >> v[2];
    set(name, v);
  }
  else if (val == "mat") {
    mat34 v;
    for (int i = 0; i < 12; ++i) {
      istream >> v[i];
    }
    set(name, v);
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
