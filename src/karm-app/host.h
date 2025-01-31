#pragma once

#include <karm-base/box.h>
#include <karm-base/rc.h>
#include <karm-base/result.h>
#include <karm-base/slice.h>
#include <karm-events/events.h>
#include <karm-gfx/surface.h>
#include <karm-meta/utils.h>

#include "client.h"

namespace Karm::App {

struct Host : public Meta::Static {
    bool _alive{};
    Error _result{OK};
    Gfx::Context _gfx{};
    Box<Client> _client;

    Host(Box<Client> &&client) : _client(std::move(client)) {}
    virtual ~Host() = default;

    virtual Gfx::Surface surface() = 0;
    virtual void flip(Slice<Math::Recti> regions) = 0;
    virtual void pump() = 0;
    virtual void wait(size_t ms) = 0;

    void handle(Events::Event &event) {
        _client->handle(event);
    }

    void paint() {
        Gfx::Surface s = surface();
        _gfx.begin(s);
        _client->paint(_gfx);
        _gfx.end();

        Array<Math::Recti, 1> dirty = {s.bound()};
        flip(dirty);
    }

    Error run() {
        _alive = true;

        paint();
        while (_alive) {
            pump();
            wait(16);
            paint();
        }

        return _result;
    }

    void exit(Error result = OK) {
        _alive = false;
        _result = result;
    }
};

} // namespace Karm::App
