#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <mwl_image.h>

int main(void) {
    MWL_Image *image = MWL_import_image("../../samples/qoi/testcard_rgba.qoi");

    int res = MWL_export_image(image, MWL_QOI, "test.qoi");
    MWL_free_image(image);
    if (res != MWL_SUCCESS) {
        printf("\nERROR\n\n");
        exit(EXIT_FAILURE);
    }
    
    image = MWL_import_image("test.qoi");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int win_h = (int)image->height;
    int win_w = (int)image->width;
    GLFWwindow *window = glfwCreateWindow(win_w, win_h, "Image Handler", NULL, NULL);
    if (window == NULL) {
        printf("Window creation failed\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("GLAD init failed\n");
        exit(EXIT_FAILURE);
    }

    // Enables alpha channel blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const char *vert_str =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 texCoord;\n"
        "void main() {\n"
        "     gl_Position = vec4(aPos, 1.0);\n"
        "     texCoord = aTexCoord;\n"
        "}\0";
    const char *frag_str =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 texCoord;\n"
        "uniform sampler2D tex;\n"
        "void main() {\n"
        "     vec2 texCoord = vec2(texCoord.x, -texCoord.y);\n"
        "     FragColor = texture(tex, texCoord);\n"
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
        // position        // texture coords
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top right
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
       -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
       -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top left
    };
    uint32_t indices[] = {
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
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

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLint channels = image->channels == MWL_RGB ? GL_RGB : GL_RGBA;

    uint32_t tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, channels, win_w, win_h, 0, channels, GL_UNSIGNED_BYTE, image->data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glUniform1i(glGetUniformLocation(program, "tex"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUseProgram(program);
    glBindVertexArray(VAO);
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(program);

    glfwTerminate();

    MWL_free_image(image);

    return 0;
}

