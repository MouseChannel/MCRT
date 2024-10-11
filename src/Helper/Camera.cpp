#include "Helper/Camera.hpp"
#include "Context/Context.hpp"
#include "Rendering/AppWindow.hpp"
#include "Rendering/RaytracingPass.hpp"
#include "Wrapper/SwapChain.hpp"
#include "imgui.h"
#include "iostream"


namespace MCRT {

Camera::Camera()
{
}

void Camera::init()
{

    auto window = Context::Get_Singleton()->get_window();

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    auto w = ANativeWindow_getWidth(Context::Get_Singleton()->get_window()->get_handle());

    auto h = ANativeWindow_getHeight(Context::Get_Singleton()->get_window()->get_handle());

#else
    glfwSetWindowUserPointer(window->get_handle(), Context::Get_Singleton()->get_camera().get());
    glfwSetCursorPosCallback(window->get_handle(),
                             [](GLFWwindow* window, double xpos, double ypos) {
                                 auto camera = (Camera*)glfwGetWindowUserPointer(window);
                                 camera->onMouseMove(xpos, ypos);
                             });
    glfwSetScrollCallback(window->get_handle(),
                          [](GLFWwindow* window, double xpos, double ypos) {
                              auto camera = (Camera*)glfwGetWindowUserPointer(window);
                              camera->onMouseScroll(xpos, ypos);
                          });
    int w, h;
    glfwGetFramebufferSize(window->get_handle(), &w, &h);
#endif
    //        auto extent = Context::Get_Singleton()->get_swapchain()->Get_Extent2D();
    setPerpective(m_fov_angel, (float)w / (float)h, 1e-2f, 1e6);

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
    // printf("out %f %f %f |%f %f %f| %f %f %f \n",m_vMatrix[0][0], m_vMatrix[0][1], m_vMatrix[0][2],

    //                m_vMatrix[1][0],
    //                m_vMatrix[1][1],
    //                m_vMatrix[1][2],
    //                m_vMatrix[2][0],
    //                m_vMatrix[2][1],
    //                m_vMatrix[2][2]);
    // std::cout << m_position.x<<m_position.y<<m_position.z << std::endl;
    Context::Get_Singleton()->reset();
}

void Camera::move_update()
{
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#else
    if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_W) == GLFW_PRESS) {
        move(CAMERA_MOVE::MOVE_FRONT);
        // std::cout<<"front"<<std::endl;
    }

    if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_S) == GLFW_PRESS) {
        move(CAMERA_MOVE::MOVE_BACK);
    }
    if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_E) == GLFW_PRESS) {
        move(CAMERA_MOVE::MOVE_TOP);
    }
    if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_Q) == GLFW_PRESS) {
        move(CAMERA_MOVE::MOVE_DOWN);
    }

    if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_A) == GLFW_PRESS) {
        move(CAMERA_MOVE::MOVE_LEFT);
    }

    if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_D) == GLFW_PRESS) {
        move(CAMERA_MOVE::MOVE_RIGHT);
    }
    // if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_LEFT) == GLFW_PRESS) {
    //     yaw(-1);
    //     update();
    // }
    // if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
    //     yaw(1);
    //     update();
    // }
    // if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_DOWN) == GLFW_PRESS) {
    //     pitch(-1);
    //     update();
    // }
    // if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_UP) == GLFW_PRESS) {
    //     pitch(1);
    //     update();
    // }

    // if (glfwGetKey(Context::Get_Singleton()->get_window()->get_handle(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    //     setSentitivity(1);
    // } else {
    //     setSentitivity(1e-2);
    // }
#endif
}

