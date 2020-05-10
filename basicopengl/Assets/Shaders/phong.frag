#version 450
// Copyright(c) 2017 Packt
// MIT License (Check Licenses/Licenses.mit for full text)

in vec3 LightIntensity;
layout( location = 0 ) out vec4 FragColor;

void main() {
    FragColor = vec4(LightIntensity, 1.0);
}