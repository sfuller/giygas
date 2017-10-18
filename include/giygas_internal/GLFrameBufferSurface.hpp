#pragma once
#include <giygas/FrameBufferSurface.hpp>
#include "GL.hpp"
#include "GLSurfaceRenderer.hpp"

namespace giygas {
    class GLFrameBufferSurface : public FrameBufferSurface {
        GL *_gl;
        GLuint _handle;
        GLSurfaceRenderer _renderer;
        unsigned int _width;
        unsigned int _height;

        void move_common(GLFrameBufferSurface &&other) noexcept;
        GLenum get_attachment_flag(SurfaceBufferType attachment_type) const;

    public:
        GLFrameBufferSurface(GL *gl);
        GLFrameBufferSurface(const GLFrameBufferSurface &) = delete;
        GLFrameBufferSurface(GLFrameBufferSurface &&) noexcept;
        GLFrameBufferSurface &operator=(const GLFrameBufferSurface &) = delete;
        GLFrameBufferSurface &operator=(GLFrameBufferSurface &&) noexcept;
        ~GLFrameBufferSurface() override;

        void attach_texture(
            Texture *texture,
            SurfaceBufferType attachment_type
        ) override;

        void attach_renderbuffer(
            RenderBuffer *renderbuffer,
            SurfaceBufferType attachment_type
        ) override;

        void set_size(unsigned int width, unsigned int height);

        unsigned int width() const override;
        unsigned int height() const override;

        void set_viewport(
            unsigned int x,
            unsigned int y,
            unsigned int width,
            unsigned int height
        ) override;

        void set_clear_color(Vector4 color) override;
        void set_clear_depth(double value) override;
        void set_clear_stencil(int value) override;
        void clear(SurfaceBufferType surfaces) override;

        void draw(
            VertexArray *vao,
            ElementBuffer<unsigned int> *ebo,
            Material *material,
            ElementDrawInfo element_info
        ) override;

        void draw(
            VertexArray *vao,
            ElementBuffer<unsigned short> *ebo,
            Material *material,
            ElementDrawInfo element_info
        ) override;

        void draw(
            VertexArray *vao,
            ElementBuffer<unsigned char> *ebo,
            Material *material,
            ElementDrawInfo element_info
        ) override;
    };
}