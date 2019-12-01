#include "util/Core.h"

#include <cstdint>
#include <queue>
#include <set>
#include <sstream>
#include <utility>
#include <unordered_set>

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
  IntCodeComputer(const std::vector<int64_t>& mem)
    : mem(std::max(81920UL, mem.size()), 0) {
    std::copy(mem.begin(), mem.end(), this->mem.begin());
  }

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

struct Point {
  int32_t x = 0;
  int32_t y = 0;
  Point(int32_t x, int32_t y) : x{x}, y{y} {}
  bool operator==(const Point& rhs) const noexcept {
    return x == rhs.x && y == rhs.y;
  }
  bool operator<(const Point& rhs) const noexcept {
    return y < rhs.y || (y == rhs.y && x < rhs.x);
  }
};

struct Panel {
  Point pos;
  Panel(int32_t x, int32_t y) : pos{x, y} {}
  bool operator==(const Panel& rhs) const noexcept { return pos == rhs.pos; }
  bool operator<(const Panel& rhs) const noexcept { return pos < rhs.pos; }
  auto X() const noexcept { return pos.x; }
  auto Y() const noexcept { return pos.y; }
  struct Hash {
    size_t operator()(const Panel& p) const noexcept {
      auto val = static_cast<uint64_t>(p.pos.x) << 32 | p.pos.y;
      return std::hash<uint64_t>{}(val);
    }
  };
};

enum class Direction {
  UP,
  DOWN,
  LEFT,
  RIGHT,
};

class Dir {
  Direction dir = Direction::UP;

 public:
  void TurnRight() {
    if (dir == Direction::UP)
      dir = Direction::RIGHT;
    else if (dir == Direction::DOWN)
      dir = Direction::LEFT;
    else if (dir == Direction::LEFT)
      dir = Direction::UP;
    else if (dir == Direction::RIGHT)
      dir = Direction::DOWN;
  }

  void TurnLeft() {
    if (dir == Direction::UP)
      dir = Direction::LEFT;
    else if (dir == Direction::DOWN)
      dir = Direction::RIGHT;
    else if (dir == Direction::LEFT)
      dir = Direction::DOWN;
    else if (dir == Direction::RIGHT)
      dir = Direction::UP;
  }

  bool operator==(const Direction& dir) const noexcept {
    return this->dir == dir;
  }
};

class PaintRobot : public AoC::Solver<int64_t, std::string> {
  std::vector<int64_t> mem;
  Point pos{0, 0};

 public:
  PaintRobot(std::istream& in, const std::vector<std::string>&)
    : mem{AoC::StreamToContainer<decltype(mem)>(in, ',')} {}

  template <class WhitePanels, class PaintedPanels>
  void Paint(WhitePanels& whitePanels, PaintedPanels& paintedPanels, int init) {
    IntCodeComputer robot{mem};
    robot.PushInput(init);
    auto x = 0, y = 0;
    Dir dir;
    while (robot.Execute() != ExecState::HALTED) {
      auto paintColour = robot.Out();
      if (robot.Execute() != ExecState::HAS_OUTPUT)
        throw std::logic_error{"IntCode in invalid state"};
      auto direction = robot.Out();
      paintedPanels.emplace(x, y);
      if (paintColour == 1)
        whitePanels.emplace(x, y);
      else
        whitePanels.erase(Panel{x, y});
      if (direction == 0)
        dir.TurnLeft();
      else
        dir.TurnRight();
      if (dir == Direction::UP)
        ++y;
      else if (dir == Direction::DOWN)
        --y;
      else if (dir == Direction::LEFT)
        --x;
      else if (dir == Direction::RIGHT)
        ++x;
      robot.PushInput(whitePanels.count(Panel{x, y}) ? 1 : 0);
    }
  }

  int64_t SolvePart1() {
    std::unordered_set<Panel, Panel::Hash> whitePanels;
    std::unordered_set<Panel, Panel::Hash> paintedPanels;
    Paint(whitePanels, paintedPanels, 0);
    return paintedPanels.size();
  }

  std::string SolvePart2() {
    std::unordered_set<Panel, Panel::Hash> whitePanels;
    std::set<Panel> paintedPanels;
    Paint(whitePanels, paintedPanels, 1);
    std::string ret;
    auto xMin = paintedPanels.begin()->X(), xMax = paintedPanels.rbegin()->X();
    auto yMin = paintedPanels.begin()->Y(), yMax = paintedPanels.rbegin()->Y();
    ret.reserve((xMax - xMin) * (yMax - yMin) + (yMax - yMin));
    for (auto y = yMax; y >= yMin; --y) {
      for (auto x = xMin; x <= xMax; ++x) {
        ret += whitePanels.count(Panel{x, y}) ? '@' : ' ';
      }
      ret += '\n';
    }
    return ret;
  }

  [[nodiscard]] Results Solve() override {
    return {SolvePart1(), SolvePart2()};
  }
};

int main(int argc, const char* argv[]) {
  return AoC::main<PaintRobot>(argc, argv);
}
