#pragma once
#include "EditorWindow.hpp"
#include <opfor/core/Logger.hpp>
#include "imgui.h"

class EditorLog : public IEditorWindow
{
  private:
    bool _autoScroll = true;

  public:
    void OnDrawGUI() override
    {
        if (!ImGui::Begin("Log"))
        {
            ImGui::End();
            return;
        }

        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-Scroll", &_autoScroll);
            ImGui::EndPopup();
        }

        if (ImGui::Button("Options"))
        {
            ImGui::OpenPopup("Options");
        }
        ImGui::SameLine();
        bool clear = ImGui::Button("Clear");
        if (clear)
        {
            Logger::Clear();
        }

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        const auto &logs = Logger::GetLog();
        const auto &lineOffsets = Logger::GetLineOffsets();

        const char *bufp = logs.data();
        const char *bufendp = logs.data() + logs.size();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(lineOffsets.size()));
        while (clipper.Step())
        {
            for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
            {
                const char *line_start = bufp + lineOffsets[line_no];
                const char *line_end = (line_no + 1 < static_cast<int>(lineOffsets.size()))
                                           ? (bufp + lineOffsets[line_no + 1] - 1)
                                           : bufendp;
                ImGui::TextUnformatted(line_start, line_end);
            }
        }
        clipper.End();

        ImGui::PopStyleVar();

        if (_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();

        ImGui::End();
    }
};