void Camera::move(CAMERA_MOVE _mode)
{
    // std::cout << (int)_mode << std::endl;
    glm::vec3 m_right = glm::cross(m_front, m_up);
    auto m_top = glm::cross(m_right, m_front);
    switch (_mode) {
    case CAMERA_MOVE::MOVE_LEFT:
        m_position -= glm::normalize(glm::cross(m_front, m_up)) * m_speed * m_sensitivity;
        break;

    case CAMERA_MOVE::MOVE_RIGHT:
        m_position += glm::normalize(glm::cross(m_front, m_up)) * m_speed * m_sensitivity;
        break;
    case CAMERA_MOVE::MOVE_TOP:
        m_position += m_speed * m_top * m_sensitivity;
        break;
    case CAMERA_MOVE::MOVE_DOWN:
        m_position -= m_speed * m_top * m_sensitivity;
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
    // std::cout << m_position.x << '|' << m_position.y << "|" << m_position.z << std::endl;

    update();
}

void Camera::pitch(float _yOffset)
{
    m_pitch += _yOffset * m_sensitivity;

    // if (m_pitch >= 89.0f) {
    //     m_pitch = 89.0f;
    // }

    // if (m_pitch <= -89.0f) {
    //     m_pitch = -89.0f;
    // }
    m_pitch = std::clamp(m_pitch, -89.f, 89.f);

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
    // std::cout << m_front.x << " |" << m_front.y << "| " << m_front.z << std::endl;
}

void Camera::setSentitivity(float _s)
{
    m_sensitivity = _s;
}

void Camera::setPerpective(float angle, float ratio, float near, float far)
{
    m_pMatrx = glm::perspective(glm::radians(angle), ratio, near, far);
    // m_pMatrx = glm::perspectiveFov(60.f, 1200.f, 800.f, near, far);


    m_pMatrx[1][1] *= -1.0f;
}

void Camera::setPerpectiveFOV(float fov, float width, float height, float near, float far)
{
    m_pMatrx = glm::perspectiveFov(fov, width, height, near, far);
    m_pMatrx[1][1] *= -1.0f;
}

#if defined(VK_USE_PLATFORM_ANDROID_KHR)

int32_t Camera::handleAppInput(struct android_app* app, AInputEvent* event)
{
    switch (AInputEvent_getType(event)) {
    case AINPUT_EVENT_TYPE_MOTION:
        switch (AInputEvent_getSource(event)) {

        case AINPUT_SOURCE_TOUCHSCREEN: {
            int32_t action = AMotionEvent_getAction(event);
            int32_t eventX = AMotionEvent_getX(event, 0);
            int32_t eventY = AMotionEvent_getY(event, 0);
            switch (action) {
            case AMOTION_EVENT_ACTION_UP:

                break;
            case AMOTION_EVENT_ACTION_DOWN:
                m_xpos = eventX;
                m_ypos = eventY;
                break;
            case AMOTION_EVENT_ACTION_MOVE:

                onMouseMove(eventX, eventY);
                ((void)__android_log_print(ANDROID_LOG_ERROR, "mocheng", "%d", eventX));
                ((void)__android_log_print(ANDROID_LOG_ERROR, "mocheng", "%d", eventY));

                break;
            }
        }
        }
        break;
    case AINPUT_EVENT_TYPE_KEY:
        int32_t keyCode = AKeyEvent_getKeyCode(event);
        switch (AKeyEvent_getKeyCode(event)) {
        case AKEYCODE_VOLUME_UP:
            //                        LOGE("volume up");
            m_position += m_front * 1e-1f;
            ;
            update();
            break;
        case AKEYCODE_VOLUME_DOWN:
            //                        LOGE("volume down");
            m_position -= m_front * 1e-1f;
            ;
            update();
            break;
        }

        break;
    }

    return 1;
}

#else

#endif

void Camera::onMouseMove(double _xpos, double _ypos)
{
    if (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse)
        return;

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    if (m_firstMove) {
        m_xpos = _xpos;
        m_ypos = _ypos;
        m_firstMove = false;
        return;
    }

    float _xOffset = m_xpos - _xpos;
    float _yOffset = m_ypos - _ypos;
    // std::cout << m_xpos << ' ' << m_ypos << std::endl;

    m_xpos = _xpos;
    m_ypos = _ypos;
    pitch(_yOffset * 10);
    yaw(_xOffset * 10);

    update();
#else
    if (m_firstMove) {
        m_xpos = _xpos;
        m_ypos = _ypos;
        m_firstMove = false;
        return;
    }

    float _xOffset = m_xpos - _xpos;
    float _yOffset = m_ypos - _ypos;

    m_xpos = _xpos;
    m_ypos = _ypos;
    switch (move_mode) {
    case MOVE_MODE::ORBIT:
        if (glfwGetMouseButton(Context::Get_Singleton()->get_window()->get_handle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            orbit(_xOffset * m_sensitivity * 10, _yOffset * m_sensitivity * 10);
        }
        break;
    case MOVE_MODE::FREE:
        if (glfwGetMouseButton(Context::Get_Singleton()->get_window()->get_handle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

            pitch(_yOffset);
            yaw(_xOffset);
            update();
        }
        if (glfwGetMouseButton(Context::Get_Singleton()->get_window()->get_handle(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            glm::vec3 m_right = glm::cross(m_front, m_up);
            auto m_left = -m_right;
            auto m_top = glm::cross(m_right, m_front);
            auto m_down = -m_top;

            m_position += m_right * _xOffset * 1e-3f + m_top * _yOffset * 1e-3f;
            ;
            update();
        }
        break;
    }

#endif
}

void Camera::onMouseScroll(double _xpos, double _ypos)
{

    if (_ypos > 0)

        m_position += m_speed * m_front * m_sensitivity * 1e-2f;
    else
        m_position -= m_speed * m_front * m_sensitivity * 1e-2f;

    update();
}

void Camera::orbit(float x_offset, float y_offset)
{
    x_offset *= -1;
    auto origin_pos = m_position;
    glm::vec3 look_point = m_position + m_front;
    vec4 cur_pos = { m_position, 1 };

    vec3 m_right = glm::cross(m_front, m_up);

    auto m_top = glm::cross(m_right, m_front);

    auto m_matrix = mat4(1);

    // rotate axe_y
    m_matrix = glm::rotate(m_matrix, glm::radians(x_offset), m_top);
    // rotate axe_x
    cur_pos = m_matrix * glm::vec4(-m_front, 1);
    m_front = glm::normalize(glm::vec3(cur_pos));
    if (glm::abs(m_front.y) < 0.95 || glm::sign(m_front.y) * y_offset < 0) {
        m_right = glm::cross(m_front, m_up);
        m_matrix = glm::rotate(m_matrix, glm::radians(y_offset), m_right);
        cur_pos = m_matrix * cur_pos;
    }
    cur_pos += glm::vec4(look_point, 1);
    m_front = look_point - glm::vec3(cur_pos);

    m_position = cur_pos;
    update();
}
}
