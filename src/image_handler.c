#include "image_handler.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ppm.h"
#include "qoi.h"

void IH_render_init(void) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void IH_render_terminate(void) {
    glfwTerminate();
}

void IH_import_image(struct IH_Image* image, const char* path, enum IH_image_type type) {
    FILE *fptr = fopen(path, "r");
    if(fptr == NULL) {
        perror("FILE READ ERROR");
        image = NULL;
        return;
    }

    fseek(fptr, 0L, SEEK_END);
    uint64_t file_length = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    switch(type) {
        case IH_PPM: {
            IH_ppm_to_raw(image, fptr, file_length);
            break;
        }
        case IH_QOI: {
            IH_qoi_to_raw(image, fptr, file_length);
            break;
        }
        default:
            exit(EXIT_FAILURE);
    }

    fclose(fptr);
}

void *loop_window(void* arg) {
    struct IH_Image *image = (struct IH_Image*)arg;
    GLFWwindow *window = glfwCreateWindow((int)image->width, (int)image->height, "Image Handler", NULL, NULL);
    if(window == NULL) {
        printf("Window creation failed\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("GLAD init failed\n");
        exit(EXIT_FAILURE);
    }

    const char *vert_str =
            "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "layout (location = 1) in vec2 aTexCoord;\n"
            "out vec2 texCoord;\n"
            "void main() {\n"
            "   gl_Position = vec4(aPos, 1.0);\n"
            "   texCoord = aTexCoord;\n"
            "}\0";
    const char *frag_str =
            "#version 330 core\n"
            "out vec4 FragColor;\n"
            "in vec2 texCoord;\n"
            "uniform sampler2D tex;\n"
            "void main() {\n"
            "   vec2 texCoord = vec2(texCoord.x, -texCoord.y);\n"
            "   FragColor = texture(tex, texCoord);\n"
            "}\0";

    uint32_t vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vert_str, NULL);
    glCompileShader(vert_shader);

    uint32_t frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &frag_str, NULL);
    glCompileShader(frag_shader);

    uint32_t program = glCreateProgram();
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    float vertices[] = {
            // position          // texture coords
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // top right
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom right
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, // top left
    };
    uint32_t indices[] = {
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
    };
    uint32_t VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLint channels = image->channels == IH_RGB ? GL_RGB : GL_RGBA;

    uint32_t tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, channels, (int)image->width, (int)image->height, 0, channels, GL_UNSIGNED_BYTE, image->data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glUniform1i(glGetUniformLocation(program, "tex"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUseProgram(program);
    glBindVertexArray(VAO);
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(program);
}

IH_render_handle IH_render_image(struct IH_Image *image) {
    IH_render_handle thread_id;
    pthread_create(&thread_id, NULL, loop_window, image);
    return thread_id;
}

void IH_join_handle(IH_handle handle) {
    pthread_join(handle, NULL);
}

void IH_delete_image(struct IH_Image* image) {
    if(image != NULL)
        free(image->data);
}
