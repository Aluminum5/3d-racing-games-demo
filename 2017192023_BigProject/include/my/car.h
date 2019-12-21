#ifndef CAR_H
#define CAR_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ���������ƶ��ļ������ܵ�ѡ��
enum Direction {
    CAR_FORWARD,
    CAR_BACKWARD,
    CAR_LEFT,
    CAR_RIGHT
};


class Car {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    float Yaw;

    float MovementSpeed;
    float TurningSpeed;

    Car(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f))
        : MovementSpeed(20.0f)
        , TurningSpeed(80.0f)
        , Yaw(0.0f)
    {
        Position = position;
        updateFront();
    }

    glm::vec3 getPosition()
    {
        return Position;
    }
    float getYaw()
    {
        return Yaw;
    }

    // ������ͼ����
    glm::mat4 GetViewMatrix(glm::vec3 cameraPosition)
    {
        return glm::lookAt(cameraPosition, Position, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // ���ܼ�������
    void ProcessKeyboard(Direction direction, float deltaTime)
    {
        if (direction == CAR_FORWARD)
            Position += Front * MovementSpeed * deltaTime;
        if (direction == CAR_BACKWARD)
            Position -= Front * MovementSpeed * deltaTime;
        if (direction == CAR_LEFT)
            Yaw += TurningSpeed * deltaTime;
        if (direction == CAR_RIGHT)
            Yaw -= TurningSpeed * deltaTime;
        updateFront();
    }

private:
    // �����µ� Front ����
    void updateFront()
    {
        glm::vec3 front;
        front.x = -sin(glm::radians(Yaw));
        front.y = 0.0f;
        front.z = -cos(glm::radians(Yaw));
        Front = glm::normalize(front);
    }
};

#endif