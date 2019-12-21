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

// ------------------------------------------
// ��������
// ------------------------------------------

GLFWwindow* windowInit();
bool init();
void skyboxInit();

void setDeltaTime();

void renderLight(Shader shader);
void renderCar(Model model, Shader shader);
void renderRaceTrack(Model model, Shader shader);
void renderSkyBox(Shader shader);

void setTransMatrix(Shader shader, glm::mat4 viewMatrix, glm::mat4 modelMatrix, glm::mat4 projMatrix);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

unsigned int loadCubemap(vector<std::string> faces);

// ------------------------------------------
// ȫ�ֱ���
// ------------------------------------------

// ���ڳߴ�
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// ��Ⱦ��Ӱʱ�Ĵ��ڷֱ��ʣ���Ӱ����Ӱ�ľ�ݱ������
const unsigned int SHADOW_WIDTH = 10240;
const unsigned int SHADOW_HEIGHT = 10240;

// ������һЩ����
Car car(glm::vec3(0.0f, 0.0f, 0.0f));

// ���
Camera camera(glm::vec3(0.0f, 50.0f, 0.0f));

// ��պ�
unsigned int skyboxVAO, skyboxVBO;
unsigned int cubemapTexture;

// �����������
glm::vec3 lightPos(-1.0f, 1.0f, -1.0f);
glm::vec3 lightDirection = glm::normalize(lightPos);
glm::mat4 lightSpaceMatrix;

// ���Map��ID
unsigned int depthMap;

// �������������Ļ����
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing ����ƽ�ⲻͬ������Ⱦˮƽ���������ٶȱ仯
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ��պж�������
const float skyboxVertices[] = {
    // positions
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f
};

// ��պе�������
const vector<std::string> faces{
    FileSystem::getPath("asset/texture/skybox/right.tga"),
    FileSystem::getPath("asset/texture/skybox/left.tga"),
    FileSystem::getPath("asset/texture/skybox/top.tga"),
    FileSystem::getPath("asset/texture/skybox/bottom.tga"),
    FileSystem::getPath("asset/texture/skybox/front.tga"),
    FileSystem::getPath("asset/texture/skybox/back.tga")
    // FileSystem::getPath("asset/texture/skybox2/right.jpg"),
    // FileSystem::getPath("asset/texture/skybox2/left.jpg"),
    // FileSystem::getPath("asset/texture/skybox2/top.jpg"),
    // FileSystem::getPath("asset/texture/skybox2/bottom.jpg"),
    // FileSystem::getPath("asset/texture/skybox2/front.jpg"),
    // FileSystem::getPath("asset/texture/skybox2/back.jpg")
};

// ------------------------------------------
// main����
// ------------------------------------------

