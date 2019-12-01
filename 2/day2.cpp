#include "util/Core.h"

#include <cstdint>

class IntCode : public AoC::Solver<uint32_t, uint32_t> {
  std::vector<uint32_t> mem;
  static constexpr uint32_t target = 19690720;

  static void Add(uint16_t pos, std::vector<uint32_t>& memory) {
    memory[memory[pos + 2]] = memory[memory[pos]] + memory[memory[pos + 1]];
  }

  static void Multiply(uint16_t pos, std::vector<uint32_t>& memory) {
    memory[memory[pos + 2]] = memory[memory[pos]] * memory[memory[pos + 1]];
  }

  static void Execute(std::vector<uint32_t>& mem) {
    auto pos = 0;
    while (true) {
      switch (mem[pos]) {
      case 1:
        Add(pos + 1, mem);
        pos += 4;
        break;
      case 2:
        Multiply(pos + 1, mem);
        pos += 4;
        break;
      case 99:
        return;
      }
    }
  }

  uint32_t SolvePart2() {
    for (auto i = 0; i <= 99; ++i) {
      for (auto j = 0; j <= 99; ++j) {
        auto memTmp = mem;
        memTmp[1]   = i;
        memTmp[2]   = j;
        Execute(memTmp);
        if (memTmp[0] == target)
          return (100 * i) + j;
      }
    }
    return 0;
  }

 public:
  IntCode(std::istream& in, std::vector<std::string>)
    : mem{AoC::StreamToContainer<decltype(mem)>(in, ',')} {
    mem[1] = 12;
    mem[2] = 2;
  }

  [[nodiscard]] Results Solve() override {
    auto mem1 = mem;
    Execute(mem1); // Part 1
    return {mem1[0], SolvePart2()};
  }
};

int main(int argc, const char* argv[]) {
  return AoC::main<IntCode>(argc, argv);
}
