#ifndef METAINFO_HXX
#define METAINFO_HXX

#include <any>
#include <functional>
#include <map>
#include <string>
#include <typeindex>
#include <typeinfo>

class MetaInfo {
public:
  MetaInfo() = default;
  void put(std::any value) {
    m_data.insert_or_assign(value.type(), std::move(value));
  }
  template <typename _Ty> _Ty &get() {
    return std::any_cast<_Ty &>(m_data.at(typeid(_Ty)));
  }
  template <typename _Ty> _Ty const &get() const {
    return std::any_cast<_Ty const &>(m_data.at(typeid(_Ty)));
  }

private:
  std::unordered_map<std::type_index, std::any> m_data;
};

#endif // METAINFO_HXX
