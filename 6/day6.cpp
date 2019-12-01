#include "util/Core.h"

#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <unordered_set>

class CelestialBody {
 public:
  using cref_t = std::reference_wrapper<const CelestialBody>;

 private:
  mutable std::optional<cref_t> parent;
  std::string name;

 public:
  CelestialBody(std::string_view name) : name{name} {}

  void SetParent(const CelestialBody& parent) const {
    this->parent = std::ref(parent);
  }

  const std::string& Name() const noexcept { return name; }

  const std::optional<cref_t>& Parent() const noexcept { return parent; }

  bool operator==(const CelestialBody& rhs) const noexcept {
    return name == rhs.name;
  }

  struct Hash {
    std::size_t operator()(const CelestialBody& cb) const noexcept {
      return std::hash<std::string>{}(cb.Name());
    }
  };
};

class CelestialOrbits : public AoC::Solver<uint32_t, uint32_t> {
  std::unordered_set<CelestialBody, CelestialBody::Hash> bodies;

  uint32_t CalculateOrbits(const std::optional<CelestialBody::cref_t>& parent) {
    return parent ? 1 + CalculateOrbits(parent->get().Parent()) : 0;
  }

  static uint32_t FindCommonParent(const CelestialBody& a,
                                   const CelestialBody& b) {
    auto aJumps = 0;
    for (auto aUp = a.Parent(); aUp; aUp = aUp->get().Parent(), ++aJumps) {
      auto bJumps = 0;
      for (auto bUp = b.Parent(); bUp; bUp = bUp->get().Parent(), ++bJumps) {
        if (aUp->get() == bUp->get())
          return aJumps + bJumps;
      }
    }
    return 0;
  }

 public:
  CelestialOrbits(std::istream& in, const std::vector<std::string>&) {
    std::string line;
    while (std::getline(in, line)) {
      auto sep = line.find(')');
      std::string_view a{line.data(), sep};
      std::string_view b{line.data() + sep + 1, line.size() - sep - 1};
      auto parent = bodies.emplace(a);
      auto child  = bodies.emplace(b);
      child.first->SetParent(*parent.first);
    }
  }

  [[nodiscard]] Results Solve() {
    auto a = 0;
    for (auto& body : bodies)
      a += CalculateOrbits(body.Parent());
    auto& YOU = *bodies.find({"YOU"});
    auto& SAN = *bodies.find({"SAN"});
    auto b    = FindCommonParent(YOU, SAN);
    return {a, b};
  }
};

int main(int argc, const char* argv[]) {
  return AoC::main<CelestialOrbits>(argc, argv);
}
