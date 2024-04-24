#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <imgui.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

bool drawPolygon = false;
bool addNewPoints = true;
std::vector<std::pair<float, float>> points;
float initialPointX = 0.0f, initialPointY = 0.0f;
float currentArea = 0.0f;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && addNewPoints) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        float x = static_cast<float>(xpos) / width * 2 - 1;
        float y = static_cast<float>(height - ypos) / height * 2 - 1;

        points.push_back(std::make_pair(x, y));

        if (points.size() == 1) {
            initialPointX = x;
            initialPointY = y;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && points.size() >= 3) {
        drawPolygon = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !addNewPoints) {
        points.clear();
        addNewPoints = true;
    }
}

float calculateArea(const std::vector<std::pair<float, float>>& polygon) {
    float area = 0.0f;
    int n = polygon.size();

    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        area += polygon[i].first * polygon[j].second - polygon[j].first * polygon[i].second;
    }

    return std::abs(area) / 2.0f;
}

float calculateAreaInPixels(const std::vector<std::pair<float, float>>& polygon, int width, int height) {
    float area = calculateArea(polygon);
    float worldArea = 4.0f;
    float pixelArea = (area / worldArea) * (width * height);
    return pixelArea;
}

void renderInterface() {
    ImGui::Begin("Polygon Information");
    currentArea = calculateArea(points);
    ImGui::InputFloat("Area of the polygon", &currentArea, ImGuiInputTextFlags_ReadOnly);
    ImGui::End();
}

int main() {
    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Polygon Area Calculator", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    ImGui::StyleColorsDark();

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (drawPolygon && points.size() >= 3) {
            float area = calculateArea(points);
            ImGui::Text("Area: %.2f", area);
        }

        if (points.size() >= 2) {
            glLineWidth(2.0f);
            glBegin(GL_LINES);
            glColor3f(0.5f, 0.0f, 0.5f);
            for (size_t i = 0; i < points.size() - 1; ++i) {
                glVertex2f(points[i].first, points[i].second);
                glVertex2f(points[i + 1].first, points[i + 1].second);
            }
            glVertex2f(points.back().first, points.back().second);
            glVertex2f(points.front().first, points.front().second);
            glEnd();
        }

        if (!points.empty()) {
            glPointSize(8.0f);
            glBegin(GL_POINTS);
            glColor3f(1.0f, 0.0f, 0.0f); // Красный цвет.
            for (const auto& point : points) {
                glVertex2f(point.first, point.second);
            }
            glEnd();
        }

        renderInterface();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
