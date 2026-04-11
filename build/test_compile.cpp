#include <cstdio>
#include <cmath>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

struct Dummy {
    static constexpr int NUM_SPOTS = 6;
    static constexpr float R = 4.0f;
    static constexpr float WALL_H = 2.2f;
    static constexpr float CONE_H = 5.0f;
};

int main() {
    char buf[64];
    for (int i = 0; i < Dummy::NUM_SPOTS; ++i) {
        snprintf(buf, sizeof buf, "circusSpotLights[%d]", i);
        string base(buf);
        float ang = 6.28318530f * i / 6 + 0.52f;
        float ca = cosf(ang), sa = sinf(ang);
        float ANCHOR_R = Dummy::R * 1.85f;
        glm::vec3 postTop(ANCHOR_R * ca, 0.7f, ANCHOR_R * sa);
        glm::vec3 apex(0, Dummy::WALL_H + Dummy::CONE_H, 0);
        glm::vec3 toApex = glm::normalize(apex - postTop);
        glm::vec3 aimDir = glm::normalize(glm::mix(glm::vec3(0,1,0), toApex, 0.5f));
        printf("%s %f %f %f\n", base.c_str(), aimDir.x, aimDir.y, aimDir.z);
    }
    return 0;
}
