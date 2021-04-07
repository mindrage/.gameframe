#pragma once

#include "imgui.h"

struct SDL_Window;

IMGUI_IMPL_API void ImGui_ImplBgfx_Init(int view);
IMGUI_IMPL_API void ImGui_ImplBgfx_Shutdown();
IMGUI_IMPL_API void ImGui_ImplBgfx_NewFrame();
IMGUI_IMPL_API void ImGui_ImplBgfx_RenderDrawData(ImDrawData* draw_data);

IMGUI_IMPL_API void ImGui_ImplBgfx_InvalidateDeviceObjects();
IMGUI_IMPL_API bool ImGui_ImplBgfx_CreateDeviceObjects();