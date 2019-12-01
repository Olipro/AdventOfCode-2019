#ifndef AOC_UTIL_CORE
#define AOC_UTIL_CORE

#include <fstream>
#include <iostream>
#include <istream>
#include <string>
#include <utility>
#include <vector>
#include <type_traits>

namespace AoC {
  template <typename Part1Result_t, typename Part2Result_t>
  class Solver {
   public:
    using Results = std::pair<Part1Result_t, Part2Result_t>;
    [[nodiscard]] virtual Results Solve() = 0;
  };

  template <typename Container>
  [[nodiscard]] Container StreamToContainer(std::istream& in) {
    Container ret;
    typename Container::value_type val;
    while (in >> val)
      ret.emplace_back(val);
    return ret;
  }

  template <typename Elem, class UnaryFunction>
  void ForEachInStream(std::istream& in, Elem elem, UnaryFunction func) {
    while (in >> elem)
      func(elem);
  }

  template <typename Solver_t>
  [[nodiscard]] static int main(const int argc, const char* argv[]) {
    if (argc < 2) {
      std::cout << "Error, Wrong number of args passed. First argument must be "
                   "input file"
                << '\n';
      return -2;
    }
    std::vector<std::string> extraArgs;
    if (argc - 2 > 0) {
      extraArgs.reserve(argc - 2);
      for (auto i = 2; i < argc; ++i)
        extraArgs.emplace_back(argv[i]);
    }
    try {
      std::ifstream file{argv[1]};
      Solver_t solver{file, std::move(extraArgs)};
      auto&& [part1, part2] = solver.Solve();
      std::cout << "Part1 Answer: " << part1 << '\n';
      std::cout << "Part2 Answer: " << part2 << '\n';
    } catch (const std::exception& e) {
      std::cout << "Error: " << e.what() << '\n';
      return 1;
    }
    return 0;
  }
} // namespace AoC

#endif // AOC_UTIL_CORE
