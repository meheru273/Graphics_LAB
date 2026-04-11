#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include "shader.h"

// ─────────────────────────────────────────────────────────────────────────────
// Shared upload helper  (pos3 + nrm3 + uv2 = 8 floats per vertex)
// ─────────────────────────────────────────────────────────────────────────────
static void ct_upload(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO,
    const std::vector<float>& v, const std::vector<unsigned int>& i)
{
    glGenVertexArrays(1,&VAO); glGenBuffers(1,&VBO); glGenBuffers(1,&EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, v.size()*4, v.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, i.size()*4, i.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,32,(void*)0);  glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,32,(void*)12); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,32,(void*)24); glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}
static void ct_mat(Shader& s, glm::vec4 a, glm::vec4 d,
    glm::vec4 sp=glm::vec4(.1f,.1f,.1f,1.f), float sh=16.f)
{
    s.setVec4("material.ambient",a); s.setVec4("material.diffuse",d);
    s.setVec4("material.specular",sp); s.setFloat("material.shininess",sh);
}

// ─────────────────────────────────────────────────────────────────────────────
//  CircusTent
//
//  Local space: bottom at Y=0.  All dimensions in local units (= world units
//  when drawn without extra scale).
//  R = 4.0   WALL_H = 2.2   CONE_H = 5.0
//  N = 24 sectors  → ~15° per stripe, clean alternating red/white
//  Entrance gap: sector 0 (centered on +Z) is open on walls + cone
// ─────────────────────────────────────────────────────────────────────────────
struct CircusTent {

    static constexpr int   N      = 24;
    static constexpr float R      = 4.0f;
    static constexpr float WALL_H = 2.2f;
    static constexpr float CONE_H = 5.0f;
    static constexpr float POLE_R = 0.10f;
    static constexpr float TOTAL_H = WALL_H + CONE_H;

    // Geometry objects
    struct Mesh { unsigned int vao=0,vbo=0,ebo=0; int cnt=0; };
    Mesh cone, walls, disk, cyl, ring, seatStep, doorPanel;

    // Door state
    float doorAngle   = 0.0f;
    bool  doorOpening = false;

    // Flag wave time
    float flagTime = 0.0f;

    // Circus spotlight state
    static constexpr int NUM_SPOTS = 6;
    float spotSweep  = 0.0f;   // current sweep angle (radians)
    bool  spotMoving = false;

    // Spotlight colors
    glm::vec3 spotColors[NUM_SPOTS] = {
        {1.0f, 0.10f, 0.10f},   // red
        {0.1f, 0.30f, 1.00f},   // blue
        {1.0f, 1.00f, 0.10f},   // yellow
        {0.1f, 1.00f, 0.10f},   // green
        {1.0f, 0.10f, 1.00f},   // magenta
        {0.1f, 1.00f, 1.00f},   // cyan
    };

    void toggleSpotlights() { spotMoving = !spotMoving; }

    void updateSpotlights(float dt) {
        if (spotMoving)
            spotSweep += 1.4f * dt;
        flagTime += dt;
    }

    // Fill world-space position + direction for spotlight i
    void getSpotParams(const glm::mat4& model, int i,
        glm::vec3& posOut, glm::vec3& dirOut) const
    {
        float mountAngle = 6.28318530f * i / NUM_SPOTS + 0.52f; // offset away from entrance
        float px = R * 0.80f * cosf(mountAngle);
        float pz = R * 0.80f * sinf(mountAngle);
        posOut = glm::vec3(model * glm::vec4(px, WALL_H - 0.1f, pz, 1.f));

        // Aim sweeps around the performance ring, each light has a phase
        float phase = 6.28318530f * i / NUM_SPOTS;
        float aimAngle = spotSweep + phase;
        float aimR = 1.8f;
        glm::vec3 aimWorld = glm::vec3(model *
            glm::vec4(aimR * cosf(aimAngle), 0.05f, aimR * sinf(aimAngle), 1.f));
        dirOut = glm::normalize(aimWorld - posOut);
    }

    // Call each frame to animate
    void updateDoor(float dt) {
        float target = doorOpening ? 90.0f : 0.0f;
        float speed  = 120.0f; // degrees per second
        if (doorAngle < target)
            doorAngle = std::min(doorAngle + speed * dt, target);
        else if (doorAngle > target)
            doorAngle = std::max(doorAngle - speed * dt, target);
    }

