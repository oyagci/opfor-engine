#pragma once

#include "opfor/renderer/Framebuffer.hpp"
#include <set>

namespace opfor
{

class OpenGLFramebuffer : public Framebuffer
{
  private:
    uint32_t _RendererID;

    std::set<FramebufferAttachment> _Attachments;
    std::vector<SharedPtr<Texture>> _Textures;
    std::vector<SharedPtr<Renderbuffer>> _Renderbuffers;

    void UpdateDrawBuffers() const;

  public:
    OpenGLFramebuffer();
    virtual ~OpenGLFramebuffer();

    void Bind() override;
    void Unbind() override;

    void AttachTexture(SharedPtr<Texture>, FramebufferAttachment) override;
    void AttachRenderbuffer(SharedPtr<Renderbuffer>, RenderbufferAttachment) override;

    uint32_t GetRawHandle() const override
    {
        return _RendererID;
    }

    bool IsComplete() const override;

    void CopyToDefault(CopyTarget target) override;
    void CopyTo(CopyTarget target, Framebuffer &dst) override;
    void CopyDefault(CopyTarget target) override;
};

} // namespace opfor
