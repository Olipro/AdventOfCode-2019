#include "util/Core.h"

#include <cmath>
#include <map>
#include <set>
#include <unordered_set>

struct Point {
  int32_t x;
  int32_t y;
  Point(int32_t x, int32_t y) : x{x}, y{y} {}
  Point() = default;
  bool operator==(const Point& rhs) const noexcept {
    return x == rhs.x && y == rhs.y;
  }
};

// For Part2
class Asteroid {
  float angle;
  float distance;
  Point coords;

  static float GetAngle(const Point& base, const Point& coords) noexcept {
    float rads = std::atan2(base.x - coords.x, base.y - coords.y);
    if (rads < 0)
      rads = std::abs(rads);
    else
      rads = 2.0 * M_PI - rads;
    float deg = (rads * (180.0 / M_PI));
    return deg == 360 ? 0 : deg;
  }

  static float GetDistance(const Point& base, const Point& coords) noexcept {
    auto dx = std::pow(base.x - coords.x, 2);
    auto dy = std::pow(base.y - coords.y, 2);
    return std::sqrt(dx + dy);
  }

 public:
  Asteroid(const Point& base, const Point& coords) noexcept
    : angle{GetAngle(base, coords)},
      distance{GetDistance(base, coords)},
      coords{coords} {}
  [[nodiscard]] int32_t X() const noexcept { return coords.x; }
  [[nodiscard]] int32_t Y() const noexcept { return coords.y; }
  [[nodiscard]] float Angle() const noexcept { return angle; }
  [[nodiscard]] float Distance() const noexcept { return distance; }
  bool operator<(const Asteroid& rhs) const noexcept {
    return distance < rhs.distance || angle < rhs.angle;
  }
  bool operator==(const Asteroid& rhs) const noexcept {
    return coords == rhs.coords;
  }
};

class MonitoringStation : public AoC::Solver<int32_t, uint32_t> {
  std::vector<Point> asteroids;
  Point baseLocation;

 public:
  MonitoringStation(std::istream& in, const std::vector<std::string>&) {
    auto x = 0, y = 0;
    while (in && !in.eof()) {
      switch (in.get()) {
      case '#':
        asteroids.emplace_back(x, y);
        [[fallthrough]];
      case '.':
        ++x;
        break;
      case '\n':
        ++y;
        x = 0;
      }
    }
  }

  int32_t SolvePart1() {
    auto bestTotal = 0;
    for (auto& astBase : asteroids) {
      std::unordered_set<float> angles;
      for (auto& astChk : asteroids) {
        if (astBase == astChk)
          continue;
        auto angle = std::atan2(astBase.y - astChk.y, astBase.x - astChk.x);
        angles.emplace(angle);
      }
      if (angles.size() > bestTotal) {
        bestTotal    = angles.size();
        baseLocation = astBase;
      }
    }
    return bestTotal;
  }

  auto FindDestroyableAsteroid(float deg, std::set<Asteroid>& asteroids) {
    std::map<Asteroid, std::set<Asteroid>::iterator> candidates;
    for (auto asteroid = asteroids.begin(); asteroid != asteroids.end();
         ++asteroid) {
      if (asteroid->Angle() == deg)
        candidates.emplace(*asteroid, asteroid);
    }
    return candidates.empty() ? asteroids.end() : candidates.begin()->second;
  }

  int32_t SolvePart2() {
    std::set<Asteroid> toDestroy;
    for (auto& ast : asteroids) {
      if (ast == baseLocation)
        continue;
      toDestroy.emplace(baseLocation, ast);
    }
    auto destroyed = 0;
    while (true) {
      std::set<float> degs;
      for (auto& ast : toDestroy)
        degs.emplace(ast.Angle());
      for (auto deg = degs.begin(); deg != degs.end(); ++deg) {
        if (auto iter = FindDestroyableAsteroid(*deg, toDestroy);
            iter != toDestroy.end())
          toDestroy.erase(iter), ++destroyed;
        if (destroyed == 199) {
          deg       = ++deg == degs.end() ? degs.begin() : deg;
          auto iter = FindDestroyableAsteroid(*deg, toDestroy);
          return (iter->X() * 100) + iter->Y();
        }
      }
    }
  }

  Results Solve() override { return {SolvePart1(), SolvePart2()}; }
};

int main(int argc, const char* argv[]) {
  return AoC::main<MonitoringStation>(argc, argv);
}