    void toggleDoor() { doorOpening = !doorOpening; }

    void build() {
        _cone();
        _walls();
        _disk();
        _cylinder(cyl, 12);
        _circle(ring, 2.2f, 0.06f);
        _seatArc(seatStep, 28);
        _doorQuad(doorPanel);
    }

    // World positions of the 4 corner lamps
    void getLampPositions(const glm::mat4& m, glm::vec3 out[4]) const {
        float step = 6.28318530f/4.f;
        for(int i=0;i<4;++i){
            float a = step*i + 3.14159265f*0.25f;
            glm::vec4 lp(R*0.82f*cosf(a), WALL_H+0.3f, R*0.82f*sinf(a), 1.f);
            out[i] = glm::vec3(m*lp);
        }
    }

    // Interior camera spawn (center of tent, eye height 1.6)
    glm::vec3 interiorEyePos(const glm::mat4& m) const {
        return glm::vec3(m * glm::vec4(0.f, 1.6f, 0.f, 1.f));
    }

    // ── DRAW (exterior + interior both rendered; only the camera differs) ──
    void draw(Shader& s, const glm::mat4& model,
              unsigned int whiteTex, bool litOn) const
    {
        const glm::vec4 RED  (0.85f,0.07f,0.07f,1.f);
        const glm::vec4 WHT  (1.f,  1.f,  1.f,  1.f);
        const glm::vec4 WOOD (0.28f,0.18f,0.09f,1.f);
        const glm::vec4 TAN  (0.72f,0.58f,0.38f,1.f);  // dirt floor
        const glm::vec4 SAND (0.85f,0.78f,0.55f,1.f);
        const glm::vec4 RING_C(0.9f, 0.8f, 0.1f, 1.f); // yellow ring
        const glm::vec4 SPEC (0.12f,0.12f,0.12f,1.f);

        s.setBool("alphaTest", false);
        glBindTexture(GL_TEXTURE_2D, whiteTex);

        // ── CONE (with full cap so interior ceiling is solid) ──────────────
        {
            glm::mat4 cm = model * glm::translate(glm::mat4(1),{0,WALL_H,0});
            s.setMat4("model", cm);
            glBindVertexArray(cone.vao);
            // red stripes
            ct_mat(s, RED, RED, SPEC, 24.f);
            glDrawElements(GL_TRIANGLES, cone.cnt/2, GL_UNSIGNED_INT, 0);
            // white stripes
            ct_mat(s, WHT, WHT, SPEC, 24.f);
            glDrawElements(GL_TRIANGLES, cone.cnt/2, GL_UNSIGNED_INT,
                (void*)(size_t)(cone.cnt/2*4));
        }

        // ── WALLS ──────────────────────────────────────────────────────────
        {
            s.setMat4("model", model);
            glBindVertexArray(walls.vao);
            ct_mat(s, RED, RED, SPEC, 16.f);
            glDrawElements(GL_TRIANGLES, walls.cnt/2, GL_UNSIGNED_INT, 0);
            ct_mat(s, WHT, WHT, SPEC, 16.f);
            glDrawElements(GL_TRIANGLES, walls.cnt/2, GL_UNSIGNED_INT,
                (void*)(size_t)(walls.cnt/2*4));
        }

        // ── DOOR PANELS ────────────────────────────────────────────────────────
        // Gap spans sectors 5-6 (75°-105°), so:
        //   Left  hinge vertex: i=GAP_START=5 → angle=75°
        //   Right hinge vertex: i=GAP_END+1=7 → angle=105°
        // Each panel hangs inward from its hinge and swings outward on open.
        {
            const glm::vec4 DOOR_C(0.55f, 0.08f, 0.08f, 1.f);
            const float TP    = 6.28318530f;
            const float angL  = TP * GAP_START / N;          // 75° left hinge
            const float angR  = TP * (GAP_END+1) / N;        // 105° right hinge
            // hinge world-local positions
            const glm::vec3 hL(cosf(angL)*R, 0.f, sinf(angL)*R);
            const glm::vec3 hR(cosf(angR)*R, 0.f, sinf(angR)*R);
            // panel width = chord between hinge and midpoint of gap (half the gap)
            const float midAng = TP * (GAP_START + (GAP_END-GAP_START+1)*0.5f) / N; // ~90°
            const glm::vec3 mid(cosf(midAng)*R, 0.f, sinf(midAng)*R);
            const float panelW = glm::length(hL - mid) * 1.2f; // narrower panels
            const float ang    = glm::radians(doorAngle);
            // Direction each panel faces when closed: tangent of circle at hinge
            // Left panel swings CCW (toward -X when at 75°), right swings CW
            glBindVertexArray(doorPanel.vao);

            // LEFT panel — hinge at angL, panel extends clockwise (toward mid)
            {
                // rotate panel to align with tangent at hinge, then swing open
                float baseRot = angL + 3.14159265f*0.5f; // tangent direction
                glm::mat4 lm = model
                    * glm::translate(glm::mat4(1), hL)
                    * glm::rotate(glm::mat4(1), -(baseRot + ang), glm::vec3(0,1,0))
                    * glm::scale(glm::mat4(1), {panelW, WALL_H, 0.10f});
                s.setMat4("model", lm);
                ct_mat(s, DOOR_C, DOOR_C, SPEC, 12.f);
                glDrawElements(GL_TRIANGLES, doorPanel.cnt, GL_UNSIGNED_INT, 0);
            }
            // RIGHT panel — hinge at angR, panel extends counter-clockwise
            {
                float baseRot = angR + 3.14159265f*0.5f;
                glm::mat4 rm = model
                    * glm::translate(glm::mat4(1), hR)
                    * glm::rotate(glm::mat4(1), -(baseRot - ang), glm::vec3(0,1,0))
                    * glm::scale(glm::mat4(1), {-panelW, WALL_H, 0.10f});
                s.setMat4("model", rm);
                ct_mat(s, DOOR_C, DOOR_C, SPEC, 12.f);
                glDrawElements(GL_TRIANGLES, doorPanel.cnt, GL_UNSIGNED_INT, 0);
            }
        }

        // ── FLOOR DISK (interior ground) ────────────────────────────────────
        {
            s.setMat4("model", model *
                glm::translate(glm::mat4(1),{0,0.01f,0}));
            ct_mat(s, TAN, SAND, SPEC, 8.f);
            glBindVertexArray(disk.vao);
            glDrawElements(GL_TRIANGLES, disk.cnt, GL_UNSIGNED_INT, 0);
        }

        // ── PERFORMANCE RING (raised yellow circle) ─────────────────────────
        {
            s.setMat4("model", model *
                glm::translate(glm::mat4(1),{0,0.02f,0}));
            ct_mat(s, RING_C, RING_C, SPEC, 32.f);
            glBindVertexArray(ring.vao);
            glDrawElements(GL_TRIANGLES, ring.cnt, GL_UNSIGNED_INT, 0);
        }

        // ── TIERED SEMICIRCULAR SEATING (3 tiers, back half of tent) ────────
        // Each tier is a flat arc wedge, stacked upward and outward.
        // Front half (entrance side) is left open.
        {
            const glm::vec4 SEAT_C (0.55f, 0.28f, 0.10f, 1.f); // wooden brown
            const glm::vec4 RISER_C(0.40f, 0.20f, 0.08f, 1.f);
            const int TIERS = 3;
            const float innerR0 = 2.4f;   // inner radius of first tier
            const float tierW   = 0.55f;  // radial width of each seat arc
            const float tierH   = 0.28f;  // height step per tier
            const float riserH  = 0.22f;  // vertical face of riser

            glBindVertexArray(seatStep.vao);
            for (int t = 0; t < TIERS; ++t) {
                float innerR = innerR0 + t * tierW;
                float outerR = innerR + tierW;
                float yBase  = t * (tierH + riserH) + 0.03f;

                // Seat surface (flat arc)
                glm::mat4 sm = model *
                    glm::translate(glm::mat4(1),{0, yBase + riserH, 0}) *
                    glm::scale(glm::mat4(1),{outerR, tierH, outerR});
                s.setMat4("model", sm);
                ct_mat(s, SEAT_C, SEAT_C, SPEC, 8.f);
                glDrawElements(GL_TRIANGLES, seatStep.cnt, GL_UNSIGNED_INT, 0);

                // Riser (vertical face, slightly taller and narrower)
                glm::mat4 rm = model *
                    glm::translate(glm::mat4(1),{0, yBase, 0}) *
                    glm::scale(glm::mat4(1),{innerR + 0.05f, riserH + tierH*0.5f, innerR + 0.05f});
                s.setMat4("model", rm);
                ct_mat(s, RISER_C, RISER_C, SPEC, 8.f);
                glDrawElements(GL_TRIANGLES, seatStep.cnt, GL_UNSIGNED_INT, 0);
            }
        }

        // ── POLES (4 perimeter + 1 center) ──────────────────────────────────
        glBindVertexArray(cyl.vao);
        ct_mat(s, WOOD, WOOD, SPEC, 8.f);
        float step4 = 6.28318530f/4.f;
        for(int i=0;i<4;++i){
            float a = step4*i + 3.14159265f*0.25f;
            glm::mat4 pm = model *
                glm::translate(glm::mat4(1),{R*0.82f*cosf(a),0.f,R*0.82f*sinf(a)}) *
                glm::scale(glm::mat4(1),{POLE_R, WALL_H+0.5f, POLE_R});
            s.setMat4("model", pm);
            glDrawElements(GL_TRIANGLES, cyl.cnt, GL_UNSIGNED_INT, 0);
        }
        // (central pole removed)

        // ── CORNER POINT-LIGHT LAMP HEADS ──────────────────────────────────
        s.setBool("lightingOn", false);
        {
            glm::vec4 glow(1.8f,1.6f,0.9f,1.f);
            s.setVec4("material.ambient",glow); s.setVec4("material.diffuse",glow);
            for(int i=0;i<4;++i){
                float a = step4*i + 3.14159265f*0.25f;
                glm::mat4 lm = model *
                    glm::translate(glm::mat4(1),{R*0.82f*cosf(a),WALL_H+0.3f,R*0.82f*sinf(a)}) *
                    glm::scale(glm::mat4(1),{0.18f,0.18f,0.18f});
                s.setMat4("model", lm);
                glDrawElements(GL_TRIANGLES, cyl.cnt, GL_UNSIGNED_INT, 0);
            }
        }

        // ── REALISTIC CIRCUS SPOTLIGHT FIXTURES ────────────────────────────
        for (int i = 0; i < NUM_SPOTS; ++i) {
            float mountAngle = 6.28318530f * i / NUM_SPOTS + 0.52f;
            float px = R * 0.80f * cosf(mountAngle);
            float pz = R * 0.80f * sinf(mountAngle);
            glm::vec3 mountPos(px, WALL_H - 0.08f, pz);

            float phase    = 6.28318530f * i / NUM_SPOTS;
            float aimAngle = spotSweep + phase;
            float aimR     = 1.8f;
            glm::vec3 localAim(aimR*cosf(aimAngle), 0.05f, aimR*sinf(aimAngle));
            glm::vec3 aimDir = glm::normalize(localAim - mountPos);

            glm::vec3 up(0,1,0);
            glm::vec3 axis = glm::cross(up, aimDir);
            float sinA = glm::length(axis);
            float cosA = glm::dot(up, aimDir);
            glm::mat4 headRot = (sinA > 0.001f)
                ? glm::rotate(glm::mat4(1), atan2f(sinA,cosA), axis/sinA)
                : glm::mat4(1);

            glm::vec4 sc(spotColors[i].x*1.8f, spotColors[i].y*1.8f, spotColors[i].z*1.8f, 1.0f);
            glm::vec4 bodyCol(0.12f,0.12f,0.12f,1.f);

            // Base disk
            s.setBool("lightingOn", false);
            s.setVec4("material.ambient",bodyCol); s.setVec4("material.diffuse",bodyCol);
            {
                glm::vec3 inward = -glm::normalize(glm::vec3(px,0,pz));
                glm::mat4 bm = model *
                    glm::translate(glm::mat4(1), mountPos) *
                    glm::rotate(glm::mat4(1), atan2f(inward.x,inward.z), glm::vec3(0,1,0)) *
                    glm::rotate(glm::mat4(1), 1.5708f, glm::vec3(1,0,0)) *
                    glm::scale(glm::mat4(1),{0.14f,0.06f,0.14f});
                s.setMat4("model", bm);
                glDrawElements(GL_TRIANGLES, cyl.cnt, GL_UNSIGNED_INT, 0);
            }
            // Arm
            s.setVec4("material.ambient",bodyCol); s.setVec4("material.diffuse",bodyCol);
            {
                glm::mat4 am = model *
                    glm::translate(glm::mat4(1), mountPos) *
                    headRot *
                    glm::scale(glm::mat4(1),{0.04f, 0.28f, 0.04f});
                s.setMat4("model", am);
                glDrawElements(GL_TRIANGLES, cyl.cnt, GL_UNSIGNED_INT, 0);
            }
            // Head (colored glow)
            s.setVec4("material.ambient",sc); s.setVec4("material.diffuse",sc);
            {
                glm::mat4 hm = model *
                    glm::translate(glm::mat4(1), mountPos + aimDir * 0.28f) *
                    headRot *
                    glm::scale(glm::mat4(1),{0.10f, 0.18f, 0.10f});
                s.setMat4("model", hm);
                glDrawElements(GL_TRIANGLES, cyl.cnt, GL_UNSIGNED_INT, 0);
            }
        }

        // ── WAVING RED FLAG ON CONE APEX ───────────────────────────────────
        {
            const int   FSEG  = 12;
            const float FLEN  = 1.2f;
            const float FBASE = 0.35f;
            float apexY = WALL_H + CONE_H;

            std::vector<float>        fv;
            std::vector<unsigned int> fi;
            for (int k = 0; k <= FSEG; ++k) {
                float t     = (float)k / FSEG;
                float x     = t * FLEN;
                // Wave amplitude grows from 0 at pole (t=0) to full at tip (t=1)
                float yOff  = t * 0.14f * sinf(flagTime * 4.5f + t * 5.0f);
                float width = FBASE * (1.0f - t);
                float u     = t;
                fv.insert(fv.end(),{x, yOff+width*0.5f, 0, 0,0,1, u,0.f});
                fv.insert(fv.end(),{x, yOff-width*0.5f, 0, 0,0,1, u,1.f});
            }
            for (int k = 0; k < FSEG; ++k) {
                unsigned b0=k*2,t0=k*2+1,b1=(k+1)*2,t1=(k+1)*2+1;
                fi.insert(fi.end(),{b0,b1,t1, b0,t1,t0});
                fi.insert(fi.end(),{b0,t1,b1, b0,t0,t1});
            }
            unsigned int fVAO,fVBO,fEBO;
            ct_upload(fVAO,fVBO,fEBO,fv,fi);

            glm::mat4 flagM = model *
                glm::translate(glm::mat4(1),{0, apexY, 0}); // fixed direction, no rotation
            s.setMat4("model", flagM);
            s.setBool("lightingOn", false);
            glm::vec4 flagRed(0.9f,0.05f,0.05f,1.f);
            s.setVec4("material.ambient", flagRed);
            s.setVec4("material.diffuse", flagRed);
            s.setVec4("material.specular",glm::vec4(0.1f));
            s.setFloat("material.shininess",4.f);
            glBindVertexArray(fVAO);
            glDrawElements(GL_TRIANGLES,(int)fi.size(),GL_UNSIGNED_INT,0);
            glBindVertexArray(0);
            glDeleteVertexArrays(1,&fVAO);
            glDeleteBuffers(1,&fVBO);
            glDeleteBuffers(1,&fEBO);
        }

        // ── GUY WIRES + ANCHOR POSTS + EXTERIOR SPOTLIGHTS ───────────────────
        // 6 wires from cone-base rim → ground anchors at radius ANCHOR_R
        {
            const float ANCHOR_R = R * 1.85f;   // how far out the anchor posts are
            const float POST_H   = 0.7f;         // anchor post height
            const float WIRE_R   = 0.025f;       // rope radius
            const glm::vec4 ROPE_C(0.55f,0.42f,0.22f,1.f); // tan rope
            const glm::vec4 POST_C(0.28f,0.18f,0.10f,1.f); // dark wood

            s.setBool("lightingOn", litOn);
            glBindVertexArray(cyl.vao);

            for (int i = 0; i < 6; ++i) {
                float ang = 6.28318530f * i / 6 + 0.52f; // same offset as spotlights
                float ca = cosf(ang), sa = sinf(ang);

                // Cone-base attachment point (local)
                glm::vec3 top(R * ca, WALL_H, R * sa);
                // Ground anchor point (local)
                glm::vec3 bot(ANCHOR_R * ca, 0.f, ANCHOR_R * sa);

                // ── ANCHOR POST ────────────────────────────────────────────
                ct_mat(s, POST_C, POST_C);
                {
                    glm::mat4 pm = model *
                        glm::translate(glm::mat4(1), bot) *
                        glm::scale(glm::mat4(1),{0.06f, POST_H, 0.06f});
                    s.setMat4("model", pm);
                    glDrawElements(GL_TRIANGLES, cyl.cnt, GL_UNSIGNED_INT, 0);
                }

                // ── GUY WIRE (thin cylinder from top to bot) ──────────────
                ct_mat(s, ROPE_C, ROPE_C);
                {
                    glm::vec3 diff = bot - top;
                    float len      = glm::length(diff);
                    glm::vec3 dir  = diff / len;
                    glm::vec3 up(0,1,0);
                    glm::vec3 axis = glm::cross(up, dir);
                    float sinA = glm::length(axis);
                    float cosA = glm::dot(up, dir);
                    glm::mat4 rot = (sinA > 0.001f)
                        ? glm::rotate(glm::mat4(1), atan2f(sinA,cosA), axis/sinA)
                        : glm::mat4(1);
                    glm::mat4 wm = model *
                        glm::translate(glm::mat4(1), top) *
                        rot *
                        glm::scale(glm::mat4(1),{WIRE_R, len, WIRE_R});
                    s.setMat4("model", wm);
                    glDrawElements(GL_TRIANGLES, cyl.cnt, GL_UNSIGNED_INT, 0);
                }

                // ── EXTERIOR SPOTLIGHT ON POST TOP (animated, sweeps up) ────
                glm::vec3 postTop(ANCHOR_R*ca, POST_H, ANCHOR_R*sa);
                float phase   = 6.28318530f * i / 6;
                float tilt    = 0.5f + 0.5f * sinf(spotSweep * 1.2f + phase);
                glm::vec3 apex(0, WALL_H + CONE_H, 0);
                glm::vec3 toApex = glm::normalize(apex - postTop);
                glm::vec3 upV2(0,1,0);
                glm::vec3 aimDir = glm::normalize(glm::mix(upV2, toApex, tilt));

                glm::vec3 upV(0,1,0);
                glm::vec3 axis2 = glm::cross(upV, aimDir);
                float sinA2 = glm::length(axis2);
                float cosA2 = glm::dot(upV, aimDir);
                glm::mat4 headRot = (sinA2 > 0.001f)
                    ? glm::rotate(glm::mat4(1), atan2f(sinA2,cosA2), axis2/sinA2)
                    : glm::mat4(1);

                glm::vec3 col = spotColors[i];
                glm::vec4 sc(col.x*1.8f, col.y*1.8f, col.z*1.8f, 1.f);
                glm::vec4 bodyCol(0.12f,0.12f,0.12f,1.f);

                // post cap / base plate
                s.setBool("lightingOn", false);
                ct_mat(s, bodyCol, bodyCol);
                {
                    glm::mat4 bm = model *
                        glm::translate(glm::mat4(1), postTop) *
                        glm::scale(glm::mat4(1),{0.12f,0.05f,0.12f});
                    s.setMat4("model", bm);
                    glDrawElements(GL_TRIANGLES, cyl.cnt, GL_UNSIGNED_INT, 0);
                }
                // arm
                ct_mat(s, bodyCol, bodyCol);
                {
                    glm::mat4 am = model *
                        glm::translate(glm::mat4(1), postTop) *
                        headRot *
                        glm::scale(glm::mat4(1),{0.04f, 0.30f, 0.04f});
                    s.setMat4("model", am);
                    glDrawElements(GL_TRIANGLES, cyl.cnt, GL_UNSIGNED_INT, 0);
                }
                // head (colored glow)
                ct_mat(s, sc, sc);
                {
                    glm::mat4 hm = model *
                        glm::translate(glm::mat4(1), postTop + aimDir*0.30f) *
                        headRot *
                        glm::scale(glm::mat4(1),{0.10f,0.18f,0.10f});
                    s.setMat4("model", hm);
                    glDrawElements(GL_TRIANGLES, cyl.cnt, GL_UNSIGNED_INT, 0);
                }
            }
        }

        s.setBool("lightingOn", litOn);
        glBindVertexArray(0);
    }

private:
    // ── CONE: all N sectors, red in [0], white in [1] halves of EBO ─────────
    void _cone() {
        const float TP = 6.28318530f;
        std::vector<float> verts;
        std::vector<unsigned int> redI, whtI;

        // apex
        verts.insert(verts.end(),{0,CONE_H,0, 0,1,0, 0.5f,1.f});
        // base ring
        for(int i=0;i<=N;++i){
            float a=TP*i/N, cx=cosf(a)*R, cz=sinf(a)*R;
            float nx=cosf(a), nz=sinf(a), ny=R/CONE_H;
            float len=sqrtf(nx*nx+ny*ny+nz*nz);
            float u=(float)i/N;
            verts.insert(verts.end(),{cx,0,cz, nx/len,ny/len,nz/len, u,0.f});
        }

        // ALL sectors (no entrance gap on cone — roof is fully closed)
        for(int i=0;i<N;++i){
            unsigned a=0, b=i+1, c=i+2;
            if(i%2==0) redI.insert(redI.end(),{a,b,c});
            else        whtI.insert(whtI.end(),{a,b,c});
        }
        // Interior reverse-winding (same geometry, flipped normals via winding)
        for(int i=0;i<N;++i){
            unsigned a=0, b=i+2, c=i+1;
            if(i%2==0) redI.insert(redI.end(),{a,b,c});
            else        whtI.insert(whtI.end(),{a,b,c});
        }

        cone.cnt = (int)(redI.size()+whtI.size());
        std::vector<unsigned int> all=redI;
        all.insert(all.end(),whtI.begin(),whtI.end());
        ct_upload(cone.vao,cone.vbo,cone.ebo,verts,all);
    }

