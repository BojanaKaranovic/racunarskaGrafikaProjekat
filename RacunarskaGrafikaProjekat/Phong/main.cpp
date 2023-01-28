//Bojana Karanovic
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <thread>
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "texture.hpp"

 /**
  * @brief Returns x value inside range
  *
  * @param x - Value to constrict to range
  * @param min - minimum allowed value for x
  * @param max - maximum allowed value for x
  * 
  * @returns x value between min and max
  */
float
Clamp(float x, float min, float max) {
    return x < min ? min : x > max ? max : x;
}

int WindowWidth = 1200;
int WindowHeight = 800;
const float TargetFPS = 60.0f;
const std::string WindowTitle = "Karibi";


struct Input {
    bool MoveLeft;
    bool MoveRight;
    bool MoveUp;
    bool MoveDown;
    bool LookLeft;
    bool LookRight;
    bool LookUp;
    bool LookDown;
};

struct EngineState {
    Input* mInput;
    Camera* mCamera;
    bool mDrawDebugLines;
    float mDT;
};
bool pressed = true;
/**
 * @brief Error callback function for GLFW. See GLFW docs for details
 *
 * @param error Error code
 * @param description Error message
 */
static void
ErrorCallback(int error, const char* description) {
    std::cerr << "GLFW Error: " << description << std::endl;
}

/**
 * @brief Keyboard callback function for GLFW. See GLFW docs for details
 *
 * @param window GLFW window context object
 * @param key Triggered key GLFW code
 * @param scancode Triggered key scan code
 * @param action Triggered key action: pressed, released or repeated
 * @param mode Modifiers
 */
static void
KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    EngineState* State = (EngineState*)glfwGetWindowUserPointer(window);
    Input* UserInput = State->mInput;
    bool IsDown = action == GLFW_PRESS || action == GLFW_REPEAT;
    switch (key) {
    case GLFW_KEY_A: UserInput->MoveLeft = IsDown; break;
    case GLFW_KEY_D: UserInput->MoveRight = IsDown; break;
    case GLFW_KEY_W: UserInput->MoveUp = IsDown; break;
    case GLFW_KEY_S: UserInput->MoveDown = IsDown; break;

    case GLFW_KEY_RIGHT: UserInput->LookLeft = IsDown; break;
    case GLFW_KEY_LEFT: UserInput->LookRight = IsDown; break;
    case GLFW_KEY_UP: UserInput->LookUp = IsDown; break;
    case GLFW_KEY_DOWN: UserInput->LookDown = IsDown; break;


    case GLFW_KEY_L: {
        if (IsDown) {
            State->mDrawDebugLines ^= true; break;
        }
    } break;

    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;

    case GLFW_KEY_SPACE: if (IsDown) pressed = !pressed; break;
    }
}

static void
FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    WindowWidth = width;
    WindowHeight = height;
    glViewport(0, 0, width, height);
}

/**
 * @brief Updates engine state based on input
 * 
 * @param state EngineState
 */
static void
HandleInput(EngineState* state) {
    Input* UserInput = state->mInput;
    Camera* FPSCamera = state->mCamera;
    if (UserInput->MoveLeft) FPSCamera->Move(-1.0f, 0.0f, state->mDT);
    if (UserInput->MoveRight) FPSCamera->Move(1.0f, 0.0f, state->mDT);
    if (UserInput->MoveDown) FPSCamera->Move(0.0f, -1.0f, state->mDT);
    if (UserInput->MoveUp) FPSCamera->Move(0.0f, 1.0f, state->mDT);

    if (UserInput->LookLeft) FPSCamera->Rotate(1.0f, 0.0f, state->mDT);
    if (UserInput->LookRight) FPSCamera->Rotate(-1.0f, 0.0f, state->mDT);
    if (UserInput->LookDown) FPSCamera->Rotate(0.0f, -1.0f, state->mDT);
    if (UserInput->LookUp) FPSCamera->Rotate(0.0f, 1.0f, state->mDT);
}


