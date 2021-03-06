#include "Components.hpp"
#include <glm/ext/matrix_transform.hpp>

using namespace RightEngine;

glm::mat4 TransformComponent::GetLocalTransformMatrix() const
{
    glm::mat4 model(1);
    const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
                                             glm::radians(rotation.x),
                                             glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
                                             glm::radians(rotation.y),
                                             glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
                                             glm::radians(rotation.z),
                                             glm::vec3(0.0f, 0.0f, 1.0f));
    const glm::mat4 rotationMatrix = transformY * transformX * transformZ;

    return glm::translate(glm::mat4(1.0f), position) * rotationMatrix * glm::scale(glm::mat4(1.0f), scale);
}

const glm::mat4& TransformComponent::GetWorldTransformMatrix() const
{
    return transformMatrix;
}

void TransformComponent::RecalculateTransform(TransformComponent& parentTransform)
{
    const auto parentTransformMatrix = glm::translate(glm::mat4(1.0f), parentTransform.GetLocalPosition());
    transformMatrix = parentTransformMatrix * GetLocalTransformMatrix();
}

void TransformComponent::RecalculateTransform()
{
    transformMatrix = GetLocalTransformMatrix();
}

