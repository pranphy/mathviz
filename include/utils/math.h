#pragma once 

#include <print>
#include <random>
#include <ranges>

class BinnedInverseSampler {
private:
    std::vector<float> cdf_;
    float x_min_, x_max_, dx_;
    int num_bins_;

public:

    template <typename Func>
    BinnedInverseSampler(Func f, float x_min, float x_max, int num_bins = 1000)
        : x_min_(x_min), x_max_(x_max), num_bins_(num_bins) {
        
        dx_ = (x_max - x_min) / num_bins_;
        std::vector<float> pdf(num_bins_);
        cdf_.resize(num_bins_ + 1);

        // 1. Evaluate f(x) at each bin center
        float sum = 0.0;
        for (int i = 0; i < num_bins_; ++i) {
            float x = x_min_ + (i + 0.5) * dx_;
            pdf[i] = std::max(static_cast<float>(0.0), static_cast<float>(f(x))); // Ensure non-negative
            sum += pdf[i];
        }

        // 2. Build normalized CDF
        cdf_[0] = 0.0;
        for (int i = 0; i < num_bins_; ++i) {
            cdf_[i + 1] = cdf_[i] + (pdf[i] / sum);
        }
        cdf_.back() = 1.0; // Fix floating-point rounding at the exact end
    }

    // Generate a sample using uniform random lookup + linear interpolation
    template <typename URNG>
    float sample(URNG& gen) {
        std::uniform_real_distribution<float> uniform(0.0, 1.0);
        float u = uniform(gen);

        // Find the interval using binary search ($O(\log N)$)
        auto it = std::lower_bound(cdf_.begin(), cdf_.end(), u);
        int idx = std::distance(cdf_.begin(), it) - 1;

        if (idx < 0) idx = 0;
        if (idx >= num_bins_) idx = num_bins_ - 1;

        float cdf_low = cdf_[idx];
        float cdf_high = cdf_[idx + 1];
        float fraction = (cdf_high > cdf_low) ? (u - cdf_low) / (cdf_high - cdf_low) : 0.0;

        float x_low = x_min_ + idx * dx_;
        return x_low + fraction * dx_;
    }

};

template <typename F>
std::vector<float> sample(F func, float x_min, float x_max, int num_bins) {
    BinnedInverseSampler sampler(func, x_min, x_max, 2*num_bins);

    std::random_device rd;
    std::mt19937 gen(rd());

     auto v = std::views::iota(0, num_bins) | std::views::transform([&sampler,&gen](int) {
         return sampler.sample(gen);
     }) | std::ranges::to<std::vector<float>>();
    return v;

}

unsigned long long fct(unsigned long long n);
std::vector<float> linspace(float start, float end, int N);