    // ── WALLS: gap centred on +Z (sectors 5 & 6 skipped, gap spans 75°-105°)
    // With N=24: sector i spans angle [i*15°, (i+1)*15°].
    // Sectors 5&6 span 75°-105°, centred exactly on +Z (90°).
    static constexpr int GAP_START = 5;  // first skipped sector
    static constexpr int GAP_END   = 6;  // last  skipped sector (inclusive)

    void _walls() {
        const float TP=6.28318530f;
        std::vector<float> v;
        std::vector<unsigned int> redI, whtI;

        for(int i=0;i<=N;++i){
            float a=TP*i/N, cx=cosf(a)*R, cz=sinf(a)*R;
            float nx=cosf(a),nz=sinf(a),u=(float)i/N;
            v.insert(v.end(),{cx,0,     cz,  nx, 0, nz, u,0.f});
            v.insert(v.end(),{cx,WALL_H,cz,  nx, 0, nz, u,1.f});
            v.insert(v.end(),{cx,0,     cz, -nx, 0,-nz, u,0.f});
            v.insert(v.end(),{cx,WALL_H,cz, -nx, 0,-nz, u,1.f});
        }

        for(int i=0;i<N;++i){
            bool gap = (i >= GAP_START && i <= GAP_END);
            if(gap) continue; // skip entrance sectors on BOTH faces
            // outer
            unsigned b0=i*4, t0=i*4+1, b1=(i+1)*4, t1=(i+1)*4+1;
            if(i%2==0) redI.insert(redI.end(),{b0,b1,t1, b0,t1,t0});
            else        whtI.insert(whtI.end(),{b0,b1,t1, b0,t1,t0});
            // inner
            unsigned ib0=i*4+2, it0=i*4+3, ib1=(i+1)*4+2, it1=(i+1)*4+3;
            if(i%2==0) redI.insert(redI.end(),{ib0,it1,ib1, ib0,it0,it1});
            else        whtI.insert(whtI.end(),{ib0,it1,ib1, ib0,it0,it1});
        }

        walls.cnt=(int)(redI.size()+whtI.size());
        std::vector<unsigned int> all=redI;
        all.insert(all.end(),whtI.begin(),whtI.end());
        ct_upload(walls.vao,walls.vbo,walls.ebo,v,all);
    }

