#include "util/Core.h"

#include <cstdint>

class FuelCalculator : public AoC::Solver<uint32_t, uint32_t> {
  std::istream& in;

  [[nodiscard]] static uint32_t CalculateFuelForMass(uint32_t mass) noexcept {
    mass = mass / 3;
    return mass > 2 ? mass - 2 : 0;
  }

 public:
  FuelCalculator(std::istream& in, std::vector<std::string>) : in{in} {}

  [[nodiscard]] Results Solve() override {
    uint32_t massFuel = 0, fuelForFuel = 0;
    AoC::ForEachInStream(in, 0U, [&massFuel, &fuelForFuel](uint32_t mass) {
      const auto fuel = CalculateFuelForMass(mass);
      massFuel += fuel;
      for (auto i = CalculateFuelForMass(fuel); i > 0;
           i      = CalculateFuelForMass(i))
        fuelForFuel += i;
    });
    return {massFuel, massFuel + fuelForFuel};
  }
};

int main(int argc, const char* argv[]) {
  return AoC::main<FuelCalculator>(argc, argv);
}
