#include <cassert>
#include "VulkanShader.hpp"
#include "VulkanRenderer.hpp"

using namespace giygas;


class ShaderSafeDeletable final : public SwapchainSafeDeleteable {

    VkShaderModule _handle;

public:

    ShaderSafeDeletable(VkShaderModule handle) {
        _handle = handle;
    }

    void delete_resources(VulkanRenderer &renderer) override {
        vkDestroyShaderModule(renderer.device(), _handle, nullptr);
    }

};


VulkanShader::VulkanShader(VulkanRenderer *renderer) {
    _renderer = renderer;
    _module = VK_NULL_HANDLE;
}

VulkanShader::VulkanShader(VulkanShader &&other) noexcept {
    move_common(move(other));
}

VulkanShader& VulkanShader::operator=(VulkanShader &&other) noexcept {
    move_common(move(other));
    return *this;
}

void VulkanShader::move_common(VulkanShader &&other) noexcept {
    _renderer = other._renderer;
    _module = other._module;
    other._module = VK_NULL_HANDLE;
}

VulkanShader::~VulkanShader() {
    _renderer->delete_when_safe(unique_ptr<SwapchainSafeDeleteable>(new ShaderSafeDeletable(_module)));
}

void VulkanShader::set_code(
    const uint8_t *code,
    uint32_t length,
    ShaderType type
) {
    if (_module != VK_NULL_HANDLE) {
        // TODO: Log warning or something
        return;
    }

    // codeSize must be a multiple of 4
    assert(length % 4 == 0);
    assert(length > 0);

    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = length;
    create_info.pCode = reinterpret_cast<const uint32_t *>(code);

    vkCreateShaderModule(
        _renderer->device(),
        &create_info,
        nullptr,
        &_module
    );

    _type = type;
}

RendererType VulkanShader::renderer_type() const {
    return RendererType::Vulkan;
}

ShaderType VulkanShader::shader_type() const {
    return _type;
}

VkShaderModule VulkanShader::module() const {
    return _module;
}

