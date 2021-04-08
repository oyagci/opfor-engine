#pragma once
#include "EditorWindow.hpp"
#include "imgui.h"
#include <opfor/core/base.hpp>

#include <queue>

class EditorLog : public IEditorWindow
{
  private:
    bool _autoScroll = true;

    // XXX: Using an std::deque to store line here might slow down rendering if the logger UI is refreshed too often
    std::deque<opfor::String> _history_lines;

    void OnLog(Logger::LogLevel, opfor::StringView const data)
    {
        auto last_line = 0u;
        for (auto i = 0u; i < data.size(); ++i)
        {
            if (data[i] == '\n')
            {
                if (_history_lines.size() >= MAX_HISTORY_LINES)
                {
                    _history_lines.pop_front();
                }
                _history_lines.push_back({data.data() + last_line, data.data() + i});
                last_line = i + 1;
            }
        }
    }

  public:
    static constexpr auto MAX_HISTORY_LINES = 1000u;

    EditorLog()
    {
        using namespace std::placeholders;
        Logger::BindCallback("editor", std::bind(&EditorLog::OnLog, this, _1, _2));
    }

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

            const auto current_severity = Logger::GetLogLevel();
            const auto current_severity_str = fmt::format("{}", current_severity);
            if (ImGui::BeginCombo("Severity", current_severity_str.c_str()))
            {
                for (auto i = 0u; i < 5; ++i)
                {
                    auto const severity = static_cast<Logger::LogLevel>(i);
                    auto const severity_str = fmt::format("{}", severity);
                    const bool is_selected = severity == current_severity;
                    if (ImGui::Selectable(severity_str.c_str(), is_selected))
                    {
                        Logger::SetLogLevel(severity);
                    }

                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::EndPopup();
        }

        if (ImGui::Button("Options"))
        {
            ImGui::OpenPopup("Options");
        }
        ImGui::SameLine();
        bool const clear = ImGui::Button("Clear");
        if (clear)
        {
            _history_lines.clear();
        }

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(_history_lines.size()));
        while (clipper.Step())
        {
            for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
            {
                auto const &line = _history_lines[line_no];
                ImGui::TextUnformatted(line.data(), line.data() + line.size());
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
