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

#include <iostream>

bool wireframe = false;

struct AppState
{
    float aspect = 1920.0f / 1080.0f;
};

static AppState state;

static void framebuffer_size_callback(GLFWwindow*, int w, int h)
{
    glViewport(0, 0, w, h);

    if (h == 0) return;

    state.aspect = (float)w / (float)h;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Terrain", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1920, 1080);

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

    Camera camera(terrain.getWorldSize(), terrain.getHeightScale(), state.aspect);
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

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Terrain");

        bool rebuild = false;

        ImGui::SliderInt("Size", &terrainSize, 256, 2048);
        rebuild |= ImGui::IsItemDeactivatedAfterEdit();

        ImGui::SliderFloat("Tile scale", &terrainTileScale, 0.001f, 1.0f);
        rebuild |= ImGui::IsItemDeactivatedAfterEdit();

        ImGui::SliderFloat("Height scale", &terrainHeight, 0.1f, 20.0f);
        rebuild |= ImGui::IsItemDeactivatedAfterEdit();

        ImGui::SliderInt("Seed", &terrainSeed, 0, 9999999);
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
            terrain = Terrain(terrainSize, terrainTileScale, terrainHeight,
                terrainSeed, terrainFrequency, terrainGain,
                terrainLacunarity);

            camera = Camera(terrain.getWorldSize(), terrain.getHeightScale(), state.aspect);
            camera.setAspect(state.aspect);
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glfwTerminate();
}