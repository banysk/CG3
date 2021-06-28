#pragma once

#ifndef VBO_OPENGL
#define VBO_OPENGL

#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class VertexBufferObject {
 public:
    explicit VertexBufferObject(float* vertices, size_t vertices_size) {
        glGenBuffers(1, &ID);
        bind_buffer();
        glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
    };

    void bind_buffer() const {
        glBindBuffer(GL_ARRAY_BUFFER, ID);
    };

    void delete_buffer() {
        glDeleteBuffers(1, &ID);
    }

 private:
    unsigned int ID{};
};
#endif // !VBO_OPENGL