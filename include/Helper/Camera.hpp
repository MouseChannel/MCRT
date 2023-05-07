#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    // void Update();

    // void
    // move(KEY key);
    void lookAt(glm::vec3 _pos, glm::vec3 _front, glm::vec3 _up);
    void update();

    glm::mat4 getViewMatrix();

    glm::mat4 getProjectMatrix();

    void setSpeed(float _speed)
    {
        m_speed = _speed;
    }

    void move(CAMERA_MOVE _mode);

    void pitch(float _yOffset);
    void yaw(float _xOffset);
    void setSentitivity(float _s);
    void onMouseMove(double _xpos, double _ypos);

    void setPerpective(float angle, float ratio, float near, float far);

private:
    float m_speed = 1.0f;
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::mat4 m_vMatrix;
    glm::mat4 m_pMatrx;

    float m_pitch;
    float m_yaw;
    float m_sensitivity;
    float m_xpos;
    float m_ypos;
    bool m_firstMove;
};

}