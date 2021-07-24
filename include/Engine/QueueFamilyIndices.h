#ifndef QUEUEFAMILYINDICES_H
#define QUEUEFAMILYINDICES_H

#include <optional>
#include <cstdlib>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};
#endif