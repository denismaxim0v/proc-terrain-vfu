#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "terrain.hpp"
#include "camera.hpp"
#include "skybox.hpp"

#include <iostream>

struct AppState
{
    float aspect = 1920.0f / 1080.0f;
    Camera* camera = nullptr;
    bool    dragging = false;
    double  lastX = 0.0;
    double  lastY = 0.0;
};

static AppState state;

static void framebuffer_size_callback(GLFWwindow*, int w, int h)
{
    glViewport(0, 0, w, h);
    if (h == 0) return;
    state.aspect = (float)w / (float)h;
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int)
{
    if (ImGui::GetIO().WantCaptureMouse) return;
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        state.dragging = (action == GLFW_PRESS);
        if (state.dragging)
            glfwGetCursorPos(window, &state.lastX, &state.lastY);
    }
}

static void cursor_pos_callback(GLFWwindow*, double x, double y)
{
    if (!state.dragging || !state.camera) return;
    float dx = (float)(x - state.lastX);
    float dy = (float)(y - state.lastY);
    state.lastX = x;
    state.lastY = y;
    state.camera->orbit(dx * 0.005f, -dy * 0.005f);
}

static void scroll_callback(GLFWwindow*, double, double yoffset)
{
    if (ImGui::GetIO().WantCaptureMouse || !state.camera) return;
    state.camera->zoom((float)yoffset * -0.08f);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Terrain", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glViewport(0, 0, 1280, 720);

    int   terrainSize = 1024;
    float terrainTileScale = 0.03f;
    float terrainHeight = 4.5f;
    int   terrainSeed = 1337;
    float terrainFrequency = 220.0f;
    float terrainGain = 0.5f;
    float terrainLacunarity = 2.0f;
    bool  wireframe = false;

    Terrain terrain(terrainSize, terrainTileScale, terrainHeight,
        terrainSeed, terrainFrequency, terrainGain, terrainLacunarity);

    Skybox skybox("assets");

    Camera camera(terrain.getWorldSize(), terrain.getHeightScale(), state.aspect);
    state.camera = &camera;
    camera.setAspect(state.aspect);

    // ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

        terrain.draw(camera);
        skybox.draw(camera);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Terrain");

        bool rebuild = false;

        ImGui::SliderInt("Size", &terrainSize, 256, 4096);
        rebuild |= ImGui::IsItemDeactivatedAfterEdit();

        ImGui::SliderFloat("Tile scale", &terrainTileScale, 0.001f, 1.0f);
        rebuild |= ImGui::IsItemDeactivatedAfterEdit();

        ImGui::SliderFloat("Height scale", &terrainHeight, 0.1f, 30.0f);
        rebuild |= ImGui::IsItemDeactivatedAfterEdit();

        ImGui::SliderInt("Seed", &terrainSeed, 0, 99999999);
        rebuild |= ImGui::IsItemDeactivatedAfterEdit();

        ImGui::SliderFloat("Frequency", &terrainFrequency, 1.0f, 500.0f);
        rebuild |= ImGui::IsItemDeactivatedAfterEdit();

        ImGui::SliderFloat("Gain", &terrainGain, 0.1f, 0.9f);
        rebuild |= ImGui::IsItemDeactivatedAfterEdit();

        ImGui::SliderFloat("Lacunarity", &terrainLacunarity, 1.0f, 4.0f);
        rebuild |= ImGui::IsItemDeactivatedAfterEdit();

        ImGui::Checkbox("Wireframe", &wireframe);

        if (rebuild)
        {
            terrain.update(terrainSize, terrainTileScale, terrainHeight,
                terrainSeed, terrainFrequency, terrainGain,
                terrainLacunarity);

            camera.update(terrain.getWorldSize(), terrain.getHeightScale());
            camera.setAspect(state.aspect);
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}