    // ── SOLID FLOOR DISK ──────────────────────────────────────────────────
    void _disk() {
        const float TP=6.28318530f;
        std::vector<float> v;
        std::vector<unsigned int> idx;
        v.insert(v.end(),{0,0,0, 0,1,0, 0.5f,0.5f});
        for(int i=0;i<=N;++i){
            float a=TP*i/N, cx=cosf(a)*R, cz=sinf(a)*R;
            v.insert(v.end(),{cx,0,cz, 0,1,0, cx/(2*R)+0.5f,cz/(2*R)+0.5f});
        }
        for(int i=0;i<N;++i)
            idx.insert(idx.end(),{0u,(unsigned)(i+1),(unsigned)(i+2)});
        disk.cnt=(int)idx.size();
        ct_upload(disk.vao,disk.vbo,disk.ebo,v,idx);
    }

    // ── UNIT CYLINDER (radius=1, h=1) for poles ───────────────────────────
    void _cylinder(Mesh& m, int segs){
        const float TP=6.28318530f;
        std::vector<float> v; std::vector<unsigned int> idx;
        for(int i=0;i<=segs;++i){
            float a=TP*i/segs, cx=cosf(a),cz=sinf(a),u=(float)i/segs;
            v.insert(v.end(),{cx,0,cz, cx,0,cz, u,0.f});
            v.insert(v.end(),{cx,1,cz, cx,0,cz, u,1.f});
        }
        for(int i=0;i<segs;++i){
            unsigned b0=i*2,t0=i*2+1,b1=(i+1)*2,t1=(i+1)*2+1;
            idx.insert(idx.end(),{b0,b1,t1, b0,t1,t0});
        }
        m.cnt=(int)idx.size();
        ct_upload(m.vao,m.vbo,m.ebo,v,idx);
    }

