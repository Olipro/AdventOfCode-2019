#include "util/Core.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include <vector>

struct Point {
  int32_t x = 0;
  int32_t y = 0;
  bool operator==(const Point& rhs) const noexcept {
    return x == rhs.x && y == rhs.y;
  }
};

class WirePiece {
  Point start;
  Point end;
  bool isInv;

 public:
  WirePiece(Point start, Point end, bool isInv = false)
    : start{start}, end{end}, isInv{isInv} {}
  bool operator==(const WirePiece& rhs) const noexcept {
    if (isInv != rhs.isInv)
      return start == rhs.end && end == rhs.start;
    return start == rhs.start && end == rhs.end;
  }
  std::optional<Point> Intersects(const WirePiece& rhs) const noexcept {
    std::optional<Point> ret;
    const auto& trueStart    = isInv ? end : start;
    const auto& trueEnd      = isInv ? start : end;
    const auto& trueRhsStart = rhs.isInv ? rhs.end : rhs.start;
    const auto& trueRhsEnd   = rhs.isInv ? rhs.start : rhs.end;
    if (trueStart.x == trueEnd.x)
      if (trueRhsStart.x <= trueStart.x && trueRhsEnd.x >= trueEnd.x)
        if (trueStart.y <= trueRhsStart.y && trueEnd.y >= trueRhsEnd.y)
          ret = Point{trueStart.x, trueRhsStart.y};
    if (trueStart.y == trueEnd.y)
      if (trueRhsStart.y <= trueStart.y && trueRhsEnd.y >= trueEnd.y)
        if (trueStart.x <= trueRhsStart.x && trueEnd.x >= trueRhsStart.x)
          ret = Point{trueRhsStart.x, trueStart.y};
    return !ret || *ret == Point{0, 0} ? std::nullopt : ret;
  }

  const Point& Start() const noexcept { return start; }
  const Point& End() const noexcept { return end; }
};

class CrossedWires : public AoC::Solver<uint32_t, uint32_t> {
  std::vector<WirePiece> wireA;
  std::vector<WirePiece> wireB;
  static Point Up(std::vector<WirePiece>& wire, int32_t dist, Point curLoc) {
    wire.emplace_back(curLoc, Point{curLoc.x, curLoc.y - dist}, true);
    curLoc.y -= dist;
    return curLoc;
  }
  static Point Down(std::vector<WirePiece>& wire, int32_t dist, Point curLoc) {
    wire.emplace_back(curLoc, Point{curLoc.x, curLoc.y + dist});
    curLoc.y += dist;
    return curLoc;
  }
  static Point Left(std::vector<WirePiece>& wire, int32_t dist, Point curLoc) {
    wire.emplace_back(curLoc, Point{curLoc.x - dist, curLoc.y}, true);
    curLoc.x -= dist;
    return curLoc;
  }
  static Point Right(std::vector<WirePiece>& wire, int32_t dist, Point curLoc) {
    wire.emplace_back(curLoc, Point{curLoc.x + dist, curLoc.y});
    curLoc.x += dist;
    return curLoc;
  }
  static uint32_t Distance(const Point& a, const Point& b) noexcept {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
  }

  static void ProcessWire(std::vector<WirePiece>& wire, std::istream& in) {
    char dir      = 0;
    uint32_t dist = 0;
    Point curLoc;
    while (in) {
      in >> dir;
      in >> dist;
      switch (dir) {
      case 'U':
        curLoc = Up(wire, dist, curLoc);
        break;
      case 'D':
        curLoc = Down(wire, dist, curLoc);
        break;
      case 'L':
        curLoc = Left(wire, dist, curLoc);
        break;
      case 'R':
        curLoc = Right(wire, dist, curLoc);
        break;
      }
      if (in.get() == '\n')
        return;
    }
  }
  uint32_t SolvePart1() {
    auto dist = std::numeric_limits<uint32_t>::max();
    for (const auto& a : wireA) {
      for (const auto& b : wireB) {
        auto intersection = a.Intersects(b);
        if (intersection)
          dist = std::min(dist, Distance(*intersection, {0, 0}));
      }
    }
    return dist;
  }
  uint32_t SolvePart2() {
    int32_t aSteps = 0;
    uint32_t ret   = std::numeric_limits<uint32_t>::max();
    for (const auto& a : wireA) {
      int32_t bSteps = 0;
      for (const auto& b : wireB) {
        auto intersection = a.Intersects(b);
        if (intersection)
          ret = std::min(ret,
                         (aSteps + bSteps + Distance(*intersection, a.Start()) +
                          Distance(*intersection, b.Start())));
        bSteps += Distance(b.Start(), b.End());
      }
      aSteps += Distance(a.Start(), a.End());
    }
    return ret;
  }

 public:
  CrossedWires(std::istream& in, const std::vector<std::string>&) {
    ProcessWire(wireA, in);
    ProcessWire(wireB, in);
  }
  [[nodiscard]] Results Solve() override {
    return {SolvePart1(), SolvePart2()};
  }
};

int main(int argc, const char* argv[]) {
  return AoC::main<CrossedWires>(argc, argv);
}
