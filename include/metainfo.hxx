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
  template <typename Ty> Ty &get() {
    return std::any_cast<Ty &>(m_data.at(typeid(Ty)));
  }
  template <typename Ty> Ty const &get() const {
    return std::any_cast<Ty const &>(m_data.at(typeid(Ty)));
  }
  auto begin() {
    return m_data.begin();
  }
  auto end() {
    return m_data.end();
  }
  template<typename Ty>
  auto find() {
    return m_data.find(typeid(Ty));
  }

private:
  std::unordered_map<std::type_index, std::any> m_data;
};

#endif // METAINFO_HXX
