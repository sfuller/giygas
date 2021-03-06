#include <giygas/GLFWContext.hpp>
#include <giygas/Renderer.hpp>
#include <giygas/giygas.hpp>
#include <giygasutil/GameLoopDelegate.hpp>
#include <giygasutil/GameLoopRunner.hpp>
#include <iostream>
#include <giygasutil/util.hpp>
#include "example_common.hpp"
#include <array>
#include <cmath>

using namespace giygas;
using namespace giygasutil;
using namespace std;
using namespace giygas_examples_common;

class VertexData {
public:
    Vector4 position;
    Vector4 color;
};

class TriangleExampleApp : public GameLoopDelegate {

    unique_ptr<Renderer> _renderer;
    ShaderLoader _shader_loader;
    unique_ptr<Shader> _vertex_shader;
    unique_ptr<Shader> _fragment_shader;
    unique_ptr<Pipeline> _pipeline;
    unique_ptr<VertexBuffer> _vertex_buffer;
    unique_ptr<IndexBuffer8> _index_buffer;
    unique_ptr<RenderPass> _pass;
    unique_ptr<Framebuffer> _swapchain_framebuffer;

public:

    TriangleExampleApp(GLFWContext &context, const char *arg0) {
        _renderer = unique_ptr<Renderer>(giygas::make_renderer(&context));
        _shader_loader = ShaderLoader(get_content_dir(arg0).c_str(), _renderer->renderer_type());
    }

    void init() {
        _renderer->initialize();

        //
        // Setup mesh
        //
        array<VertexData, 3> verts = {
            VertexData{ Vector4(-1, 1, 0, 1), Vector4(1, 0, 0, 1) },
            VertexData{ Vector4(0, -1, 0, 1), Vector4(0, 1, 0, 1) },
            VertexData{ Vector4(1,  1, 0, 1), Vector4(0, 0, 1, 1) },
        };
        array<uint8_t, 3> indices = {0, 1, 2};

        _vertex_buffer = unique_ptr<VertexBuffer>(_renderer->make_vertex_buffer(VertexBufferCreateFlag_Writable));
        VertexBuffer::set_data(*_vertex_buffer, 0, verts.data(), verts.size());

        _index_buffer = unique_ptr<IndexBuffer8>(_renderer->make_index_buffer_8(IndexBufferCreateFlag_None));
        _index_buffer->set(0, indices.data(), indices.size());

        //
        // Setup Vertex Attribute layout
        //
        array<VertexAttribute, 2> vertex_attribs = {};
        VertexAttribute &position_attrib = vertex_attribs[0];
        VertexAttribute &color_attrib = vertex_attribs[1];
        position_attrib.component_count = 4;
        position_attrib.component_size = sizeof(VertexData::position);
        position_attrib.offset = offsetof(VertexData, position);
        color_attrib.component_count = 4;
        color_attrib.component_size = sizeof(VertexData::color);
        color_attrib.offset = offsetof(VertexData, color);
        VertexAttributeLayout layout = {};
        layout.stride = sizeof(VertexData);
        layout.attribute_count = vertex_attribs.size();
        layout.attributes = vertex_attribs.data();

        //
        // Setup shaders
        //
        size_t vertex_code_size, fragment_code_size;
        unique_ptr<uint8_t[]> vertex_code, fragment_code;
        vertex_code = _shader_loader.get_shader_code("shaders/triangle.vs", vertex_code_size);
        fragment_code = _shader_loader.get_shader_code("shaders/triangle.fs", fragment_code_size);
        _vertex_shader = unique_ptr<Shader>(_renderer->make_shader());
        _fragment_shader = unique_ptr<Shader>(_renderer->make_shader());
        _vertex_shader->set_code(vertex_code.get(), vertex_code_size, ShaderType::Vertex);
        _fragment_shader->set_code(fragment_code.get(), fragment_code_size, ShaderType::Fragment);

        //
        // Setup renderpass
        //
        RenderPassCreateParameters pass_params = {};
        RenderPassAttachment pass_attachment = {};
        pass_attachment.purpose = AttachmentPurpose::Color;
        pass_attachment.target = _renderer->swapchain();
        pass_params.attachment_count = 1;
        pass_params.attachments = &pass_attachment;
        _pass = unique_ptr<RenderPass>(_renderer->make_render_pass());
        _pass->create(pass_params);

        //
        // Create swapchain framebuffers
        //
        _swapchain_framebuffer = unique_ptr<Framebuffer>(_renderer->make_framebuffer());
        giygasutil::create_basic_framebuffer(
            _swapchain_framebuffer.get(),
            _renderer->swapchain(),
            _pass.get()
        );

        //
        // Setup pipeline
        //
        array<const Shader *, 2> shaders = {_vertex_shader.get(), _fragment_shader.get()};
        PipelineCreateParameters pipeline_params = {};
        pipeline_params.viewport.width = _renderer->swapchain()->width();
        pipeline_params.viewport.height = _renderer->swapchain()->height();
        pipeline_params.viewport.min_depth = 0;
        pipeline_params.viewport.min_depth = 1;
        pipeline_params.scissor.width = _renderer->swapchain()->width();
        pipeline_params.scissor.height = _renderer->swapchain()->height();
        pipeline_params.shader_count = 2;
        pipeline_params.shaders = shaders.data();
        pipeline_params.vertex_buffer_layout_count = 1;
        pipeline_params.vertex_buffer_layouts = &layout;
        pipeline_params.pass = _pass.get();
        _pipeline = unique_ptr<Pipeline>(_renderer->make_pipeline());
        _pipeline->create(pipeline_params);
    }

    float _t = 0.0f;

    void update_logic(float elapsed_seconds) override {
        array<VertexData, 3> verts = {
            VertexData{ Vector4(-1, 1, 0, 1), Vector4(1, 0, 0, 1) },
            VertexData{ Vector4(0, abs(sin(_t * M_PI)), 0, 1), Vector4(0, 1, 0, 1) },
            VertexData{ Vector4(1,  1, 0, 1), Vector4(0, 0, 1, 1) },
        };

        _t += elapsed_seconds;
        VertexBuffer::set_data(*_vertex_buffer, 0, verts.data(), verts.size());
    }

    void update_graphics() override {
        ClearValue clear_value = {};
        clear_value.purpose = AttachmentPurpose::Color;
        clear_value.color_value = Vector4(0.5f, 0.5f, 1.0f, 1.0f);

        DrawInfo draw_info = {};
        const VertexBuffer *vertex_buffer = _vertex_buffer.get();
        draw_info.pipeline = _pipeline.get();
        draw_info.vertex_buffer_count = 1;
        draw_info.vertex_buffers = &vertex_buffer;
        draw_info.index_buffer = _index_buffer.get();
        draw_info.index_range.offset = 0;
        draw_info.index_range.count = 3;

        PassSubmissionInfo submit_info = {};
        submit_info.draw_count = 1;
        submit_info.draws = &draw_info;
        submit_info.pass_info.pass = _pass.get();
        submit_info.pass_info.framebuffer = _swapchain_framebuffer.get();
        submit_info.pass_info.clear_value_count = 1;
        submit_info.pass_info.clear_values = &clear_value;

        _renderer->submit(&submit_info, 1);
    }

    bool should_close() const override {
        return false;
    }

};

int main (int /*argc*/, char **argv) {
    GLFWContext context;
    TriangleExampleApp app(context, argv[0]);
    GameLoopRunner runner(&context, &app);
    app.init();
    context.show();
    runner.run();
    return 0;
}
