#pragma once
#include "Wrapper/Pipeline/Pipeline_base.hpp"
namespace MCRT {
class Compute_Pipeline : public Pipeline_base {

public:
    Compute_Pipeline();
    vk::PipelineLayout get_layout()
    {
        return layout;
    }

private:
    vk::PipelineLayout layout;
};

}