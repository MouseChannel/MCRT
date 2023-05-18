#include "Helper/Camera.hpp"
#include "Rendering/Context.hpp"
#include "Rendering/GLFW_Window.hpp"
#include "Rendering/RT_Context.hpp"
#include "iostream"

namespace MCRT {

Camera::Camera()
{
}
void Camera::init()
{

    auto window = Context::Get_Singleton()->get_window();
    glfwSetWindowUserPointer(window->get_handle(), Context::Get_Singleton()->get_camera().get());
    glfwSetCursorPosCallback(window->get_handle(), [](GLFWwindow* window, double xpos, double ypos) {
        auto camera = (Camera*)glfwGetWindowUserPointer(window);
        camera->onMouseMove(xpos, ypos);
    });

    setPerpective(60, 1, 0.1f, 100000);

    m_vMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::lookAt(glm::vec3 _pos, glm::vec3 _front, glm::vec3 _up)
{
    m_position = _pos;
    m_front = glm::normalize(_front);
    m_up = _up;
    update();
    // m_vMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::update()
{
    m_vMatrix = glm::lookAt(m_position, m_position + m_front, m_up);

    Context::Get_Singleton()->get_rt_context()->reset();
}

void Camera::move_update()
{
    if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_W) == GLFW_PRESS) {
        move(CAMERA_MOVE::MOVE_FRONT);
    }

    if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_S) == GLFW_PRESS) {
        move(CAMERA_MOVE::MOVE_BACK);
    }

    if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_A) == GLFW_PRESS) {
        move(CAMERA_MOVE::MOVE_LEFT);
    }

    if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_D) == GLFW_PRESS) {
        move(CAMERA_MOVE::MOVE_RIGHT);
    }
    if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_Q) == GLFW_PRESS) {
        yaw(-1);
        update();
    }
    if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_E) == GLFW_PRESS) {
        yaw(1);
        update();
    }
}
void Camera::move(CAMERA_MOVE _mode)
{
    // std::cout << (int)_mode << std::endl;
    switch (_mode) {
    case CAMERA_MOVE::MOVE_LEFT:
        m_position -= glm::normalize(glm::cross(m_front, m_up)) * m_speed * m_sensitivity;
        break;
    case CAMERA_MOVE::MOVE_RIGHT:
        m_position += glm::normalize(glm::cross(m_front, m_up)) * m_speed * m_sensitivity;
        break;
    case CAMERA_MOVE::MOVE_FRONT:
        m_position += m_speed * m_front * m_sensitivity;
        break;
    case CAMERA_MOVE::MOVE_BACK:
        m_position -= m_speed * m_front * m_sensitivity;
        break;
    default:
        break;
    }

    update();
}

void Camera::pitch(float _yOffset)
{
    m_pitch += _yOffset * m_sensitivity;

    if (m_pitch >= 89.0f) {
        m_pitch = 89.0f;
    }

    if (m_pitch <= -89.0f) {
        m_pitch = -89.0f;
    }

    m_front.y = sin(glm::radians(m_pitch));
    m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_front = glm::normalize(m_front);
}
void Camera::yaw(float _xOffset)
{
    m_yaw += _xOffset * m_sensitivity;

    m_front.y = sin(glm::radians(m_pitch));
    m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_front = glm::normalize(m_front);
}
void Camera::setSentitivity(float _s)
{
    m_sensitivity = _s;
}

void Camera::setPerpective(float angle, float ratio, float near, float far)
{
    m_pMatrx = glm::perspective(glm::radians(angle), ratio, near, far);
}

void Camera::onMouseMove(double _xpos, double _ypos)
{
    return;

    // onMouseMove(m_xpos + 1.0, m_ypos);
    // std::cout << "on mouse 1" << std::endl;

    // std::cout << _xpos << ' ' << _ypos << std::endl;
    if (m_firstMove) {
        m_xpos = _xpos;
        m_ypos = _ypos;
        m_firstMove = false;
        return;
    }

    float _xOffset = _xpos - m_xpos;
    float _yOffset = -(_ypos - m_ypos);

    // std::cout << _xOffset << ' ' << _yOffset << std::endl;
    m_xpos = _xpos;
    m_ypos = _ypos;

    // pitch(_yOffset);
    // yaw(_xOffset);
    m_front = glm::normalize(glm::vec3(0) - m_position);
    update();
}
}