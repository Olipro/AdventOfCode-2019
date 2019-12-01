#include "util/Core.h"

#include <cstdint>

enum class ParamMode {
  POSITION,
  IMMEDIATE,
};
class Insn {
  uint8_t opCode;
  ParamMode paramA;
  ParamMode paramB;
  ParamMode paramC;

 public:
  Insn(int32_t op) {
    opCode = op % 100;
    paramA = (op / 100) % 10 ? ParamMode::IMMEDIATE : ParamMode::POSITION;
    paramB = (op / 1000) % 10 ? ParamMode::IMMEDIATE : ParamMode::POSITION;
    paramC = (op / 10000) % 10 ? ParamMode::IMMEDIATE : ParamMode::POSITION;
  }
  [[nodiscard]] uint8_t OpCode() const noexcept { return opCode; }
  [[nodiscard]] ParamMode ParamA() const noexcept { return paramA; }
  [[nodiscard]] ParamMode ParamB() const noexcept { return paramB; }
  [[nodiscard]] ParamMode ParamC() const noexcept { return paramC; }
};

class IntCodeComputer {
  std::vector<int32_t> mem;
  int32_t insnPtr = 0;

  std::pair<int32_t, int32_t> GetArgs(const Insn& op) {
    auto a =
      op.ParamA() == ParamMode::POSITION ? mem[mem[insnPtr++]] : mem[insnPtr++];
    auto b =
      op.ParamB() == ParamMode::POSITION ? mem[mem[insnPtr++]] : mem[insnPtr++];
    return {a, b};
  }

  void Add(const Insn& op) {
    auto [a, b]         = GetArgs(op);
    mem[mem[insnPtr++]] = a + b;
  }

  void Multiply(const Insn& op) {
    auto [a, b]         = GetArgs(op);
    mem[mem[insnPtr++]] = a * b;
  }

  void JumpIfTrue(const Insn& op) {
    auto [a, b] = GetArgs(op);
    if (a)
      insnPtr = b;
  }

  void JumpIfFalse(const Insn& op) {
    auto [a, b] = GetArgs(op);
    if (!a)
      insnPtr = b;
  }

  void LessThan(const Insn& op) {
    auto [a, b]         = GetArgs(op);
    mem[mem[insnPtr++]] = a < b;
  }
  void Equals(const Insn& op) {
    auto [a, b]         = GetArgs(op);
    mem[mem[insnPtr++]] = a == b;
  }

  void Store(int32_t input, int32_t& dst) { dst = input; }

  int32_t Load(const Insn& op, int32_t* arg) {
    return op.ParamA() == ParamMode::POSITION ? mem[*arg] : *arg;
  }

 public:
  IntCodeComputer(std::vector<int32_t> mem) : mem{std::move(mem)} {}

  template <class Input, class Output>
  void Execute(Input inputFunc, Output outputFunc) {
    while (true) {
      Insn op{mem[insnPtr++]};
      switch (op.OpCode()) {
      case 1:
        Add(op);
        break;
      case 2:
        Multiply(op);
        break;
      case 3:
        Store(inputFunc(), mem[mem[insnPtr++]]);
        break;
      case 4:
        outputFunc(Load(op, mem.data() + insnPtr++));
        break;
      case 5:
        JumpIfTrue(op);
        break;
      case 6:
        JumpIfFalse(op);
        break;
      case 7:
        LessThan(op);
        break;
      case 8:
        Equals(op);
        break;
      case 99:
        return;
      }
    }
  }
};

class IntCode : public AoC::Solver<uint32_t, uint32_t> {
  std::vector<int32_t> mem;

  uint32_t SolvePart1() {
    IntCodeComputer comp{mem};
    int32_t ret;
    comp.Execute([] { return 1; }, [&ret](int32_t out) { ret = out; });
    return ret;
  }

  uint32_t SolvePart2() {
    IntCodeComputer comp{mem};
    int32_t ret;
    comp.Execute([] { return 5; }, [&ret](int32_t out) { ret = out; });
    return ret;
  }

 public:
  IntCode(std::istream& in, const std::vector<std::string>&)
    : mem{AoC::StreamToContainer<decltype(mem)>(in, ',')} {}

  [[nodiscard]] Results Solve() override {
    return {SolvePart1(), SolvePart2()};
  }
};

int main(int argc, const char* argv[]) {
  return AoC::main<IntCode>(argc, argv);
}
