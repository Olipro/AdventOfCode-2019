#include "util/Core.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>
#include <unordered_set>

class Moon {
  int64_t x;
  int64_t y;
  int64_t z;
  int64_t xVel = 0;
  int64_t yVel = 0;
  int64_t zVel = 0;

  static int64_t CalcVelocity(int64_t us, int64_t them) {
    if (us > them)
      return -1;
    if (us < them)
      return 1;
    return 0;
  }

 public:
  Moon(int64_t x, int64_t y, int64_t z) : x{x}, y{y}, z{z} {}

  Moon& SetVelocity(const Moon& other) noexcept {
    xVel += CalcVelocity(x, other.x);
    yVel += CalcVelocity(y, other.y);
    zVel += CalcVelocity(z, other.z);
    return *this;
  }

  Moon& Move() noexcept {
    x += xVel;
    y += yVel;
    z += zVel;
    return *this;
  }

  int64_t X() const noexcept { return x; }
  int64_t Y() const noexcept { return y; }
  int64_t Z() const noexcept { return z; }
  int64_t XVel() const noexcept { return xVel; }
  int64_t YVel() const noexcept { return yVel; }
  int64_t ZVel() const noexcept { return zVel; }
  int64_t PotentialEnergy() const noexcept {
    return std::abs(x) + std::abs(y) + std::abs(z);
  }
  int64_t KineticEnergy() const noexcept {
    return std::abs(xVel) + std::abs(yVel) + std::abs(zVel);
  }
  int64_t TotalEnergy() const noexcept {
    return PotentialEnergy() * KineticEnergy();
  }
};

class NBodyProblem : public AoC::Solver<uint64_t, uint64_t> {
  std::vector<Moon> moons;

  static Moon GetMoon(std::istream& in) {
    int64_t x, y, z;
    in.seekg(3, std::istream::cur);
    in >> x;
    in.seekg(4, std::istream::cur);
    in >> y;
    in.seekg(4, std::istream::cur);
    in >> z;
    in.seekg(2, std::istream::cur);
    return {x, y, z};
  }

  static void Tick(std::vector<Moon>& moons) {
    for (auto moon = moons.begin(); moon != moons.end(); ++moon) {
      for (auto other = moons.begin(); other != moons.end(); ++other) {
        if (moon == other)
          continue;
        moon->SetVelocity(*other);
      }
    }
    for (auto& moon : moons)
      moon.Move();
  }

 public:
  NBodyProblem(std::istream& in, const std::vector<std::string>&) {
    while (true) {
      auto moon = GetMoon(in);
      if (!in)
        break;
      moons.emplace_back(moon);
    }
  }

  int64_t SolvePart1() {
    auto moons = this->moons;
    for (auto x = 0; x < 1000; ++x)
      Tick(moons);
    auto ret = 0;
    for (auto& moon : moons)
      ret += moon.TotalEnergy();
    return ret;
  }

  template <class Pred>
  static int64_t FindRepeat(std::vector<Moon> moons, Pred pred) {
    auto cnt        = 0;
    const auto init = moons;
    do {
      Tick(moons);
      ++cnt;
    } while (!std::equal(
      init.begin(), init.end(), moons.cbegin(), moons.cend(), pred));
    return cnt;
  }

  int64_t SolvePart2() {
    auto xCnt = FindRepeat(moons, [](auto& a, auto& b) {
      return a.X() == b.X() && a.XVel() == b.XVel();
    });
    auto yCnt = FindRepeat(moons, [](auto& a, auto& b) {
      return a.Y() == b.Y() && a.YVel() == b.YVel();
    });
    auto zCnt = FindRepeat(moons, [](auto& a, auto& b) {
      return a.Z() == b.Z() && a.ZVel() == b.ZVel();
    });
    return std::lcm(std::lcm(xCnt, yCnt), zCnt);
  }

  Results Solve() override { return {SolvePart1(), SolvePart2()}; }
};

int main(int argc, const char* argv[]) {
  return AoC::main<NBodyProblem>(argc, argv);
}
