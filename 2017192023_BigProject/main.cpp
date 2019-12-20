#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/camera.h>
#include <learnopengl/filesystem.h>
#include <learnopengl/model.h>
#include <learnopengl/shader_m.h>

#include <my/car.h>

#include <iostream>

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "assimp.lib")

// һЩ����
GLFWwindow* windowInit();
bool init();
void setDeltaTime();
void renderLight(Shader shader);
void renderCar(Model model, Shader shader);
void renderRaceTrack(Model model, Shader shader);
void setTransMatrix(Shader shader, glm::mat4 viewMatrix, glm::mat4 modelMatrix, glm::mat4 projMatrix);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// ���ڳߴ�
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Car car(glm::vec3(0.0f, 0.0f, 0.0f));

// camera
Camera camera(glm::vec3(0.0f, 50.0f, 0.0f));

// �������������Ļ����
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing ����ƽ�ⲻͬ������Ⱦˮƽ���������ٶȱ仯
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// --------
// main ����
int main()
{
    GLFWwindow* window = windowInit();
    bool isInit = init();
    // һЩ��ʼ��
    if (window == NULL || !isInit) {
        return -1;
    }

    // �����ͱ�����ɫ��
    Shader shader("shader/light.vs", "shader/light.fs");
    Shader simpleShader("shader/model_loading.vs", "shader/model_loading.fs");

    // ����ģ��
    // Model model(FileSystem::getPath("asset/model/obj/nanosuit/nanosuit.obj"));
    // Model model(FileSystem::getPath("asset/model/obj/roomdoor/Door_Component_BI3.obj"));
    
    // Model model(FileSystem::getPath("asset/model/obj/simple-car/Car.obj"));
    // Model model(FileSystem::getPath("asset/model/obj/LowPolyCar/Car.obj"));
    
    // Model model(FileSystem::getPath("asset/model/obj/Residential Buildings/Residential Buildings 001.obj"));
    
    Model model(FileSystem::getPath("asset/model/obj/Lamborghini/Lamborghini.obj"));

    Model raceTrack(FileSystem::getPath("asset/model/obj/race-track/race-track.obj"));

    // -------
    // ѭ����Ⱦ
    while (!glfwWindowShouldClose(window)) {
        // ����һ֡��ʱ�䳤���Ա���ʹ֡�����ٶȾ���
        setDeltaTime();

        // ��������
        processInput(window);

        // ��Ⱦ����
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ����uniform����֮ǰ��Ӧ��shader
        shader.use();

        // ���ù����������
        renderLight(shader);
        // ʹ��shader��Ⱦcar
        renderCar(model, shader);

        simpleShader.use();
        renderRaceTrack(raceTrack, simpleShader);

        // �����������͵���IO�¼������µİ���,����ƶ��ȣ�
        glfwSwapBuffers(window);

        // ��ѯ�¼�
        glfwPollEvents();
    }

    // �ر�glfw
    glfwTerminate();
    return 0;
}

// -----
// ��ʼ��
GLFWwindow* windowInit()
{
    // ---------
    // ��ʼ������
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // -------
    // ��������
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, u8"2017191023_������_��ĩ����ҵ", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        system("pause");
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // ��GLFW��׽�û������
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}

bool init()
{
    // ------------------
    // ��������OpenGL����ָ��
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        system("pause");
        return false;
    }

    // ����ȫ��openGL״̬
    glEnable(GL_DEPTH_TEST);

    // ���߿�ͼ
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    return true;
}

// ���ù����������
void renderLight(Shader shader)
{
    // �����λ��
    shader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
    shader.setVec3("viewPos", camera.Position);

    // ��������
    shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    // ��������
    shader.setFloat("shininess", 32.0f);
}

void renderCar(Model model, Shader shader)
{
    // ��ͼת��
    glm::mat4 viewMatrix = camera.GetViewMatrix();
    // ģ��ת��
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, car.getPosition());
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90 + car.getYaw()), glm::vec3(0.0, 1.0, 0.0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.004f, 0.004f, 0.004f));
    // ͶӰת��
    glm::mat4 projMatrix = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // Ӧ�ñ任����
    setTransMatrix(shader, viewMatrix, modelMatrix, projMatrix);

    model.Draw(shader);
}

void renderRaceTrack(Model model, Shader shader)
{
    // ��ͼת��
    glm::mat4 viewMatrix = camera.GetViewMatrix();
    // ģ��ת��
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    // ͶӰת��
    glm::mat4 projMatrix = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // Ӧ�ñ任����
    setTransMatrix(shader, viewMatrix, modelMatrix, projMatrix);

    model.Draw(shader);
}

void setTransMatrix(Shader shader, glm::mat4 viewMatrix, glm::mat4 modelMatrix, glm::mat4 projMatrix)
{
    shader.setMat4("view", viewMatrix);
    shader.setMat4("model", modelMatrix);
    shader.setMat4("projection", projMatrix);
}

// ����һ֡��ʱ�䳤��
void setDeltaTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

// -------
// ��������
void processInput(GLFWwindow* window)
{
    // esc�˳�
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // ��� WSAD ǰ������ Space�� ��Ctrl��
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    // �����ƶ�
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) 
        car.ProcessKeyboard(CAR_FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        car.ProcessKeyboard(CAR_BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        car.ProcessKeyboard(CAR_LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        car.ProcessKeyboard(CAR_RIGHT, deltaTime);

}
// -------
// ����ƶ�
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // ���귭ת�Զ�Ӧ����ϵ

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// -------
// ������
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// ----------
// �ı䴰�ڴ�С
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // ȷ������ƥ����´��ڳߴ�
    // ��Ⱥ͸߶Ƚ����Դ���ָ����retina������ʾ
    glViewport(0, 0, width, height);
}
