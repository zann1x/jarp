#include "Renderer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <stdexcept>
#include <spdlog/spdlog.h>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl.h>

#include <stb_image.h>

GLuint vao;
GLuint vbo;
GLuint texture_id;

struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texture;
};

Renderer::Renderer()
    : shader{ "../shaders/basic.vert.glsl", "../shaders/basic.frag.glsl" }
{
}

void Renderer::load_sample_render_data() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 1000 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, color)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, texture)));
    glEnableVertexAttribArray(2);

    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), static_cast<const void*>(indices), GL_STATIC_DRAW);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glActiveTexture(GL_TEXTURE0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int x, y, n;
    stbi_set_flip_vertically_on_load(true);
    stbi_uc* data = stbi_load("../assets/icebear.png", &x, &y, &n, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else {
        spdlog::error("{:s}", stbi_failure_reason());
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    this->shader.bind();
    this->shader.set_int("u_texture_position", 0);
}

static std::array<Vertex, 4> create_quad(float x, float y) {
    float size = 1.0f;

    Vertex bottom_left = {
        .position = glm::vec3(x, y, 0.0f),
        .color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        .texture = glm::vec2(0.0f, 0.0f)
    };
    Vertex bottom_right = {
        .position = glm::vec3(x + size,  y, 0.0f),
        .color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        .texture = glm::vec2(1.0f, 0.0f)
    };
    Vertex top_right = {
        .position = glm::vec3(x + size, y + size, 0.0f),
        .color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
        .texture = glm::vec2(1.0f, 1.0f)
    };
    Vertex top_left = {
        .position = glm::vec3(x, y + size, 0.0f ),
        .color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
        .texture = glm::vec2(0.0f, 1.0f)
    };

    return { bottom_left, bottom_right, top_right, top_left };
}

static bool show_demo_window = false;
static bool show_another_window = false;
static glm::vec3 clear_color = glm::vec3(0.1f, 0.1f, 0.1f);

void Renderer::draw(float delta, const Camera& camera) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if (show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("%.3f ms/frame (%.1f fps)", delta, ImGui::GetIO().Framerate);

        ImGui::End();
    }

    ImGui::Render();

#if 0
    glm::mat4 projection = glm::ortho(-1.6f, 1.6f, -0.9f, 0.9f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);
#else
    glm::mat4 projection = camera.get_projection();
    glm::mat4 view = camera.get_view();
#endif
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;

    this->shader.bind();
    this->shader.set_mat4("u_mvp", mvp);

    //GLfloat vertices[] = {
    //    // vec3 position    // vec4 color
    //    -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // bottom left
    //     0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // bottom right
    //     0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    //    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f  // top left
    //};
    auto vertices = create_quad(-0.5f, -0.5f);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices.data());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
