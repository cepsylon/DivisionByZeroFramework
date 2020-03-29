#version 450 core

layout(location = 0) in vec3 iaPosition;
layout(location = 1) in vec4 iaColor;

layout(set = 0, binding = 0) uniform UniformBufferObject
{
  mat4 model;
} ubo;

layout(location = 0) out vec4 vColor;

void main()
{
  vColor = iaColor;
  gl_Position = ubo.model * vec4(iaPosition, 1.0f);
}