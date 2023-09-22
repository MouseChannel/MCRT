#include "Helper/Camera.hpp"
#include "Rendering/Context.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MoCheng3D {

    Camera::Camera() {

        m_position = glm::vec3(0.0f, -1.0f, -10.0f);
        m_front = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
        m_up = glm::vec3(1.0f, 0.0f, 0.0f);
        auto extent = Context::Get_Singleton()->Get_SwapChain()->Get_Extent2D();
        m_pMatrx = glm::perspective(glm::radians(60.0f),
                                    float(extent.width) / float(extent.height),
                                    0.1f, 66.0f);
        m_vMatrix = glm::mat4(1.0f);
        Update();
    }

    void Camera::Update() {
        m_vMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
    }
}