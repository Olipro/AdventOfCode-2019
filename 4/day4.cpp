#include "util/Core.h"

#include <map>
#include <vector>

struct Digits {
  std::vector<char> digits;
  Digits(int v) {
    for (auto i = 100000; i >= 1; i /= 10)
      digits.emplace_back(v / i % 10);
  }
};

class PasswordGuesser : public AoC::Solver<uint32_t, uint32_t> {
  int32_t begin;
  int32_t end;
  static bool HasAdjacent(const Digits& d) {
    for (auto a = d.digits.begin(); (a + 1) != d.digits.end(); ++a)
      if (*a == *(a + 1))
        return true;
    return false;
  }

  static bool HasTwoUniqueDigits(const Digits& d) {
    std::map<int, int> sets;
    for (auto i : d.digits)
      ++sets[i];
    for (auto& i : sets)
      if (i.second == 2)
        return true;
    return false;
  }

  static bool DoesntDecrease(const Digits& d) {
    for (auto a = d.digits.rbegin(); (a + 1) != d.digits.rend(); ++a)
      if (*a < *(a + 1))
        return false;
    return true;
  }

 public:
  PasswordGuesser(std::istream&, std::vector<std::string> args) {
    if (args.size() != 2)
      throw std::runtime_error{"Must pass start + end numbers on command line, "
                               "space separated. filename is ignored"};
    begin = std::stoi(args[0]);
    end   = std::stoi(args[1]);
  }
  [[nodiscard]] Results Solve() override {
    auto totalA = 0, totalB = 0;
    for (auto i = begin; i <= end; ++i) {
      Digits d{i};
      if (HasAdjacent(d) && DoesntDecrease(d)) {
        ++totalA;
        if (HasTwoUniqueDigits(d))
          ++totalB;
      }
    }
    return {totalA, totalB};
  }
};

int main(int argc, const char* argv[]) {
  return AoC::main<PasswordGuesser>(argc, argv);
}