    // ── DOOR PANEL: unit quad (0..1 in X, 0..1 in Y, flat on Z=0) ───────────
    //    Scaled and positioned at draw time.  Double-sided.
    void _doorQuad(Mesh& m) {
        float v[] = {
            // front face
            0,0,0,  0,0,1,  0,0,
            1,0,0,  0,0,1,  1,0,
            1,1,0,  0,0,1,  1,1,
            0,1,0,  0,0,1,  0,1,
            // back face
            0,0,0,  0,0,-1, 1,0,
            1,0,0,  0,0,-1, 0,0,
            1,1,0,  0,0,-1, 0,1,
            0,1,0,  0,0,-1, 1,1,
        };
        unsigned int idx[] = {
            0,1,2, 0,2,3,   // front
            4,6,5, 4,7,6    // back
        };
        m.cnt = 12;
        ct_upload(m.vao, m.vbo, m.ebo,
            std::vector<float>(v, v+64),
            std::vector<unsigned int>(idx, idx+12));
    }

    // ── SEATING ARC: from just after right door edge (105°) going all the way
    //   around to just before left door edge (75°) — a 330° sweep that cleanly
    //   skips the entire entrance zone.
    void _seatArc(Mesh& m, int segs){
        // Gap is at 75°–105° (+Z). Start right after: 108°. End just before: 72°.
        const float startA = 108.0f * 3.14159265f / 180.0f;
        const float endA   =  72.0f * 3.14159265f / 180.0f;
        const float sweep  = (endA > startA) ? (endA - startA) :
                             (6.28318530f - startA + endA);
        std::vector<float> v; std::vector<unsigned int> idx;
        float innerR = 0.8f, outerR = 1.0f; // normalised; scale at draw
        for(int i=0;i<=segs;++i){
            float t = (float)i/segs;
            float a = startA + t * sweep;
            float ca=cosf(a), sa=sinf(a), u=t;
            v.insert(v.end(),{ca*innerR,0,sa*innerR, 0,1,0, u,0.f});
            v.insert(v.end(),{ca*outerR,0,sa*outerR, 0,1,0, u,1.f});
        }
        for(int i=0;i<segs;++i){
            unsigned b0=i*2,b1=i*2+1,b2=(i+1)*2,b3=(i+1)*2+1;
            idx.insert(idx.end(),{b0,b2,b3, b0,b3,b1});
            idx.insert(idx.end(),{b0,b3,b2, b0,b1,b3}); // reverse for interior view
        }
        m.cnt=(int)idx.size();
        ct_upload(m.vao,m.vbo,m.ebo,v,idx);
    }

    // ── FLAT TORUS-RING on the floor ──────────────────────────────────────
    void _circle(Mesh& m, float r, float thickness){
        const float TP=6.28318530f;
        std::vector<float> v; std::vector<unsigned int> idx;
        int segs=40;
        float r0=r-thickness, r1=r+thickness;
        for(int i=0;i<=segs;++i){
            float a=TP*i/segs, u=(float)i/segs;
            v.insert(v.end(),{cosf(a)*r0,0,sinf(a)*r0, 0,1,0, u,0.f});
            v.insert(v.end(),{cosf(a)*r1,0,sinf(a)*r1, 0,1,0, u,1.f});
        }
        for(int i=0;i<segs;++i){
            unsigned b0=i*2,t0=i*2+1,b1=(i+1)*2,t1=(i+1)*2+1;
            idx.insert(idx.end(),{b0,b1,t1, b0,t1,t0});
        }
        m.cnt=(int)idx.size();
        ct_upload(m.vao,m.vbo,m.ebo,v,idx);
    }
};
