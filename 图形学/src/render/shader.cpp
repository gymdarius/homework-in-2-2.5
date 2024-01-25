#include "shader.h"
#include "../utils/math.hpp"
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#ifdef _WIN32
#undef min
#undef max
#endif

using Eigen::Vector3f;
using Eigen::Vector4f;

// vertex shader & fragement shader can visit
// all the static variables below from Uniforms structure
Eigen::Matrix4f Uniforms::MVP;
Eigen::Matrix4f Uniforms::inv_trans_M;
int Uniforms::width;
int Uniforms::height;

// vertex shader
VertexShaderPayload vertex_shader(const VertexShaderPayload& payload)
{
    VertexShaderPayload output_payload = payload;
    // Vertex position transformation
    //����λ�ñ任 
    output_payload.position = Uniforms::MVP * output_payload.position;
    // Viewport transformation
    //�ӿڱ任
    output_payload.position[0] /= output_payload.position[3];
    output_payload.position[1] /= output_payload.position[3];
    output_payload.position[2] /= output_payload.position[3];
    
    output_payload.position[0] = (output_payload.position[0] + 1.0) * Uniforms::width / 2;
    output_payload.position[1] = (output_payload.position[1] + 1.0) * Uniforms::height / 2;
    
    output_payload.position[2] = output_payload.position[3] * 25 + 25;

    // Vertex normal transformation
    //���㷨�߱任
    Vector4f Tnormal = { output_payload.normal[0], output_payload.normal[1],
                             output_payload.normal[2], 0 };
    Tnormal = Uniforms::inv_trans_M * Tnormal;
    output_payload.normal = { Tnormal[0], Tnormal[1], Tnormal[2] };

    return output_payload;
}

Vector3f phong_fragment_shader(const FragmentShaderPayload& payload, GL::Material material,
                               const std::list<Light>& lights, Camera camera)
{
    //std::list<Light>& light = lights;
    Vector3f result = { 0, 0, 0 };
    // ��ȡ���ʵĻ����⡢������;��淴��ϵ��
    Vector3f ka = material.ambient;
    Vector3f kd = material.diffuse;
    Vector3f ks = material.specular;
    // ���û�����ǿ��
    Vector3f ambient_intensity = ka;
    // ѭ������ÿ����Դ

    if (lights.empty()) {
        return result;
    }
    // �������߷���
    Vector3f view_direction = payload.world_pos - camera.position;
    view_direction = view_direction.normalized();
    for (Light L : lights) {
        // ������߷���
        Vector3f light_direction = L.position - payload.world_pos;
        light_direction = light_direction.normalized();
        // ���������
        Vector3f half_vector = light_direction + view_direction;
        //float length2= std::sqrt(half_vector[0] * half_vector[0] + half_vector[1] * half_vector[1] + half_vector[2] * half_vector[2]);
        //half_vector *= 1.0f / length2;
        half_vector = half_vector.normalized();
        // �������˥��
        float distance = (L.position - payload.world_pos).norm();
        float attenuation = L.intensity / (distance * distance);
        // ����������
        float diffuse_factor = std::max(0.0f, payload.world_normal.dot(light_direction));
        Vector3f diffuse_intensity = kd * diffuse_factor * attenuation;
        // ���㾵�淴��
        float specular_factor = pow(std::max(0.0f, payload.world_normal.dot(half_vector)), material.shininess);
        Vector3f specular_intensity = ks * specular_factor * attenuation;
        // �������⡢������;��淴����ӵ������
        result += ambient_intensity + diffuse_intensity + specular_intensity;
    }
    // set rendering result max threshold to 255
    result = result * 255.f;
    result = result.cwiseMin(Vector3f(255, 255, 255)).cwiseMax(Vector3f(0, 0, 0));
    return result;
}
