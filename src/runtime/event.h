//
// Created by Mike Smith on 2021/4/8.
//

#pragma once

#include <runtime/command.h>

namespace luisa::compute {

class Device;

class Event : concepts::Noncopyable {

private:
    Device *_device;
    uint64_t _handle;

public:
    explicit Event(Device &device) noexcept;
    ~Event() noexcept;
    
    Event(Event &&another) noexcept;
    Event &operator=(Event &&rhs) noexcept;
    
    [[nodiscard]] CommandHandle signal() const noexcept;
    [[nodiscard]] CommandHandle wait() const noexcept;
    void synchronize() const noexcept;
};

}// namespace luisa::compute