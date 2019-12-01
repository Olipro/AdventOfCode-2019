#include "util/Core.h"

#include <cstdint>
#include <functional>
#include <queue>
#include <utility>

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
  explicit Insn(int32_t op) {
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

enum class ExecState { NEED_INPUT, HAS_OUTPUT, HALTED };

class IntCodeComputer {
  std::vector<int32_t> mem;
  std::queue<int32_t> input;
  int32_t insnPtr = 0;
  int32_t out     = 0;

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

  static void Store(int32_t input, int32_t& dst) { dst = input; }

  int32_t Load(const Insn& op, const int32_t* arg) {
    return op.ParamA() == ParamMode::POSITION ? mem[*arg] : *arg;
  }

 public:
  IntCodeComputer(std::vector<int32_t> mem) : mem{std::move(mem)} {}

  [[nodiscard]] int32_t Out() const noexcept { return out; }

  IntCodeComputer& PushInput(int32_t val) {
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
        Store(input.front(), mem[mem[insnPtr++]]);
        input.pop();
        break;
      case 4:
        out = Load(op, mem.data() + insnPtr++);
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
      case 99:
        return ExecState::HALTED;
      }
    }
  }
};

class IntCode : public AoC::Solver<int32_t, int32_t> {
  std::vector<int32_t> mem;

  template <class Iter>
  constexpr bool DigitUsed(int digit, Iter begin, Iter end) {
    while (begin != end) {
      if (*begin++ == digit) {
        return true;
      }
    }
    return false;
  }

  template <size_t N, class Func, class Iter>
  constexpr void HandleDigit(int min,
                             int max,
                             Func&& func,
                             std::array<int32_t, N>& arr,
                             Iter iter) {
    if (iter == arr.end())
      return func(arr);
    for (auto i = min; i <= max; ++i) {
      if (!DigitUsed(i, arr.begin(), iter)) {
        *iter = i;
        HandleDigit(min, max, std::forward<Func>(func), arr, iter + 1);
      }
    }
  }

  template <int Min, int Max, class Func>
  constexpr void ForEachCombination(Func&& func) {
    std::array<int32_t, 1 + Max - Min> digits{};
    for (auto i = Min; i <= Max; ++i) {
      digits[0] = i;
      HandleDigit(
        Min, Max, std::forward<Func>(func), digits, digits.begin() + 1);
    }
  }

  template <class CompArr, class ValArr>
  int32_t TryLoop2(CompArr& comps, ValArr& vals) {
    for (auto i = 0; i < comps.size(); ++i)
      comps[i].PushInput(vals[i]);
    comps.front().PushInput(0);
    for (auto state = ExecState::NEED_INPUT; state != ExecState::HALTED;) {
      for (auto iter = comps.begin(); iter != comps.end(); ++iter) {
        if ((state = iter->Execute()) == ExecState::HAS_OUTPUT) {
          if ((iter + 1) == comps.end()) {
            comps.front().PushInput(iter->Out());
          } else {
            (iter + 1)->PushInput(iter->Out());
          }
        }
      }
    }
    return comps.back().Out();
  }

  int32_t SolvePart1() {
    int32_t max = 0;
    ForEachCombination<0, 4>([this, &max](auto& arr) {
      auto comps = AoC::make_array<IntCodeComputer, 5>(mem);
      for (auto i = 0; i < arr.size(); ++i)
        comps[i].PushInput(arr[i]);
      comps.front().PushInput(0);
      for (auto i = comps.begin(); i != comps.end(); ++i) {
        if (i->Execute() == ExecState::HAS_OUTPUT && (i + 1) != comps.end())
          (i + 1)->PushInput(i->Out());
      }
      max = std::max(max, comps.back().Out());
    });
    return max;
  }

  int32_t SolvePart2() {
    int32_t max = 0;
    ForEachCombination<5, 9>([this, &max](auto& arr) {
      auto comps = AoC::make_array<IntCodeComputer, 5>(mem);
      max        = std::max(max, TryLoop2(comps, arr));
    });
    return max;
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
