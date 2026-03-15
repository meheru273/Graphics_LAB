#ifndef pointLight_h
#define pointLight_h

#include <iostream>
#include <stdlib.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"
using namespace std;

class PointLight {
public:
    glm::vec3 position;

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;

    float Kc;
    float Kl;
    float Kq;
    int lightNumber;

    PointLight(glm::vec3 pos, glm::vec4 amb, glm::vec4 diff, glm::vec4 spec, float constant, float linear, float quadratic, int num)
    {
        position = pos;
        ambient = amb;
        diffuse = diff;
        specular = spec;
        Kc = constant;
        Kl = linear;
        Kq = quadratic;
        lightNumber = num;
    }

    void setUpLight(Shader& lightingShader)
    {
        string temp = "pointLights[";
        temp += to_string(lightNumber - 1);
        temp += "]";

        lightingShader.use();
        lightingShader.setVec4(temp + ".ambient", ambient * ambientOn * isOn);
        lightingShader.setVec4(temp + ".diffuse", diffuse * diffuseOn * isOn);
        lightingShader.setVec4(temp + ".specular", specular * specularOn * isOn);
        lightingShader.setVec3(temp + ".position", position);
        lightingShader.setFloat(temp + ".Kc", Kc);
        lightingShader.setFloat(temp + ".Kl", Kl);
        lightingShader.setFloat(temp + ".Kq", Kq);
    }

    void turnOff()
    {
        isOn = 0.0;
    }
    void turnOn()
    {
        isOn = 1.0;
    }
    void turnAmbientOn()
    {
        ambientOn = 1.0;
    }
    void turnAmbientOff()
    {
        ambientOn = 0.0;
    }
    void turnDiffuseOn()
    {
        diffuseOn = 1.0;
    }
    void turnDiffuseOff()
    {
        diffuseOn = 0.0;
    }
    void turnSpecularOn()
    {
        specularOn = 1.0;
    }
    void turnSpecularOff()
    {
        specularOn = 0.0;
    }

private:
    float ambientOn = 1.0;
    float diffuseOn = 1.0;
    float specularOn = 1.0;
    float isOn = 1.0;
};

#endif /* pointLight_h */