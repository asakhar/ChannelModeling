#ifndef DATALOGGER_HPP
#define DATALOGGER_HPP
#include "unitproto.hpp"
#include <functional>
#include <ostream>

template <typename In_t, typename Pack_t>
class DataLogger : public UnitProto<In_t, In_t> {
public:
  void run() override {
    UnitProto<In_t, In_t>::meta.put(Pack_t{UnitProto<In_t, In_t>::input});
    UnitProto<In_t, In_t>::output = std::move(UnitProto<In_t, In_t>::input);
  }
};

template <typename In_t, typename Pack_t>
class DataPrinter : public UnitProto<In_t, In_t> {
  using DataItem = decltype(In_t{}[0]);
  std::function<std::string(DataItem const &)> m_printer;
  std::ostream &m_str;
  std::string m_sep;

public:
  DataPrinter(std::function<std::string(DataItem const &)> printer,
              std::ostream &stream = std::cout,
              std::string const &separator = " ")
      : m_printer{printer}, m_str{stream}, m_sep{separator} {}
  void run() override {
    auto &meta = UnitProto<In_t, In_t>::meta;
    for (DataItem const &item : meta.template get<Pack_t>().data) {
      m_str << m_printer(item) << m_sep;
    }
    m_str << std::endl;
    UnitProto<In_t, In_t>::output = std::move(UnitProto<In_t, In_t>::input);
  }
};

#endif // DATALOGGER_HPP
