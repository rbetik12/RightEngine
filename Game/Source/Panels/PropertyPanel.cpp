#include "PropertyPanel.hpp"
#include "Components.hpp"
#include <imgui_internal.h>
#include <imgui.h>

using namespace RightEngine;

namespace
{
    void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();
    }
}

PropertyPanel::PropertyPanel(const std::shared_ptr<Scene>& aScene)
{
    scene = aScene;
}

void PropertyPanel::SetScene(const std::shared_ptr<Scene>& aScene)
{
    scene = aScene;
}

void PropertyPanel::SetSelectedEntity(const std::shared_ptr<Entity>& entity)
{
    selectedEntity = entity;
}

template<class T>
void DrawComponent(const std::string& componentName, const std::shared_ptr<Entity>& entity,
                   std::function<void(T&)> uiFunction)
{
    const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
                                             | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap
                                             | ImGuiTreeNodeFlags_FramePadding;
    if (entity->HasComponent<T>())
    {
        auto& component = entity->GetComponent<T>();
        bool open = ImGui::TreeNodeEx((void*) typeid(T).hash_code(), treeNodeFlags, componentName.c_str());

        if (open)
        {
            uiFunction(component);
            ImGui::TreePop();
        }
    }
}

void PropertyPanel::OnImGuiRender()
{
    ImGui::Begin("Properties");
    if (selectedEntity)
    {
        DrawComponent<Tag>("Tag", selectedEntity, [](auto& component)
        {
            const size_t bufSize = 256;
            char buf[bufSize];
            R_CORE_ASSERT(component.name.size() < bufSize, "");
            snprintf(buf, bufSize, component.name.c_str());
            ImGui::InputText("Entity Name", buf, bufSize);
            //TODO: Change name only if buf was changed
            component.name = std::string(buf);
            ImGui::Separator();
            ImGui::LabelText("Entity ID", "%d", component.id);
        });

        DrawComponent<Transform>("Transform", selectedEntity, [](auto& component)
        {
            auto& position = component.GetLocalPosition();
            DrawVec3Control("Position", position);
            auto& rotation = component.GetRotation();
            DrawVec3Control("Rotation", rotation);
            auto& scale = component.GetScale();
            DrawVec3Control("Scale", scale);
        });
    }
    ImGui::End();
}
