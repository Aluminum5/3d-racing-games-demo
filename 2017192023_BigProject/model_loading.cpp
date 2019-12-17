#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/camera.h>
#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>

#include <iostream>

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "assimp.lib")

// һЩ����
void init();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// ���ڳߴ�
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const vec4 BG_COLOR(0.1f, 0.1f, 0.1f, 1.0f);

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
    // һЩ��ʼ��
    init();

    // �����ͱ�����ɫ��
    Shader ourShader("shader/model_loading.vs", "shader/model_loading.fs");

    // ����ģ��
    // Model ourModel(FileSystem::getPath("asset/model/obj/nanosuit/nanosuit.obj"));
    // Model ourModel(FileSystem::getPath("asset/model/obj/roomdoor/Door_Component_BI3.obj"));
    Model ourModel(FileSystem::getPath("asset/model/obj/Residential Buildings/Residential Buildings 001.obj"));
    // Model ourModel(FileSystem::getPath("asset/model/obj/Residential Buildings/3d-Hologramm-(Wavefront OBJ).obj"));

    // -------
    // ѭ����Ⱦ
    while (!glfwWindowShouldClose(window)) {
        // ÿ֡��ʱ���߼�
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // ��������
        processInput(window);

        // ��Ⱦ
        glClearColor(BG_COLOR);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ����uniform����֮ǰ��Ӧ��shader
        ourShader.use();

        // ��ͼ/ͶӰת��
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // ��Ⱦ���ص�ģ��
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // ����������λ��scene������
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)); // ��С
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        // ----------------------------------------
        // �����������͵���IO�¼������µİ���,����ƶ��ȣ�
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // �ر�glfw
    glfwTerminate();
    return 0;
}

// -----
// ��ʼ��
void init() {
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
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // ��GLFW��׽�û������
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // --------------------
    // ��������OpenGL����ָ��
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        system("pause");
        return -1;
    }

    // ����ȫ��openGL״̬
    glEnable(GL_DEPTH_TEST);

    // ���߿�ͼ
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
