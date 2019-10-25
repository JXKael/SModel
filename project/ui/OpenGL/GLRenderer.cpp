#include "GLRenderer.h"

using namespace ui;

GLRenderer::GLRenderer() : vbo(QGLBuffer::VertexBuffer) {
    model = glm::mat4x4(1.0f);
    view = glm::mat4x4(1.0f);
    projection = glm::mat4x4(1.0f);

    clear_color = glm::vec4(1.0f);
}

GLRenderer::GLRenderer(const QString &vertex_shader, const QString &frag_shader)
  : vbo(QGLBuffer::VertexBuffer),
    vertex_shader_(vertex_shader),
    frag_shader_(frag_shader)
{
    model = glm::mat4x4(1.0f);
    view = glm::mat4x4(1.0f);
    projection = glm::mat4x4(1.0f);

    clear_color = glm::vec4(1.0f);
}

GLRenderer::~GLRenderer() {
    this->ClearVertices();
    this->ClearTextures();
    vbo.destroy();
    vao.destroy();
}

void GLRenderer::InitializeGL() {
    initializeOpenGLFunctions();
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    bool success = false;
    if (!vao.isCreated()) {
        success = vao.create();
        assert(success);
    }

    if (!vbo.isCreated()) {
        success = vbo.create();
        assert(success);
    }

    // ---------------- 读取shader源代码，编译shader源代码，并关联到shader program
    bool v_success = shader_program.addShaderFromSourceFile(QOpenGLShader::Vertex, vertex_shader_);
    bool f_success = shader_program.addShaderFromSourceFile(QOpenGLShader::Fragment, frag_shader_);
    // ---------------- 链接着色器程序对象
    bool l_success = shader_program.link();
    if (!(l_success && v_success && f_success)) {
        std::cout << "shader compile error: " << std::endl;
        std::cout << "vshader: " << vertex_shader_.toStdString() << std::endl;
        std::cout << "fshader: " << frag_shader_.toStdString() << std::endl;
        std::cout << "shaders log: " << shader_program.log().toStdString() << std::endl;
        exit(EXIT_FAILURE);
    }

    this->BindStaticDrawData();
}

void GLRenderer::ResizeGL(int w, int h) {
    // 防止窗口大小变为0
    if (h == 0) h = 1;
    // 重置当前的视口
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void GLRenderer::SetVertices(const std::vector<float> &vertices) {
    this->vertices.clear();
    this->vertices = vertices;
}

void GLRenderer::AddVertices(const std::vector<float> &vertices) {
    for (float v : vertices) {
        this->vertices.push_back(v);
    }
}

void GLRenderer::AddTexture(const std::string &tex_name, const QString &tex_path) {
    if (texture_paths.size() < texture_max_num)
        texture_paths[tex_name] = tex_path;
}

void GLRenderer::LoadTextures() {
    for (const std::pair<std::string, QString> &texture_path : texture_paths) {
        const std::string &tex_name = texture_path.first;
        const QString &tex_path = texture_path.second;

        QImage image;
        if (!image.load(tex_path)) {
            std::cerr << "error loading texture: " << tex_path.toStdString() << std::endl;
            exit(EXIT_FAILURE);
        }
        QOpenGLTexture *texture = new QOpenGLTexture(image.mirrored());
        textures[tex_name] = texture;

    }
}

void GLRenderer::ClearVertices() {
    vertices.clear();
}

void GLRenderer::ClearTextures() {
    texture_paths.clear();
    texture_ids.clear();
    for (const std::pair<std::string, QOpenGLTexture *> &texture : textures) {
        if (nullptr != texture.second) {
            texture.second->destroy();
            delete texture.second;
        }
    }
    textures.clear();
}

void GLRenderer::BindStaticDrawData() {
    if (vertices.empty()) return;

    bool success = false;
    vao.bind();
    success = vbo.bind();
    assert(success);
    vbo.setUsagePattern(QGLBuffer::UsagePattern::StaticDraw);

    success = shader_program.bind();
    assert(success);

    vbo.allocate(vertices.data(), sizeof(vertices[0]) * (int)vertices.size());

    this->PassVerticesToShader();
    this->PassVertexColorToShader();
    this->PassTexcoordToShader();
    this->LoadTextures();
    this->PassTextureToShader();

    shader_program.release();
    vao.release();
    vbo.release();
}

void GLRenderer::PaintGL() {
    if (!vertices.empty()) {
        vao.bind();
        shader_program.bind();

        this->PassMVPToShader();
        this->PassOtherToShader();
        this->ActiveTexture();

        Draw();

        shader_program.release();
        vao.release();
    }
}

void GLRenderer::ActiveTexture() {
    for (const std::pair<std::string, QOpenGLTexture *> &tex_pair : textures) {
        const std::string &tex_name = tex_pair.first;
        QOpenGLTexture *texture = tex_pair.second;

        texture->bind(texture_ids[tex_name]);
    }
}

void GLRenderer::Draw() {
    glDrawArrays(render_mode, 0, (GLsizei)(vertices.size() / 8));
}

// Called in InitializeGL
void GLRenderer::PassVerticesToShader() {
    // 顶点
    shader_program.setAttributeBuffer("position", GL_FLOAT, 0, 3, 8 * sizeof(float));
    shader_program.enableAttributeArray("position");
}

void GLRenderer::PassVertexColorToShader() {
    // 颜色
    shader_program.setAttributeBuffer("color", GL_FLOAT, 3 * sizeof(float), 3, 8 * sizeof(float));
    shader_program.enableAttributeArray("color");
}

void GLRenderer::PassTexcoordToShader() {
    // 纹理坐标
    shader_program.setAttributeBuffer("texcoord", GL_FLOAT, 6 * sizeof(float), 2, 8 * sizeof(float));
    shader_program.enableAttributeArray("texcoord");
}

void GLRenderer::PassTextureToShader() {
    int id = 0;
    // 纹理
    for (const std::pair<std::string, QOpenGLTexture *> &tex_pair : textures) {
        if (id < texture_max_num) {
            const std::string &tex_name = tex_pair.first;
            QOpenGLTexture *texture = tex_pair.second;

            texture->setMinificationFilter(QOpenGLTexture::Linear);
            texture->setMagnificationFilter(QOpenGLTexture::Linear);

            shader_program.setUniformValue(tex_name.c_str(), id);

            texture_ids[tex_name] = id;

            ++id;
        } else {
            break;
        }
    }
}

// Called in Paint
void GLRenderer::PassMVPToShader() {
    glm::mat4 MVP = projection * view * model;
    shader_program.setUniformValue("MVP", QMatrix4x4(glm::value_ptr(MVP), 4, 4));
}

void GLRenderer::PassOtherToShader() {}

