#define STB_IMAGE_IMPLEMENTATION
#pragma once
#include <GL/glew.h>
#include "cpp/Camera.cpp"
#include "cpp/Shader.cpp"
#include "cpp/Texture.cpp"
#include "cpp/VBO.cpp"
#include "cpp/VAO.cpp"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "cpp/Model.h"


unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
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

struct point {
    point() = default;
    point(float x, float y, float z) : x(x), y(y), z(z) {}

    float x;
    float y;
    float z;
};

int main() {
    Camera Camera;
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.majorVersion = 4;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Core;

    sf::Window window(sf::VideoMode(1000, 800, 32), "First Window",
        sf::Style::Titlebar | sf::Style::Close, settings);

    glewExperimental = GL_TRUE;

    if (GLEW_OK != glewInit()) {
        std::cout << "Error:: glew not init =(" << std::endl;
        return -1;
    }
    // настройки
    glEnable(GL_DEPTH_TEST);

    // шейдеры и текстуры
    Shader shader("res/shaders/materials.vs", "res/shaders/materials.fs");
    Shader lamp("res/shaders/lamp.vs", "res/shaders/lamp.fs");
    Shader skyboxShader("res/shaders/cube.vs", "res/shaders/cube.fs");
    Shader ourShader("res/shaders/model_loading.vs", "res/shaders/model_loading.fs");
    // ====================
    Model ourModel1("res/planet/planet.obj");

    Model ourModel2("res/backpack/backpack.obj");

    glm::vec3 lightPos(5.0f, 20.0f, 5.0f);

    // =============================
    int SIZE = 64;
    int POINTS = (SIZE - 1) * (SIZE - 1);
    std::ifstream fin("points.txt");

    float buf_x, buf_y, buf_z;
    std::vector<std::vector<point>> vertices_buf;
    std::vector<point> fill;
    fill.resize(SIZE);
    for (int i = 0; i < SIZE; i++) {
        vertices_buf.push_back(fill);
    }
    
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            fin >> buf_x >> buf_y >> buf_z;
            vertices_buf[x][y] = point(buf_x, buf_z, buf_y);
        }
    }

    float _vertices[36 * (64-1) * (64-1)]; // 36 * (size - 1) * (size - 1)

    int quad_counter = 0;
    for (int pos_x = 0; pos_x < SIZE - 1; pos_x++) {
        for (int pos_y = 0; pos_y < SIZE - 1; pos_y++) {
            // triangle 1
            _vertices[36 * quad_counter + 0] = vertices_buf[pos_x][pos_y].x - 32; // vert 0 * point
            _vertices[36 * quad_counter + 1] = vertices_buf[pos_x][pos_y].y - 20; // vert 0 * point
            _vertices[36 * quad_counter + 2] = vertices_buf[pos_x][pos_y].z - 32; // vert 0 * point
                      
            _vertices[36 * quad_counter + 6] = vertices_buf[pos_x + 1][pos_y].x - 32 ; // vert 1 * point
            _vertices[36 * quad_counter + 7] = vertices_buf[pos_x + 1][pos_y].y - 20; // vert 1 * point
            _vertices[36 * quad_counter + 8] = vertices_buf[pos_x + 1][pos_y].z - 32; // vert 1 * point
                      
            _vertices[36 * quad_counter + 12] = vertices_buf[pos_x][pos_y + 1].x - 32; // vert 2 * point
            _vertices[36 * quad_counter + 13] = vertices_buf[pos_x][pos_y + 1].y - 20; // vert 2 * point
            _vertices[36 * quad_counter + 14] = vertices_buf[pos_x][pos_y + 1].z - 32; // vert 2 * point

            // triangle 2
            _vertices[36 * quad_counter + 18] = vertices_buf[pos_x + 1][pos_y].x - 32; // vert 3 * point
            _vertices[36 * quad_counter + 19] = vertices_buf[pos_x + 1][pos_y].y - 20; // vert 3 * point
            _vertices[36 * quad_counter + 20] = vertices_buf[pos_x + 1][pos_y].z - 32; // vert 3 * point
                     
            _vertices[36 * quad_counter + 24] = vertices_buf[pos_x][pos_y + 1].x - 32; // vert 4 * point
            _vertices[36 * quad_counter + 25] = vertices_buf[pos_x][pos_y + 1].y - 20; // vert 4 * point
            _vertices[36 * quad_counter + 26] = vertices_buf[pos_x][pos_y + 1].z - 32; // vert 4 * point

            _vertices[36 * quad_counter + 30] = vertices_buf[pos_x + 1][pos_y + 1].x - 32; // vert 5 * point
            _vertices[36 * quad_counter + 31] = vertices_buf[pos_x + 1][pos_y + 1].y - 20; // vert 5 * point
            _vertices[36 * quad_counter + 32] = vertices_buf[pos_x + 1][pos_y + 1].z - 32; // vert 5 * point

            // normal 1
            auto vec1_1 = glm::vec3(
                _vertices[36 * quad_counter + 12] - _vertices[36 * quad_counter + 0],
                _vertices[36 * quad_counter + 13] - _vertices[36 * quad_counter + 1],
                _vertices[36 * quad_counter + 14] - _vertices[36 * quad_counter + 2]
            ); // vert2 - vert0

            auto vec1_2 = glm::vec3(
                _vertices[36 * quad_counter + 6] - _vertices[36 * quad_counter + 0],
                _vertices[36 * quad_counter + 7] - _vertices[36 * quad_counter + 1],
                _vertices[36 * quad_counter + 8] - _vertices[36 * quad_counter + 2]
            ); // vert1 - vert0

            auto norm1 = glm::cross(vec1_1, vec1_2);

            _vertices[36 * quad_counter + 3] = norm1.x; // vert 0 * normal
            _vertices[36 * quad_counter + 4] = norm1.y; // vert 0 * normal
            _vertices[36 * quad_counter + 5] = norm1.z; // vert 0 * normal

            _vertices[36 * quad_counter + 9] = norm1.x; // vert 1 * normal
            _vertices[36 * quad_counter + 10] = norm1.y; // vert 1 * normal
            _vertices[36 * quad_counter + 11] = norm1.z; // vert 1 * normal

            _vertices[36 * quad_counter + 15] = norm1.x; // vert 2 * normal
            _vertices[36 * quad_counter + 16] = norm1.y; // vert 2 * normal
            _vertices[36 * quad_counter + 17] = norm1.z; // vert 2 * normal

            // normal 2
            auto vec2_1 = glm::vec3(
                _vertices[36 * quad_counter + 30] - _vertices[36 * quad_counter + 18],
                _vertices[36 * quad_counter + 31] - _vertices[36 * quad_counter + 19],
                _vertices[36 * quad_counter + 32] - _vertices[36 * quad_counter + 20]
            ); // vert5 - vert3

            auto vec2_2 = glm::vec3(
                _vertices[36 * quad_counter + 24] - _vertices[36 * quad_counter + 18],
                _vertices[36 * quad_counter + 25] - _vertices[36 * quad_counter + 19],
                _vertices[36 * quad_counter + 26] - _vertices[36 * quad_counter + 20]
            ); // vert4 - vert3

            auto norm2 = glm::cross(vec1_1, vec1_2);

            _vertices[36 * quad_counter + 21] = norm2.x; // vert 3 * normal
            _vertices[36 * quad_counter + 22] = norm2.y; // vert 3 * normal
            _vertices[36 * quad_counter + 23] = norm2.z; // vert 3 * normal

            _vertices[36 * quad_counter + 27] = norm2.x; // vert 4 * normal
            _vertices[36 * quad_counter + 28] = norm2.y; // vert 4 * normal
            _vertices[36 * quad_counter + 29] = norm2.z; // vert 4 * normal

            _vertices[36 * quad_counter + 33] = norm2.x; // vert 5 * normal
            _vertices[36 * quad_counter + 34] = norm2.y; // vert 5 * normal
            _vertices[36 * quad_counter + 35] = norm2.z; // vert 5 * normal

            // counter
            quad_counter++;
        }
    }

    std::cout << quad_counter << "\n";

    // =============================
    float vertices[] = {
        -1.0f, 1.0f, -1.0f,//Point A 0
        -1.0f, 1.0f, 1.0f,//Point B 1
        1.0f, 1.0f, -1.0f,//Point C 2
        1.0f, 1.0f, 1.0f,//Point D 3

        -1.0f, -1.0f, -1.0f,//Point E 4
        -1.0f, -1.0f, 1.0f,//Point F 5
        1.0f, -1.0f, -1.0f,//Point G 6
        1.0f, -1.0f, 1.0f,//Point H 7
    };

    unsigned int indices[] = {
        /*Above ABC,BCD*/
        0,1,2,
        1,2,3,
        /*Following EFG,FGH*/
        4,5,6,
        5,6,7,
        /*Left ABF,AEF*/
        0,1,5,
        0,4,5,
        /*Right side CDH,CGH*/
        2,3,7,
        2,6,7,
        /*ACG,AEG*/
        0,2,6,
        0,4,6,
        /*Behind BFH,BDH*/
        1,5,7,
        1,3,7
    };


    // =================================== cube map
    std::vector<std::string> faces = {
        "res/cube/posx.jpg",
        "res/cube/negx.jpg",
        "res/cube/posy.jpg",
        "res/cube/negy.jpg",
        "res/cube/posz.jpg",
        "res/cube/negz.jpg",
    };

    // ===================================
    VertexArrayObject cube_VAO;
    VertexBufferObject locationVBO(_vertices, sizeof(_vertices));
    VertexArrayObject::link_vertex_attr(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) nullptr);
    VertexArrayObject::link_vertex_attr(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    // ========================================================================
    VertexArrayObject cubeVAO;
    VertexBufferObject cubeVBO(vertices, sizeof(vertices));
    VertexArrayObject::link_vertex_attr(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) nullptr);
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // =============================================
    unsigned int cubemapTexture = loadCubemap(faces);

    shader.use();
    shader.set_int("texture1", 0);

    skyboxShader.use();
    skyboxShader.set_int("skybox", 0);

    // ====================================
    sf::Mouse::setPosition(sf::Vector2i(1000 / 2, 800 / 2), window);
    Camera.set_pos(0.0f, 10.0f, 0.0f);

    bool isGo = true;
    float phi = 0.0f;

    while (isGo) {
        phi += 0.00005f;
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
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // ================================================
        lamp.use();

        float x = sin(phi) * 20 * lightPos.x;
        float y = lightPos.y;
        float z = cos(phi) * 20 * lightPos.z;

        lamp.set_mat4("projection", Camera.get_projection());
        lamp.set_mat4("view", Camera.get_view());
        lamp.set_mat4(
            "model",
            glm::scale(glm::translate(Camera.get_model(), glm::vec3(x, y, z)), glm::vec3(3.0f))
        );

        cubeVAO.bind_array();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        // model
        // Убеждаемся, что активировали шейдер прежде, чем настраивать uniform-переменные/объекты_рисования
        ourShader.use();

        // Преобразования Вида/Проекции
        ourShader.set_mat4("projection", Camera.get_projection());
        ourShader.set_mat4("view", Camera.get_view());

        // Рендеринг загруженной модели 1
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // смещаем вниз чтобы быть в центре сцены
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// объект слишком большой для нашей сцены, поэтому немного уменьшим его
        ourShader.set_mat4("model", model);
        ourModel1.Draw(ourShader);

        // Рендеринг загруженной модели 2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f)); // смещаем вниз чтобы быть в центре сцены
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// объект слишком большой для нашей сцены, поэтому немного уменьшим его
        ourShader.set_mat4("model", model);
        ourModel2.Draw(ourShader);
        // ================================================ obsidian
        shader.use();

        shader.set_vec3("viewPos", Camera.get_pos());

        shader.set_vec3("light.ambient", 1.0f, 0.5f, 1.0f);
        shader.set_vec3("light.diffuce", 1.0f, 0.5f, 0.31f);
        shader.set_vec3("light.specular", 1.0f, 1.0f, 1.0f);
        shader.set_vec3("light.position", glm::vec3(x, y, z));

        shader.set_vec3("material.ambient", glm::vec3(0.05, 0.05, 0.0)); // определяет, какой цвет отражает поверхность при фоновом освещении (обычно он совпадает с цветом поверхности)
        shader.set_vec3("material.diffuse", glm::vec3(0.5, 0.5, 0.4)); // определяет цвет поверхности при рассеянном освещении. Рассеянный цвет (так же, как и фоновое освещение) устанавливается на желаемый цвет поверхности
        shader.set_vec3("material.specular", glm::vec3(0.7, 0.7, 0.04)); // задает цвет зеркального блика на поверхности (или, возможно, даже отражает специфический для поверхности цвет)
        shader.set_float("material.shininess", 0.078125f * 128); // влияет на рассеивание/радиус зеркального блика.

        shader.set_mat4("projection", Camera.get_projection());
        shader.set_mat4("view", Camera.get_view());
        shader.set_mat4("model", Camera.get_model());

        cube_VAO.bind_array();
        glDrawArrays(GL_TRIANGLES, 0, 18 * POINTS);
        // ================================================
        glDepthFunc(GL_LEQUAL); // меняем функцию глубины, чтобы обеспечить прохождение теста глубины, когда значения равны содержимому буфера глубины
        skyboxShader.use();
        auto view = glm::mat4(glm::mat3(Camera.get_view())); // убираем из матрицы вида секцию, отвечающую за операцию трансляции
        skyboxShader.set_mat4("view", view);
        skyboxShader.set_mat4("projection", Camera.get_projection());

        // Куб скайбокса
        cubeVAO.bind_array();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthFunc(GL_LESS);
        // ================================================
        window.display();
    }


    window.close();
    return 0;
}
