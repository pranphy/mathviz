#include "utils/math.h"

unsigned long long fct(unsigned long long n) {
  unsigned long long r = 1;
  for (unsigned long long i = 2; i <= n; ++i)
    r *= i; // may overflow for large n
  return r;
}

std::vector<float> linspace(float start, float end, int N) {
    auto v = std::views::iota(0, N) | std::views::transform([=](int i) {
        return start + static_cast<float>(i) * (end - start) / (N - 1);
    }) | std::ranges::to<std::vector<float>>(); // C++23 range collector
    return v;
}
