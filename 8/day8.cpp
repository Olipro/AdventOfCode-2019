#include "util/Core.h"

#include <algorithm>

class DSNDecoder : public AoC::Solver<uint32_t, std::string> {
  static constexpr auto WIDTH  = 25;
  static constexpr auto HEIGHT = 6;
  using Layer                  = std::array<int8_t, WIDTH * HEIGHT>;
  std::vector<Layer> image;

 public:
  DSNDecoder(std::istream& in, const std::vector<std::string>&) {
    while (!in.eof()) {
      Layer layer{};
      for (auto& pixel : layer)
        pixel = in.get() - 0x30;
      if (in)
        image.emplace_back(layer);
    }
  }

  std::string SolvePart2() {
    Layer composite;
    composite.fill(2);
    for (auto& layer : image) {
      for (auto i = 0; i < layer.size(); ++i) {
        if (composite[i] == 2)
          composite[i] = layer[i];
      }
    }
    std::string ret((WIDTH * HEIGHT) + HEIGHT, ' ');
    auto* chr = ret.data();
    for (auto col = 0; col < HEIGHT; ++col) {
      for (auto row = 0; row < WIDTH; ++row) {
        *chr++ = composite[(col * WIDTH) + row] == 1 ? '@' : ' ';
      }
      *chr++ = '\n';
    }
    return ret;
  }

  [[nodiscard]] Results Solve() override {
    const auto& layer1 = *std::min_element(
      image.begin(), image.end(), [](const auto& a, const auto& b) {
        return std::count(a.begin(), a.end(), 0) <
               std::count(b.begin(), b.end(), 0);
      });
    auto part1 = std::count(layer1.begin(), layer1.end(), 1) *
                 std::count(layer1.begin(), layer1.end(), 2);
    return {part1, SolvePart2()};
  }
};

int main(int argc, const char* argv[]) {
  return AoC::main<DSNDecoder>(argc, argv);
}