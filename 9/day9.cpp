#include "util/Core.h"

#include <cstdint>
#include <queue>
#include <sstream>
#include <utility>

enum class ParamMode {
  POSITION,
  IMMEDIATE,
  RELATIVE,
};
class Insn {
  uint8_t opCode;
  ParamMode paramA;
  ParamMode paramB;
  ParamMode paramC;

  static ParamMode GetMode(int32_t val) {
    switch (val) {
    case 0:
      return ParamMode::POSITION;
    case 1:
      return ParamMode::IMMEDIATE;
    case 2:
      return ParamMode::RELATIVE;
    }
    throw std::runtime_error{"Invalid ParamMode"};
  }

 public:
  explicit Insn(int64_t op) {
    opCode = op % 100;
    paramA = GetMode((op / 100) % 10);
    paramB = GetMode((op / 1000) % 10);
    paramC = GetMode((op / 10000) % 10);
  }
  [[nodiscard]] uint8_t OpCode() const noexcept { return opCode; }
  [[nodiscard]] ParamMode ParamA() const noexcept { return paramA; }
  [[nodiscard]] ParamMode ParamB() const noexcept { return paramB; }
  [[nodiscard]] ParamMode ParamC() const noexcept { return paramC; }
};

enum class ExecState { NEED_INPUT, HAS_OUTPUT, HALTED };

class IntCodeComputer {
  std::vector<int64_t> mem;
  std::queue<int64_t> input;
  int64_t insnPtr = 0;
  int64_t relPtr  = 0;
  int64_t out     = 0;

  int64_t& GetArg(const ParamMode mode) {
    switch (mode) {
    case ParamMode::POSITION:
      return mem[mem[insnPtr++]];
    case ParamMode::IMMEDIATE:
      return mem[insnPtr++];
    case ParamMode::RELATIVE:
      return mem[mem[insnPtr++] + relPtr];
    }
  }

  std::pair<int64_t, int64_t> GetArgs(const Insn& op) {
    auto a = GetArg(op.ParamA());
    auto b = GetArg(op.ParamB());
    return {a, b};
  }

  void Add(const Insn& op) {
    auto [a, b]         = GetArgs(op);
    GetArg(op.ParamC()) = a + b;
  }

  void Multiply(const Insn& op) {
    auto [a, b]         = GetArgs(op);
    GetArg(op.ParamC()) = a * b;
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
    GetArg(op.ParamC()) = a < b;
  }

  void Equals(const Insn& op) {
    auto [a, b]         = GetArgs(op);
    GetArg(op.ParamC()) = a == b;
  }

  void AdjustRelPtr(const Insn& op) { relPtr += GetArg(op.ParamA()); }

  void Store(int64_t input, const Insn& op) { GetArg(op.ParamA()) = input; }

  int64_t Load(const Insn& op) { return GetArg(op.ParamA()); }

 public:
  IntCodeComputer(std::vector<int64_t> mem) : mem{std::move(mem)} {}

  [[nodiscard]] int64_t Out() const noexcept { return out; }

  IntCodeComputer& PushInput(int64_t val) {
    input.push(val);
    return *this;
  }

  [[nodiscard]] ExecState Execute() {
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
        if (input.empty())
          return ExecState::NEED_INPUT;
        Store(input.front(), op);
        input.pop();
        break;
      case 4:
        out = Load(op);
        return ExecState::HAS_OUTPUT;
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
      case 9:
        AdjustRelPtr(op);
        break;
      case 99:
        return ExecState::HALTED;
      }
    }
  }
};

class SensorBoost : public AoC::Solver<int64_t, int64_t> {
  std::vector<int64_t> mem;

 public:
  SensorBoost(std::istream& in, const std::vector<std::string>&)
    : mem{AoC::StreamToContainer<decltype(mem)>(in, ',')} {}

  int64_t SolvePart1() {
    // 640K ought to be enough for anybody - Bill Gates (but not really)
    std::vector<int64_t> space(81920, 0);
    for (auto i = 0; i < mem.size(); ++i)
      space[i] = mem[i];
    IntCodeComputer comp{std::move(space)};
    comp.PushInput(1);
    if (comp.Execute() == ExecState::HAS_OUTPUT)
      return comp.Out();
    throw std::runtime_error{"Error getting output."};
  }

  int64_t SolvePart2() {
    std::vector<int64_t> space(81920, 0);
    for (auto i = 0; i < mem.size(); ++i)
      space[i] = mem[i];
    IntCodeComputer comp{std::move(space)};
    comp.PushInput(2);
    if (comp.Execute() == ExecState::HAS_OUTPUT)
      return comp.Out();
    throw std::runtime_error{"Error getting output."};
  }

  [[nodiscard]] Results Solve() override {
    return {SolvePart1(), SolvePart2()};
  }
};

int main(int argc, const char* argv[]) {
  return AoC::main<SensorBoost>(argc, argv);
}
