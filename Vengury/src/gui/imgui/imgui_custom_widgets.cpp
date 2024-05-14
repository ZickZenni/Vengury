#include "imgui_custom_widgets.hpp"

namespace ImGuiWidgets {
    float       SeparatorTextBorderSize = 2;
    ImVec2      SeparatorTextAlign = ImVec2(0.15f, 0.5f);
    ImVec2      SeparatorTextPadding = ImVec2(4, 4);

	void SeparatorText(const char* label)
	{
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return;

        // The SeparatorText() vs SeparatorTextEx() distinction is designed to be considerate that we may want:
        // - allow separator-text to be draggable items (would require a stable ID + a noticeable highlight)
        // - this high-level entry point to allow formatting? (which in turns may require ID separate from formatted string)
        // - because of this we probably can't turn 'const char* label' into 'const char* fmt, ...'
        // Otherwise, we can decide that users wanting to drag this would layout a dedicated drag-item,
        // and then we can turn this into a format function.
        SeparatorTextEx(0, label, ImGui::FindRenderedTextEnd(label), 0.0f);
	}

    void SeparatorTextEx(ImGuiID id, const char* label, const char* label_end, float extra_w)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImGuiStyle& style = g.Style;

        const ImVec2 label_size = ImGui::CalcTextSize(label, label_end, false);
        const ImVec2 pos = window->DC.CursorPos;
        const ImVec2 padding = SeparatorTextPadding;

        const float separator_thickness = SeparatorTextBorderSize;
        const ImVec2 min_size(label_size.x + extra_w + padding.x * 2.0f, ImMax(label_size.y + padding.y * 2.0f, separator_thickness));
        const ImRect bb(pos, ImVec2(window->WorkRect.Max.x, pos.y + min_size.y));
        const float text_baseline_y = ImTrunc((bb.GetHeight() - label_size.y) * SeparatorTextAlign.y + 0.99999f); //ImMax(padding.y, ImFloor((style.SeparatorTextSize - label_size.y) * 0.5f));
        ImGui::ItemSize(min_size, text_baseline_y);
        if (!ImGui::ItemAdd(bb, id))
            return;

        const float sep1_x1 = pos.x;
        const float sep2_x2 = bb.Max.x;
        const float seps_y = ImTrunc((bb.Min.y + bb.Max.y) * 0.5f + 0.99999f);

        const float label_avail_w = ImMax(0.0f, sep2_x2 - sep1_x1 - padding.x * 2.0f);
        const ImVec2 label_pos(pos.x + padding.x + ImMax(0.0f, (label_avail_w - label_size.x - extra_w) * SeparatorTextAlign.x), pos.y + text_baseline_y); // FIXME-ALIGN

        // This allows using SameLine() to position something in the 'extra_w'
        window->DC.CursorPosPrevLine.x = label_pos.x + label_size.x;

        const ImU32 separator_col = ImGui::GetColorU32(ImGuiCol_Separator);
        if (label_size.x > 0.0f)
        {
            const float sep1_x2 = label_pos.x - style.ItemSpacing.x;
            const float sep2_x1 = label_pos.x + label_size.x + extra_w + style.ItemSpacing.x;
            if (sep1_x2 > sep1_x1 && separator_thickness > 0.0f)
                window->DrawList->AddLine(ImVec2(sep1_x1, seps_y), ImVec2(sep1_x2, seps_y), separator_col, separator_thickness);
            if (sep2_x2 > sep2_x1 && separator_thickness > 0.0f)
                window->DrawList->AddLine(ImVec2(sep2_x1, seps_y), ImVec2(sep2_x2, seps_y), separator_col, separator_thickness);
            if (g.LogEnabled)
                ImGui::LogSetNextTextDecoration("---", NULL);
            ImGui::RenderTextEllipsis(window->DrawList, label_pos, ImVec2(bb.Max.x, bb.Max.y + style.ItemSpacing.y), bb.Max.x, bb.Max.x, label, label_end, &label_size);
        }
        else
        {
            if (g.LogEnabled)
                ImGui::LogText("---");
            if (separator_thickness > 0.0f)
                window->DrawList->AddLine(ImVec2(sep1_x1, seps_y), ImVec2(sep2_x2, seps_y), separator_col, separator_thickness);
        }
    }
    ImVec2 ImMax(const ImVec2& lhs, const ImVec2& rhs)
    {
        return ImVec2(lhs.x >= rhs.x ? lhs.x : rhs.x, lhs.y >= rhs.y ? lhs.y : rhs.y);
    }
    float ImTrunc(float f)
    {
        return (float)(int)(f);
    }
    ImVec2 ImTrunc(const ImVec2& v)
    {
        return ImVec2((float)(int)(v.x), (float)(int)(v.y));
    }
}
