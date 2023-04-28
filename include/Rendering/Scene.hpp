#pragma once
#include <vector>
#include <memory>
namespace MCRT {
class Model;
class Scene {
public:
    Scene();

private:
    std::vector<std::shared_ptr<Model>> models;
};
}