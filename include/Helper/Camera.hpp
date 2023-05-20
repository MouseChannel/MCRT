#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace MCRT {
// enum KEY {
//     forward = GLFW_KEY_UP,
//     back = GLFW_KEY_DOWN,
//     left = GLFW_KEY_LEFT,
//     right = GLFW_KEY_RIGHT
// };
enum class CAMERA_MOVE { MOVE_LEFT,
                         MOVE_RIGHT,
                         MOVE_FRONT,
                         MOVE_BACK };

class Camera {
public:
    Camera();
    [[nodiscard("missing v_matrix")]] auto& Get_v_matrix()
    {
        return m_vMatrix;
    }
    [[nodiscard("missing p_matrix")]] auto& Get_p_matrix()
    {
        return m_pMatrx;
    }

    [[nodiscard]] auto get_pos()
    {
        return m_position;
    }
    [[nodiscard]] auto get_front()
    {
        return m_front;
    }
    [[nodiscard]] auto get_fov_angel()
    {
        return m_fov_angel;
    }
    void init();
    void lookAt(glm::vec3 _pos, glm::vec3 _front, glm::vec3 _up);
    // update view_matrix
    void update();

    void setSpeed(float _speed)
    {
        m_speed = _speed;
    }
    void move_update();
    void move(CAMERA_MOVE _mode);

    void pitch(float _yOffset);
    void yaw(float _xOffset);
    void setSentitivity(float _s);
    void onMouseMove(double _xpos, double _ypos);

    void setPerpective(float angle, float ratio, float near, float far);

private:
    float m_speed = 1.0f;
    glm::vec3 m_position { 0, 2, 20 };
    glm::vec3 m_front { 0, 0, -1 };
    glm::vec3 m_up { 0, 1, 0 };
    glm::mat4 m_vMatrix;
    glm::mat4 m_pMatrx;
    // int cur_

    float m_pitch { 0 };
    float m_fov_angel { 60 };
    float m_yaw { -90 };
    float m_sensitivity { 0.1 };
    float m_xpos { 0 };
    float m_ypos { 0 };
    bool m_firstMove;
};

}