#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/camera.h>
#include <learnopengl/filesystem.h>
#include <learnopengl/model.h>
#include <learnopengl/shader_m.h>

#include <iostream>

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "assimp.lib")

// һЩ����
GLFWwindow* windowInit();
bool init();
void setDeltaTime();
void renderModel(Model model, Shader shader);
void setTransMatrix(Shader shader, glm::mat4 viewMatrix, glm::mat4 modelMatrix, glm::mat4 projMatrix);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// ���ڳߴ�
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));

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
    Shader shader("shader/model_loading.vs", "shader/model_loading.fs");

    // ----------------------------------
    // TODO ���ض��ģ�ͣ��ع���ȡģ�ͼ��ش���
    // ----------------------------------

    // ����ģ��
    Model model(FileSystem::getPath("asset/model/obj/simple-car/Car.obj"));
    // Model model(FileSystem::getPath("asset/model/obj/nanosuit/nanosuit.obj"));
    // Model model(FileSystem::getPath("asset/model/obj/roomdoor/Door_Component_BI3.obj"));
    // Model model(FileSystem::getPath("asset/model/obj/Residential Buildings/Residential Buildings 001.obj"));
    // Model model(FileSystem::getPath("asset/model/obj/Residential Buildings/3d-Hologramm-(Wavefront OBJ).obj"));

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

        // ʹ��shader��Ⱦmodel
        renderModel(model, shader);

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

void renderModel(Model model, Shader shader)
{
    // ����uniform����֮ǰ��Ӧ��shader
    shader.use();

    // ��ͼת��
    glm::mat4 viewMatrix = camera.GetViewMatrix();
    // ģ��ת��
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    // modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -1.75f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
    // ͶӰת��
    glm::mat4 projMatrix = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // Ӧ�ñ任����
    setTransMatrix(shader, viewMatrix, modelMatrix, projMatrix);

    model.Draw(shader);
}

void setTransMatrix(Shader shader, glm::mat4 viewMatrix, glm::mat4 modelMatrix, glm::mat4 projMatrix) {
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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
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
