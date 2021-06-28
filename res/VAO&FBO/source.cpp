#define STB_IMAGE_IMPLEMENTATION
#pragma once
#include <GL/glew.h>
#include "cpp/Camera.cpp"
#include "cpp/Shader.cpp"
#include "cpp/Texture.cpp"
#include "cpp/VBO.cpp"
#include "cpp/VAO.cpp"

int main() {
    Camera Camera;
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.majorVersion = 4;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Core;

    sf::Window window(sf::VideoMode(800, 600, 32), "First Window",
        sf::Style::Titlebar | sf::Style::Close, settings);

    glewExperimental = GL_TRUE;

    if (GLEW_OK != glewInit()) {
        std::cout << "Error:: glew not init =(" << std::endl;
        return -1;
    }
    // настройки
    glEnable(GL_DEPTH_TEST);

    // шейдеры и текстуры
    Shader box("res/shaders/materials.vs", "res/shaders/materials.fs");
    Shader lamp("res/shaders/lamp.vs", "res/shaders/lamp.fs");
    // ====================

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    VertexArrayObject cube_VAO;
    VertexBufferObject VBO(vertices, sizeof(vertices));
    VertexArrayObject::link_vertex_attr(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) nullptr);
    VertexArrayObject::link_vertex_attr(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    VertexArrayObject light_cube_VAO;
    VertexArrayObject::link_vertex_attr(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) nullptr);
    VBO.bind_buffer();

    // ========================================================================
    sf::Mouse::setPosition(sf::Vector2i(800 / 2, 600 / 2), window);
    Camera.set_pos(0.0f, 0.0f, 7.0f);

    bool isGo = true;
    float phi = 0.0f;

    while (isGo) {
        phi += 0.0005f;
        // ========== обработка событий
        sf::Event windowEvent;
        while (window.pollEvent(windowEvent)) {
            switch (windowEvent.type) {
            case sf::Event::Closed:
                isGo = false;
                break;
            case sf::Event::KeyPressed:
                Camera.keyboard();
                break;
            case sf::Event::MouseMoved:
                Camera.move_mouse(window);
                break;
            case sf::Event::MouseWheelScrolled:
                Camera.scroll(windowEvent.mouseWheelScroll.delta);
                break;
            default:
                break;
            }
        }
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // ================================================
        lamp.use();

        float x = sin(phi) * 2 * lightPos.x;
        float y = sin(phi) * 2 * lightPos.y;
        float z = cos(phi) * 2 * lightPos.z;

        lamp.set_mat4("projection", Camera.get_projection());
        lamp.set_mat4("view", Camera.get_view());
        lamp.set_mat4(
            "model",
            glm::scale(glm::translate(Camera.get_model(), glm::vec3(x, y, z)), glm::vec3(0.2f))
        );

        light_cube_VAO.bind_array();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // ================================================ obsidian
        box.use();

        box.set_vec3("viewPos", Camera.get_pos());

        box.set_vec3("light.ambient", 1.0f, 0.5f, 0.31f);
        box.set_vec3("light.diffuce", 1.0f, 0.5f, 0.31f);
        box.set_vec3("light.specular", 1.0f, 1.0f, 1.0f);
        box.set_vec3("light.position", glm::vec3(x, y, z));

        box.set_vec3("material.ambient", glm::vec3(0.05375, 0.05, 0.06625)); // определяет, какой цвет отражает поверхность при фоновом освещении (обычно он совпадает с цветом поверхности)
        box.set_vec3("material.diffuse", glm::vec3(0.18275, 0.17, 0.22525)); // определяет цвет поверхности при рассеянном освещении. Рассеянный цвет (так же, как и фоновое освещение) устанавливается на желаемый цвет поверхности
        box.set_vec3("material.specular", glm::vec3(0.332741, 0.328634, 0.346435)); // задает цвет зеркального блика на поверхности (или, возможно, даже отражает специфический для поверхности цвет)
        box.set_float("material.shininess", 38.4f); // влияет на рассеивание/радиус зеркального блика.

        box.set_mat4("projection", Camera.get_projection());
        box.set_mat4("view", Camera.get_view());
        box.set_mat4("model", Camera.get_model());

        cube_VAO.bind_array();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // ================================================

        // ================================================
        window.display();
    }


    window.close();
    return 0;
}
