#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include <iostream>
#include <vector>
#include <map>

#include <QGLbuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>

#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ui {

class GLRenderer : public QOpenGLFunctions {
protected:
    // 着色器路径
    QString vertex_shader_;
    QString frag_shader_;

    // 背景颜色
    glm::vec4 clear_color;

    // 渲染
    QOpenGLVertexArrayObject vao;
    QOpenGLShaderProgram shader_program;
    QGLBuffer vbo;
    GLenum render_mode;

    // 变换矩阵
    Camera camera;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    std::vector<float> vertices;

    // 纹理
    const int texture_max_num = 6;
    std::map<std::string, QString> texture_paths;
    std::map<std::string, QOpenGLTexture *> textures;
    std::map<std::string, GLuint> texture_ids;

    std::string name;
public:
    GLRenderer();
    GLRenderer(const QString &vertex_shader, const QString &frag_shader);
    ~GLRenderer();
    virtual void InitializeGL();
    virtual void ResizeGL(int w, int h);

    inline void SetCamera(const Camera &camera) { this->camera = camera; }
    inline void SetModel(glm::mat4 model) { this->model = model; }
    inline void SetView(glm::mat4 view) { this->view = view; }
    inline void SetProjection(glm::mat4 projection) { this->projection = projection; }
    inline void SetClearColor(glm::vec4 color) { this->clear_color = color; }
    inline void SetClearColor(float r, float g, float b, float a) { this->clear_color = glm::vec4(r, g, b, a); }
    inline void SetRenderMode(GLenum mode) { this->render_mode = mode; }
    inline void SetName(const std::string &name) { this->name = name; }
    inline const std::string &GetName() const { return name; }

    virtual void SetVertices(const std::vector<float> &vertices);
    virtual void AddVertices(const std::vector<float> &vertices);
    virtual void AddTexture(const std::string &tex_name, const QString &tex_path);
    virtual void ClearVertices();
    virtual void ClearTextures();

    virtual void BindStaticDrawData();
    virtual void BindDynamicDrawData();

    // Call this function to draw everything
    virtual void PaintGL();
    virtual void Draw();

    virtual void PassVerticesToShader();
    virtual void PassVertexColorToShader();
    virtual void PassTexcoordToShader();

    virtual void LoadTextures();
    virtual void PassTextureToShader();

    virtual void PassDynamicVerticesToShader();
    virtual void PassMVPToShader();
    virtual void ActiveTexture();
    virtual void PassOtherToShader();
};

}; // namespace ui

#endif // GL_RENDERER_H
