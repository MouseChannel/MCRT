#pragma once

#include <Helper/Instance_base.hpp>
#include <vector>
#include <Helper/Uniform_Manager.hpp>

namespace MCRT {

    class Image;

    class ComputeContext;

    class NoiseManager : public Instance_base<NoiseManager> {
    public:
        void InitPerlinNoise();

        void pre_compute_PerlinNoise();
        auto get_noise_image(){
            return perlin_noise;
        }
        auto get_permutations(){
            return permutations;
        }
    private:
        void make_permutations();

        std::shared_ptr<ComputeContext> context;
        std::shared_ptr<Image> perlin_noise;
        int noise_size = 128;
        std::shared_ptr<Uniform_Stuff<int>> permutations;
        // std::vector<int> permutations;
    };
}
