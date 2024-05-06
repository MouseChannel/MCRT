#include "Wrapper/SubPass/OpacitySubPass.hpp"
namespace MCRT {

OpacitySubPass::OpacitySubPass()
    : BaseSubPass(nullptr)
{
}
//void OpacitySubPass::prepare_renderTarget()
//{
//    swapChainTarget.reset(new SwapChainTarget);
//}
void OpacitySubPass::set_description()
{
    vk::AttachmentReference color_reference;
    //    color_reference.se
    //
    //    description.set
}
void OpacitySubPass::run_subpass()
{}

}