int main() {
    GLFWwindow* Window = 0;
    if (!glfwInit()) {
        std::cerr << "Failed to init glfw" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle.c_str(), 0, 0);
    if (!Window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(Window);

    GLenum GlewError = glewInit();
    if (GlewError != GLEW_OK) {
        std::cerr << "Failed to init glew: " << glewGetErrorString(GlewError) << std::endl;
        glfwTerminate();
        return -1;
    }

    EngineState State = { 0 };
    Camera FPSCamera;
    Input UserInput = { 0 };
    State.mCamera = &FPSCamera;
    State.mInput = &UserInput;
    glfwSetWindowUserPointer(Window, &State);

    glfwSetErrorCallback(ErrorCallback);
    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
    glfwSetKeyCallback(Window, KeyCallback);

    glViewport(0.0f, 0.0f, WindowWidth, WindowHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    unsigned CubeDiffuseTexture = Texture::LoadImageToTexture("res/container_diffuse.png");
    unsigned CubeSpecularTexture = Texture::LoadImageToTexture("res/container_specular.png");
    unsigned FloorDiffuseTexture = Texture::LoadImageToTexture("res/floor_diffuse.jpg");
    unsigned FloorSpecularTexture = Texture::LoadImageToTexture("res/floor_specular.jpg");
    unsigned SandDiffuseTexture = Texture::LoadImageToTexture("res/sand.png");
    unsigned OceanDiffuseTexture = Texture::LoadImageToTexture("res/oceanDiffuse.png");
    unsigned OceanSpecularTexture = Texture::LoadImageToTexture("res/oceanSpec.png");
    unsigned PalmTreeDiffuseTexture = Texture::LoadImageToTexture("res/palm_tree.png");
    unsigned PalmLeafDiffuseTexture = Texture::LoadImageToTexture("res/palm_leaf.png");

    std::vector<float> CubeVertices = {
        // X     Y     Z     NX    NY    NZ    U     V    
        //FRONT SIDE
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 
         0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 
        -0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 
         0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 
         0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 
        -0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 
        // LEFT SIDE
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        // RIGHT SIDE
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 
        // BOTTOM SIDE
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        // TOP SIDE
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 
        // BACK SIDE
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    };



    unsigned CubeVAO;
    glGenVertexArrays(1, &CubeVAO);
    glBindVertexArray(CubeVAO);
    unsigned CubeVBO;
    glGenBuffers(1, &CubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, CubeVertices.size() * sizeof(float), CubeVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Model Alduin("res/alduin/alduin-dragon.obj");
    if (!Alduin.Load()) {
        std::cerr << "Failed to load alduin\n";
        glfwTerminate();
        return -1;
    }

    Model Fox("res/low-poly-fox/low-poly-fox.obj");
    if (!Fox.Load()) {
        std::cerr << "Failed to load fox\n";
        glfwTerminate();
        return -1;
    }

    Model Monkey("res/monkey/12958_Spider_Monkey_v1_l2.obj");
    if (!Monkey.Load()) {
        std::cerr << "Failed to load fox\n";
        glfwTerminate();
        return -1;
    }

    //Used to only define color
    Shader ColorShader("shaders/color.vert", "shaders/color.frag");

    //Gouraud, Phong, Phong with materials and Phong with materials and textures
    Shader GouraudShader("shaders/gouraud.vert", "shaders/basic.frag");

    Shader PhongShader("shaders/basic.vert", "shaders/phong.frag");

    //Phong shader with material support
    Shader PhongShaderMaterial("shaders/basic.vert", "shaders/phong_material.frag");
    

    //Phong shader with material and texture support
    Shader PhongShaderMaterialTexture("shaders/basic.vert", "shaders/phong_material_texture.frag");
    glUseProgram(PhongShaderMaterialTexture.GetId());
    PhongShaderMaterialTexture.SetUniform3f("uDirLight.Direction", glm::vec3(-8.0f, 10.0f, -3.0f));
    //Yellow ambient and diffuse, white specular 
    PhongShaderMaterialTexture.SetUniform3f("uDirLight.Ka", glm::vec3(0.6f, 0.6f, 0.4f));
    PhongShaderMaterialTexture.SetUniform3f("uDirLight.Kd", glm::vec3(0.6f, 0.6f, 0.4f));
    PhongShaderMaterialTexture.SetUniform3f("uDirLight.Ks", glm::vec3(1.0f));

    PhongShaderMaterialTexture.SetUniform3f("uPointLight.Ka", glm::vec3(0.7f, 0.5f, 0.0f));
    PhongShaderMaterialTexture.SetUniform3f("uPointLight.Kd", glm::vec3(0.7f, 0.5f, 0.0f));
    PhongShaderMaterialTexture.SetUniform3f("uPointLight.Ks", glm::vec3(1.0f));
    PhongShaderMaterialTexture.SetUniform1f("uPointLight.Kc", 0.7f );
    PhongShaderMaterialTexture.SetUniform1f("uPointLight.Kl", 0.592f );
    PhongShaderMaterialTexture.SetUniform1f("uPointLight.Kq", 0.532f);

    //Fire position
    PhongShaderMaterialTexture.SetUniform3f("uPointLightPosition1", glm::vec3(-1.7f, 0.21f, -2.0f));
    PhongShaderMaterialTexture.SetUniform3f("uPointLightPosition2", glm::vec3(-10.0f, -0.4f, 0.0f));
    PhongShaderMaterialTexture.SetUniform3f("uPointLightPosition3", glm::vec3(10.0f, -0.4f, -3.0f));

    PhongShaderMaterialTexture.SetUniform3f("uSpotLightPosition1", glm::vec3(-15.0f, 2.5f, -15.0f));
    PhongShaderMaterialTexture.SetUniform3f("uSpotLightPosition2", glm::vec3(-15.0f, 2.5f, -15.0f));
    PhongShaderMaterialTexture.SetUniform3f("uSpotLightDirection1", glm::vec3(2.0f, 0.0f, 10.0f));
    PhongShaderMaterialTexture.SetUniform3f("uSpotLightDirection2", glm::vec3(-2.0f, 0.0f, -10.0f));
    PhongShaderMaterialTexture.SetUniform3f("uSpotlight.Ka", glm::vec3(1.0f, 1.0f, 0.0f));
    PhongShaderMaterialTexture.SetUniform3f("uSpotlight.Kd", glm::vec3(1.0f, 1.0f, 0.0f));
    PhongShaderMaterialTexture.SetUniform3f("uSpotlight.Ks", glm::vec3(1.0f));
    PhongShaderMaterialTexture.SetUniform1f("uSpotlight.Kc", 0.5f);
    PhongShaderMaterialTexture.SetUniform1f("uSpotlight.Kl", 0.092f);
    PhongShaderMaterialTexture.SetUniform1f("uSpotlight.Kq", 0.032f);
    PhongShaderMaterialTexture.SetUniform1f("uSpotlight.InnerCutOff", glm::cos(glm::radians(50.0f)));
    PhongShaderMaterialTexture.SetUniform1f("uSpotlight.OuterCutOff", glm::cos(glm::radians(50.5f)));

    PhongShaderMaterialTexture.SetUniform1i("uMaterial.Kd", 0.9);
    PhongShaderMaterialTexture.SetUniform1i("uMaterial.Ks", 1);
    PhongShaderMaterialTexture.SetUniform1f("uMaterial.Shininess", 150.0f);
    glUseProgram(0);

    

    glm::mat4 Projection = glm::perspective(45.0f, WindowWidth / (float)WindowHeight, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(FPSCamera.GetPosition(), FPSCamera.GetTarget(), FPSCamera.GetUp());
    glm::mat4 ModelMatrix(1.0f);
    
    //Current angle around Y axis, with regards to XZ plane at which the point light is situated at
    float Angle = 0.0f;
    //Distance of point light from center of rotation
    float Distance = 2.0f;
    float TargetFrameTime = 1.0f / TargetFPS;
    float StartTime = glfwGetTime();
    float EndTime = glfwGetTime();
    glClearColor(0.0, 0.86, 0.922, 1.0);
    float angle = 0;

    //Currently used shader
    Shader* CurrentShader = &PhongShaderMaterialTexture;
    
    while (!glfwWindowShouldClose(Window)) {
        glfwPollEvents();
        HandleInput(&State);
        CurrentShader = &PhongShaderMaterialTexture;

        
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // In case of window resize, update projection. Bit bad for performance to do it every iteration.
        // If laggy, remove this line
        Projection = glm::perspective(45.0f, WindowWidth / (float)WindowHeight, 0.1f, 100.0f);
        View = glm::lookAt(FPSCamera.GetPosition(), FPSCamera.GetTarget(), FPSCamera.GetUp());
        StartTime = glfwGetTime();
        glUseProgram(CurrentShader->GetId());
        CurrentShader->SetProjection(Projection);
        CurrentShader->SetView(View);
        CurrentShader->SetUniform3f("uViewPos", FPSCamera.GetPosition());

        

        //Change intensity of fire
        CurrentShader->SetUniform1f("uPointLight.Kc", 0.3 + abs(sin(glfwGetTime())));
        CurrentShader->SetUniform1f("uPointLight.Kl", 0.2 + abs(sin(glfwGetTime())));
        CurrentShader->SetUniform1f("uPointLight.Kq", 0.5 + abs(sin(glfwGetTime())));

        if (angle > 360) {
            angle = 0;
        }
        if (pressed) {
            CurrentShader->SetUniform3f("uSpotlight.Ka", glm::vec3(0.0f, 0.0f, 0.0f));
            CurrentShader->SetUniform3f("uSpotlight.Kd", glm::vec3(0.0f, 0.0f, 0.0f));
            CurrentShader->SetUniform3f("uSpotlight.Ks", glm::vec3(0.0f, 0.0f, 0.0f));
            
        }
        else {
            CurrentShader->SetUniform3f("uSpotlight.Ka", glm::vec3(1.0f, 1.0f, 0.0f));
            CurrentShader->SetUniform3f("uSpotlight.Kd", glm::vec3(1.0f, 1.0f, 0.0f));
            CurrentShader->SetUniform3f("uSpotlight.Ks", glm::vec3(1.0f));
            CurrentShader->SetUniform3f("uSpotLightDirection1", glm::vec3(sin(glfwGetTime()), 0.00, cos(glfwGetTime())));
            CurrentShader->SetUniform3f("uSpotLightDirection2", glm::vec3(sin(glfwGetTime() + 3.14), 0.00, cos(glfwGetTime() + 3.14)));
        }
        //CurrentShader->SetUniform1f("uPointLight.Kq", 0.5 + abs(sin(glfwGetTime())));
        Angle += State.mDT;
        angle += 1.3;

        //Ocean - goes up and down, simulating the rising of the ocean
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0, 0.2 * sin(glfwGetTime()) - 6.6, -10.0));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(100.0f, 10.0f, 40.0));
        ColorShader.SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        CurrentShader->SetModel(ModelMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, OceanDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, OceanSpecularTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);
        glBindTexture(GL_TEXTURE_2D, 0);

        //Islands - 3
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-10.0f, -1.5f, 0.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f, 2.0f));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(2.0f), glm::vec3(0.0, 1.0, 0.0));
        CurrentShader->SetModel(ModelMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SandDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-0.3f, -1.4f, -2.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(6.0f, 3.0f, 5.0f));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(2.0f), glm::vec3(0.0, 1.0, 0.0));
        CurrentShader->SetModel(ModelMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SandDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(10.0f, -1.5f, -3.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(4.0f, 2.0f, 2.0f));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(2.0f), glm::vec3(0.0, 1.0, 0.0));
        CurrentShader->SetModel(ModelMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SandDiffuseTexture);
        
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);


        //Palm tree - made of one tree trunk and treetop
        //Trunk
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.3f, 1.0f, -2.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f, 3.0f, 0.4f));
        CurrentShader->SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PalmTreeDiffuseTexture);
        
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //Treetop - made of four leafs
        //Front leaf 
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.30f, 2.35f, -1.6f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));
        CurrentShader->SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PalmLeafDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.30f, 2.2f, -1.1f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));
        CurrentShader->SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PalmLeafDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.30f, 2.05f, -0.6f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));
        CurrentShader->SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PalmLeafDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //Right leaf
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.80f, 2.35f, -2.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));
        CurrentShader->SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PalmLeafDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1.3f, 2.2f, -2.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));
        CurrentShader->SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PalmLeafDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1.8f, 2.05f, -2.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));
        CurrentShader->SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PalmLeafDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //Left leaf
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-0.2f, 2.35f, -2.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));
        CurrentShader->SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PalmLeafDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-0.7f, 2.2f, -2.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));
        CurrentShader->SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PalmLeafDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-1.2, 2.05f, -2.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));
        CurrentShader->SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PalmLeafDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //Back leaf
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.30f, 2.35f, -2.4f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));
        CurrentShader->SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PalmLeafDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.30f, 2.2f, -2.9f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));
        CurrentShader->SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PalmLeafDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.30f, 2.05f, -3.4f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));
        CurrentShader->SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PalmLeafDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //Monkey model
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.009, 0.009, 0.009));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(90.0f), glm::vec3(-1.0, 0.0, 0.0));
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 85.0f, 12.8f));
        CurrentShader->SetModel(ModelMatrix);
        Monkey.Render();

        glUseProgram(ColorShader.GetId());
        ColorShader.SetProjection(Projection);
        ColorShader.SetView(View);
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 1.0f, -2.0f));
        ColorShader.SetModel(ModelMatrix);

        //Fires 
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-10.0f, -0.4f, 0.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.15f));
        ColorShader.SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        ColorShader.SetUniform3f("uColor", glm::vec3(0.7, 0.3, 0.0));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-1.7f, 0.2f, -2.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.15f));
        ColorShader.SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        ColorShader.SetUniform3f("uColor", glm::vec3(0.7, 0.3, 0.0));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(10.0f, -0.4f, -3.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.15f));
        ColorShader.SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        ColorShader.SetUniform3f("uColor", glm::vec3(0.7, 0.3, 0.0));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Sun
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-8.0f, 10.0f, -3.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));

        ColorShader.SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        ColorShader.SetUniform3f("uColor", glm::vec3(0.5f, 0.5f, 0.0f));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(30.0f), glm::vec3(2.0, 1.0, 1.0));
        ColorShader.SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        ColorShader.SetUniform3f("uColor", glm::vec3(0.8, 0.4 + abs(sin(glfwGetTime())), 0.1));
        glDrawArrays(GL_TRIANGLES, 0, 36);

       ModelMatrix = glm::rotate(ModelMatrix, glm::radians(60.0f), glm::vec3(2.0, 1.0, 1.0));
        ColorShader.SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        ColorShader.SetUniform3f("uColor", glm::vec3(0.5, 0.2 + abs(sin(glfwGetTime())), 0));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(95.0f), glm::vec3(2.0, 1.0, 1.0));
        ColorShader.SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        ColorShader.SetUniform3f("uColor", glm::vec3(0.8, 0.6 + abs(sin(glfwGetTime())), 0));
        glDrawArrays(GL_TRIANGLES, 0, 36);


        //Clouds
        //Dessapear or appear on space click
        if (pressed) {
            ModelMatrix = glm::mat4(1.0f);
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(11.0f, 7.0f, -5.0f));
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.2f));
            ModelMatrix = glm::rotate(ModelMatrix, glm::radians(30.0f), glm::vec3(2.0, 1.0, 1.0));
            ColorShader.SetModel(ModelMatrix);
            glBindVertexArray(CubeVAO);
            ColorShader.SetUniform3f("uColor", glm::vec3(1.0, 1.0, 1.0));
            glDrawArrays(GL_TRIANGLES, 0, 36);

            ModelMatrix = glm::mat4(1.0f);
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1.2f, 9.0f, -8.0f));
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.9f));
            ModelMatrix = glm::rotate(ModelMatrix, glm::radians(45.0f), glm::vec3(1.0, 1.0, 0.0));
            ColorShader.SetModel(ModelMatrix);
            glBindVertexArray(CubeVAO);
            ColorShader.SetUniform3f("uColor", glm::vec3(1.0, 1.0, 1.0));
            glDrawArrays(GL_TRIANGLES, 0, 36);

            ModelMatrix = glm::mat4(1.0f);
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-5.3f, 7.0f, -6.0f));
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.5f));
            ModelMatrix = glm::rotate(ModelMatrix, glm::radians(32.0f), glm::vec3(1.0, 1.0, 0.0));
            ColorShader.SetModel(ModelMatrix);
            glBindVertexArray(CubeVAO);
            ColorShader.SetUniform3f("uColor", glm::vec3(1.0, 1.0, 1.0));
            glDrawArrays(GL_TRIANGLES, 0, 36);

            ModelMatrix = glm::mat4(1.0f);
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-15.3f, 6.0f, -8.0f));
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f));
            ModelMatrix = glm::rotate(ModelMatrix, glm::radians(100.0f), glm::vec3(1.0, 2.0, 0.0));
            ColorShader.SetModel(ModelMatrix);
            glBindVertexArray(CubeVAO);
            ColorShader.SetUniform3f("uColor", glm::vec3(1.0, 1.0, 1.0));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        //Lighthouse - lights up when there are no clouds
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-15.0f, -1.5f, -15.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f));
        ColorShader.SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        ColorShader.SetUniform3f("uColor", glm::vec3(0.3, 0.0, 0.0));
        glDrawArrays(GL_TRIANGLES, 0, 36);
       
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-15.0f, -0.5f, -15.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f));
        ColorShader.SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        ColorShader.SetUniform3f("uColor", glm::vec3(0.7, 0.7, 0.7));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-15.0f, 0.5f, -15.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f));
        ColorShader.SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        ColorShader.SetUniform3f("uColor", glm::vec3(0.3, 0.0, 0.0));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-15.0f, 1.5f, -15.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        ColorShader.SetModel(ModelMatrix);
        glBindVertexArray(CubeVAO);
        ColorShader.SetUniform3f("uColor", glm::vec3(0.7, 0.7, 0.7));
        glRotatef(Angle, 0, 1, 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        



        glBindVertexArray(0);
        glUseProgram(0);
        glfwSwapBuffers(Window);

        //Time management
        EndTime = glfwGetTime();
        float WorkTime = EndTime - StartTime;
        if (WorkTime < TargetFrameTime) {
            int DeltaMS = (int)((TargetFrameTime - WorkTime) * 1000.0f);
            std::this_thread::sleep_for(std::chrono::milliseconds(DeltaMS));
            EndTime = glfwGetTime();
        }
        State.mDT = EndTime - StartTime;
    }

    glfwTerminate();
    return 0;
}
