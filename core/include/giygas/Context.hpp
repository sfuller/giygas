#pragma once
#include <functional>
#include <giygas/export.h>
#include "RendererType.hpp"
#include "EventHandler.hpp"

namespace giygas {

//    class SurfaceSizeChangedListener {
//    public:
//        virtual ~SurfaceSizeChangedListener() = default;
//        virtual void handle_surface_size_changed(
//            unsigned int width,
//            unsigned int height
//        ) = 0;
//    };

    class GIYGAS_EXPORT Context {
    public:
        virtual ~Context() = default;
        virtual bool is_valid() const = 0;
        virtual void *cast_to_specific(RendererType type) = 0;
        virtual unsigned int framebuffer_width() const = 0;
        virtual unsigned int framebuffer_height() const = 0;

        virtual EventHandler<unsigned int, unsigned int> surface_size_changed() = 0;
//        virtual void add_surface_size_changed_listener(
//            SurfaceSizeChangedListener *listener
//        ) = 0;
//        virtual void remove_surface_size_changed_listener(
//            SurfaceSizeChangedListener *listener
//        ) = 0;
    };
}
