#ifndef METAINFO_HXX
#define METAINFO_HXX

#include <any>
#include <functional>
#include <map>
#include <string>
#include <typeindex>
#include <typeinfo>
/**
 * @brief Container class to carry some user information between units
 *
 */
class MetaInfo {
public:
  MetaInfo() = default;
  /**
   * @brief Puts user data into the container.
   * It's better to put user info in singleton classes to prevent data type
   * collisions
   *
   * @param value
   */
  void put(std::any value) {
    m_data.insert_or_assign(value.type(), std::move(value));
  }
  /**
   * @brief Returns read-write enabled reference to requested object
   *
   * @tparam Ty type of object to be extracted
   * @return Ty&
   */
  template <typename Ty> Ty &get() {
    return std::any_cast<Ty &>(m_data.at(typeid(Ty)));
  }
  /**
   * @brief Returns readonly reference to requested object
   *
   * @tparam Ty type of object to be extracted
   * @return Ty const&
   */
  template <typename Ty> Ty const &get() const {
    return std::any_cast<Ty const &>(m_data.at(typeid(Ty)));
  }
  /**
   * @brief Begin iterator
   *
   * @return auto
   */
  inline auto begin() { return m_data.begin(); }
  /**
   * @brief End iterator
   *
   * @return auto
   */
  inline auto end() { return m_data.end(); }
  /**
   * @brief Begin const iterator
   *
   * @return auto
   */
  inline auto begin() const { return m_data.begin(); }
  /**
   * @brief End const iterator
   *
   * @return auto
   */
  inline auto end() const { return m_data.end(); }
  /**
   * @brief Returns iterator to object of specified type
   *
   * @tparam Ty type of object to be found
   * @return auto
   */
  template <typename Ty> auto find() { return m_data.find(typeid(Ty)); }
  /**
   * @brief Returns iterator to object of specified type
   *
   * @tparam Ty type of object to be found
   * @return auto
   */
  template <typename Ty> auto find() const { return m_data.find(typeid(Ty)); }

private:
  std::unordered_map<std::type_index, std::any> m_data;
};

#endif // METAINFO_HXX
