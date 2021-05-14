#include <cstdio>
#include <fstream>
#include <string>
struct Keeper {
  std::ifstream &file;
  std::string row;

  void *pos_;

  struct iterator {
    Keeper &keeper;
    auto &operator++() {
      if (keeper.pos_ != nullptr) {
        goto *keeper.pos_;
      }
      keeper.pos_ = &&pos2;
      while (true) {
      pos2:
        if (keeper.file.bad() || keeper.file.eof())
          break;
        std::getline(keeper.file, keeper.row);

        break;
      }
      return *this;
    }
    auto operator*() {
      if (keeper.pos_ == nullptr)
        ++*this;
      return keeper.row;
    }
    bool operator!=(iterator const & /**/) const {
      return !(keeper.file.bad() || keeper.file.eof() || keeper.file.fail());
    }
  };
  auto begin() { return iterator{*this}; }
  auto end() { return iterator{*this}; }
};

int main() {
  std::ifstream file{"test.txt"};
  Keeper keep{.file = file};
  for (auto row : keep) {
    std::printf("%s\n", row.c_str());
  }
  file.close();
  return 0;
}
