// AABB Collision System for Village Fair
// Provides axis-aligned bounding box collision detection with "slide" resolution.
#pragma once

#include <glm/glm.hpp>
#include <vector>

struct AABB {
    glm::vec3 mn;  // min corner
    glm::vec3 mx;  // max corner

    AABB() : mn(0.0f), mx(0.0f) {}
    AABB(glm::vec3 minV, glm::vec3 maxV) : mn(minV), mx(maxV) {}

    // Build from center + half-extents
    static AABB fromCenter(glm::vec3 center, glm::vec3 halfSize) {
        return AABB(center - halfSize, center + halfSize);
    }
};

// Returns true if two AABBs overlap on all three axes
inline bool checkCollision(const AABB& a, const AABB& b) {
    return (a.mn.x <= b.mx.x && a.mx.x >= b.mn.x) &&
           (a.mn.y <= b.mx.y && a.mx.y >= b.mn.y) &&
           (a.mn.z <= b.mx.z && a.mx.z >= b.mn.z);
}

// Build player AABB from foot position (thin, tall capsule-like box)
inline AABB playerAABB(glm::vec3 footPos) {
    const float halfW = 0.25f;   // half-width/depth
    const float height = 1.8f;   // full height
    return AABB(
        glm::vec3(footPos.x - halfW, footPos.y,          footPos.z - halfW),
        glm::vec3(footPos.x + halfW, footPos.y + height, footPos.z + halfW)
    );
}

// Global list of static world colliders
inline std::vector<AABB>& getWorldColliders() {
    static std::vector<AABB> colliders;
    return colliders;
}

// Check if a proposed player position collides with any world object
inline bool playerCollidesAny(glm::vec3 footPos) {
    AABB player = playerAABB(footPos);
    for (const auto& c : getWorldColliders()) {
        if (checkCollision(player, c))
            return true;
    }
    return false;
}

// Slide-based movement: try full move, then per-axis fallback
inline glm::vec3 moveWithCollision(glm::vec3 currentPos, glm::vec3 moveDir) {
    glm::vec3 proposed = currentPos + moveDir;

    // Try full movement first
    if (!playerCollidesAny(proposed))
        return proposed;

    // Try X-only slide
    glm::vec3 slideX(proposed.x, currentPos.y, currentPos.z);
    if (!playerCollidesAny(slideX))
        return slideX;

    // Try Z-only slide
    glm::vec3 slideZ(currentPos.x, currentPos.y, proposed.z);
    if (!playerCollidesAny(slideZ))
        return slideZ;

    // Fully blocked — stay put
    return currentPos;
}

// Initialize all static world colliders (call once at startup)
inline void initWorldColliders() {
    auto& c = getWorldColliders();
    c.clear();

    float Y = -0.42f;  // ground level
    float H = 4.0f;    // wall/object height

    // ══════════ Boundary walls (4 thin walls) ══════════
    // Left wall
    c.push_back(AABB(glm::vec3(-31.0f, Y, -30.0f), glm::vec3(-30.0f, Y+H, 26.0f)));
    // Right wall
    c.push_back(AABB(glm::vec3(28.0f, Y, -30.0f), glm::vec3(29.0f, Y+H, 26.0f)));
    // Back wall (far -Z)
    c.push_back(AABB(glm::vec3(-31.0f, Y, -30.0f), glm::vec3(29.0f, Y+H, -29.0f)));
    // Front wall (far +Z)
    c.push_back(AABB(glm::vec3(-31.0f, Y, 25.0f), glm::vec3(29.0f, Y+H, 26.0f)));

    // ══════════ Circus Tent — center(-5, -4), R≈4 ══════════
    c.push_back(AABB::fromCenter(glm::vec3(-5.0f, Y+2.0f, -4.0f), glm::vec3(4.0f, 2.5f, 4.0f)));

    // ══════════ Carousel — center(+5, +4), R≈3.2 ══════════
    c.push_back(AABB::fromCenter(glm::vec3(5.0f, Y+2.0f, 4.0f), glm::vec3(3.5f, 2.5f, 3.5f)));

    // ══════════ Banyan Tree — center(-5, +5), R≈2 ══════════
    c.push_back(AABB::fromCenter(glm::vec3(-4.0f, Y+1.5f, 7.0f), glm::vec3(2.0f, 2.0f, 2.0f)));

    // ══════════ Ferris Wheel — center(+5, -6) ══════════
    c.push_back(AABB::fromCenter(glm::vec3(5.0f, Y+3.0f, -6.0f), glm::vec3(3.5f, 3.5f, 3.5f)));

    // ══════════ Shop rows ══════════
    // Left shops (X≈-17, Z from -6 to 5, depth≈2)
    c.push_back(AABB(glm::vec3(-19.5f, Y, -8.0f), glm::vec3(-15.5f, Y+3.0f, 7.0f)));
    // Right shops (X≈15, Z from -6 to 5, depth≈2)
    c.push_back(AABB(glm::vec3(13.5f, Y, -8.0f), glm::vec3(17.5f, Y+3.0f, 7.0f)));
    // Bottom shops (Z≈-16, X from -7 to 11)
    c.push_back(AABB(glm::vec3(-9.0f, Y, -18.5f), glm::vec3(11.0f, Y+3.0f, -14.5f)));

    // ══════════ Pond — elliptical water (RX=6.5, RZ=4.0) at world (-1, -0.3, 12) ══════════
    // Wraps the full oval boundary so the player cannot walk into the water
    c.push_back(AABB(glm::vec3(-1.0f - 6.5f, Y - 0.5f, 20.0f - 4.0f),
                     glm::vec3(-1.0f + 6.5f, Y + 0.5f, 20.0f + 4.0f)));
}