int main()
{
    // ------------------------------
    // ��ʼ��
    // ------------------------------

    GLFWwindow* window = windowInit();
    bool isInit = init();
    // һЩ��ʼ��
    if (window == NULL || !isInit) {
        return -1;
    }

    // ------------------------------
    // �����ͱ�����ɫ��
    // ------------------------------

    Shader shader("shader/shadow_mapping.vs", "shader/shadow_mapping.fs");
    Shader simpleDepthShader("shader/shadow_mapping_depth.vs", "shader/shadow_mapping_depth.fs");
    Shader skyboxShader("shader/skybox.vs", "shader/skybox.fs");

    // ------------------------------
    // ģ�ͼ���
    // ------------------------------

    // Model model(FileSystem::getPath("asset/model/obj/nanosuit/nanosuit.obj"));
    // Model model(FileSystem::getPath("asset/model/obj/roomdoor/Door_Component_BI3.obj"));
    // Model model(FileSystem::getPath("asset/model/obj/simple-car/Car.obj"));
    // Model model(FileSystem::getPath("asset/model/obj/LowPolyCar/Car.obj"));

    // ����ģ��
    Model model(FileSystem::getPath("asset/model/obj/Lamborghini/Lamborghini.obj"));

    // ����ģ��
    Model raceTrack(FileSystem::getPath("asset/model/obj/race-track/race-track.obj"));

    // ------------------------------
    // ���Map��FBO����
    // ------------------------------

    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // �����������
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // �����ɵ����������Ϊ֡�������Ȼ���
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ---------------------------------
    // ��պе�����
    // ---------------------------------

    skyboxInit();

    // ---------------------------------
    // shader ����
    // ---------------------------------

    shader.use();
    shader.setInt("diffuseTexture", 0);
    shader.setInt("shadowMap", 15);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // ---------------------------------
    // ѭ����Ⱦ
    // ---------------------------------

    while (!glfwWindowShouldClose(window)) {
        // ����һ֡��ʱ�䳤���Ա���ʹ֡�����ٶȾ���
        setDeltaTime();

        // ����ʱ��ı��Դλ��
        // lightPos.x = sin(glfwGetTime()) * 1.0f;
        // lightPos.z = cos(glfwGetTime()) * 2.0f;
        // // lightPos.y = 5.0 + cos(glfwGetTime()) * 1.0f;

        // lightDirection = glm::normalize(lightPos);

        // ��������
        processInput(window);

        // ��Ⱦ����
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ---------------------------------
        // ��Ⱦ��ó����������Ϣ
        // ---------------------------------

        float near_plane = -200.0f, far_plane = 200.0f;
        glm::mat4 lightProjection = glm::ortho(
            -200.0f, 200.0f,
            -200.0f, 200.0f,
            near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        // �ӹ�Դ�Ƕ���Ⱦ��������
        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        // �ı��ӿڴ�С�Ա��ڽ�����ȵ���Ⱦ
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        // ʹ�����shader��Ⱦ���ɳ���
        glClear(GL_DEPTH_BUFFER_BIT);
        renderCar(model, simpleDepthShader);
        renderRaceTrack(raceTrack, simpleDepthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ��ԭ�ӿ�
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ---------------------------------
        // ģ����Ⱦ
        // ---------------------------------

        shader.use();

        // ���ù����������
        renderLight(shader);

        // ʹ��shader��Ⱦcar
        renderCar(model, shader);

        // ��Ⱦ����
        renderRaceTrack(raceTrack, shader);

        // --------------
        // �������Ⱦ��պ�

        // �ı���Ȳ��ԣ��Ż���������⵼�µ��ػ�Ӷ�������������
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        renderSkyBox(skyboxShader);
        // ��ԭ��Ȳ���
        glDepthFunc(GL_LESS);

        // �����������͵���IO�¼������µİ���,����ƶ��ȣ�
        glfwSwapBuffers(window);

        // ��ѯ�¼�
        glfwPollEvents();
    }

    // �ر�glfw
    glfwTerminate();
    return 0;
}

// ------------------------------------------
// ��������
// ------------------------------------------

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

void skyboxInit()
{
    // skybox VAO
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // �������
    cubemapTexture = loadCubemap(faces);
}

// ���ù����������
void renderLight(Shader shader)
{

    shader.setVec3("viewPos", camera.Position);
    // shader.setVec3("lightPos", lightPos);
    shader.setVec3("lightDirection", lightDirection);
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    // // �����λ��
    // shader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
    // shader.setVec3("viewPos", camera.Position);

    // // ��������
    // shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    // shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    // shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    // // ��������
    // shader.setFloat("shininess", 32.0f);
}

// ��Ⱦ����
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
    glm::mat4 projMatrix = camera.GetProjMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);

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
    glm::mat4 projMatrix = camera.GetProjMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);

    // Ӧ�ñ任����
    setTransMatrix(shader, viewMatrix, modelMatrix, projMatrix);

    model.Draw(shader);
}

void renderSkyBox(Shader shader)
{
    // viewMatrix ͨ�����죬�Ƴ�������ƶ�
    glm::mat4 viewMatrix = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    // ͶӰ
    glm::mat4 projMatrix = camera.GetProjMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);

    shader.setMat4("view", viewMatrix);
    shader.setMat4("projection", projMatrix);

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
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

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
