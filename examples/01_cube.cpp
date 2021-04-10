#include <iostream>
#include <fstream>

#include <SDL.h>
#include <SDL_syswm.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>

#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <imgui_impl_bgfx.h>

#include <Tracy.hpp>

// if we're compiling for iOS (iPhone/iPad)
#ifdef __IPHONEOS__
#include <SDL_opengles.h> // we want to use OpenGL ES
#else
#include <SDL_opengl.h> // otherwise we want to use OpenGL
#endif

namespace fileops
{

  inline static std::streamoff stream_size(std::istream &file)
  {
    std::istream::pos_type current_pos = file.tellg();
    if (current_pos == std::istream::pos_type(-1))
    {
      return -1;
    }
    file.seekg(0, std::istream::end);
    std::istream::pos_type end_pos = file.tellg();
    file.seekg(current_pos);
    return end_pos - current_pos;
  }

  inline bool stream_read_string(std::istream &file, std::string &fileContents)
  {
    std::streamoff len = stream_size(file);
    if (len == -1)
    {
      return false;
    }

    fileContents.resize(static_cast<std::string::size_type>(len));

    file.read(fileContents.data(), fileContents.length());
    return true;
  }

  inline bool read_file(const std::string &filename, std::string &fileContents)
  {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open())
    {
      return false;
    }

    const bool success = stream_read_string(file, fileContents);

    file.close();

    return success;
  }

} // namespace fileops

struct PosColorVertex
{
  float x;
  float y;
  float z;
  uint32_t abgr;
};

static PosColorVertex cube_vertices[] = {
    {-1.0f, 1.0f, 1.0f, 0xff000000},
    {1.0f, 1.0f, 1.0f, 0xff0000ff},
    {-1.0f, -1.0f, 1.0f, 0xff00ff00},
    {1.0f, -1.0f, 1.0f, 0xff00ffff},
    {-1.0f, 1.0f, -1.0f, 0xffff0000},
    {1.0f, 1.0f, -1.0f, 0xffff00ff},
    {-1.0f, -1.0f, -1.0f, 0xffffff00},
    {1.0f, -1.0f, -1.0f, 0xffffffff},
};

static const uint16_t cube_tri_list[] = {
    0,
    1,
    2,
    1,
    3,
    2,
    4,
    6,
    5,
    5,
    6,
    7,
    0,
    2,
    4,
    4,
    2,
    6,
    1,
    5,
    3,
    5,
    7,
    3,
    0,
    4,
    1,
    4,
    5,
    1,
    2,
    3,
    6,
    6,
    3,
    7,
};

static bgfx::ShaderHandle createShader(
    const std::string &shader, const char *name)
{
  const bgfx::Memory *mem = bgfx::copy(shader.data(), shader.size());
  const bgfx::ShaderHandle handle = bgfx::createShader(mem);
  bgfx::setName(handle, name);
  return handle;
}

int main(int argc, char *argv[])
{
  // Initialize SDL with video
  if (auto result = SDL_Init(SDL_INIT_VIDEO); result < 0)
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load video (%i): %s", result, SDL_GetError());
    return result;
  }

  int width = 640, height = 480;

  // Create an SDL window
  SDL_Window *window = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  if (window == nullptr)
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s", SDL_GetError());
    return 1;
  }

  SDL_SysWMinfo wmi;
  SDL_VERSION(&wmi.version);

  if (!SDL_GetWindowWMInfo(window, &wmi))
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load window info: %s", SDL_GetError());
    return 1;
  }

  //bgfx::renderFrame();

  bgfx::PlatformData pd{};
#if BX_PLATFORM_WINDOWS
  pd.nwh = wmi.info.win.window;
#elif BX_PLATFORM_OSX
  pd.nwh = wmi.info.cocoa.window;
#endif // BX_PLATFORM_WINDOWS ? BX_PLATFORM_OSX

  bgfx::Init bgfx_init;
  bgfx_init.type = bgfx::RendererType::Vulkan; // auto choose renderer
  bgfx_init.resolution.width = width;
  bgfx_init.resolution.height = height;
  bgfx_init.resolution.reset = BGFX_RESET_VSYNC;
  bgfx_init.platformData = pd;
  bgfx_init.resolution.numBackBuffers = 1;
  bgfx::init(bgfx_init);

  bgfx::setViewClear(
      0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x6495EDFF, 1.0f, 0);
  bgfx::setViewRect(0, 0, 0, width, height);

  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui::StyleColorsDark();

  ImGuiStyle &style = ImGui::GetStyle();
  style.WindowRounding = 0.0f;
  style.Colors[ImGuiCol_WindowBg].w = 1.0f;

  ImGui_ImplBgfx_Init(255);

#if BX_PLATFORM_WINDOWS
  ImGui_ImplSDL2_InitForD3D(window);
#elif BX_PLATFORM_OSX
  ImGui_ImplSDL2_InitForMetal(window);
#endif // BX_PLATFORM_WINDOWS ? BX_PLATFORM_OSX

  bgfx::VertexLayout pos_col_vert_layout;
  pos_col_vert_layout.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
      .end();
  bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
      bgfx::makeRef(cube_vertices, sizeof(cube_vertices)),
      pos_col_vert_layout);
  bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
      bgfx::makeRef(cube_tri_list, sizeof(cube_tri_list)));

  std::string vshader;
  if (!fileops::read_file("shaders/spirv/vs_cubes.bin", vshader))
  {
    return 1;
  }

  std::string fshader;
  if (!fileops::read_file("shaders/spirv/fs_cubes.bin", fshader))
  {
    return 1;
  }

  bgfx::ShaderHandle vsh = createShader(vshader, "vshader");
  bgfx::ShaderHandle fsh = createShader(fshader, "fshader");

  bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

  float cam_pitch = 0.0f;
  float cam_yaw = 0.0f;
  float rot_scale = 0.01f;

  int prev_mouse_x = 0;
  int prev_mouse_y = 0;

  for (bool quit = false; !quit;)
  {
    SDL_Event currentEvent;
    while (SDL_PollEvent(&currentEvent) != 0)
    {
      ImGui_ImplSDL2_ProcessEvent(&currentEvent);

      if (currentEvent.type == SDL_WINDOWEVENT)
      {
        if (currentEvent.window.event == SDL_WINDOWEVENT_RESIZED)
        {
          width = currentEvent.window.data1;
          height = currentEvent.window.data2;
          bgfx::reset(width, height, BGFX_RESET_VSYNC);
          bgfx::setViewClear(
          0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x6495EDFF, 1.0f, 0);
          bgfx::setViewRect(0, 0, 0, width, height);
        }
      }

      if (currentEvent.type == SDL_QUIT)
      {
        quit = true;
        break;
      }
    }

    ImGui_ImplSDL2_NewFrame(window);
    ImGui_ImplBgfx_NewFrame();

    ImGui::NewFrame();
    ImGui::ShowDemoWindow();

    if (ImGui::Begin("Frame buffer values"))
    {
      ImGui::LabelText("Width, Height", "%i, %i", width, height);
      ImGui::LabelText("Buffer Size", "%f, %f", io.DisplaySize.x, io.DisplaySize.y);
      ImGui::LabelText("Framebuffer scaling", "%f, %f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    }

    ImGui::Render();

    ImGui_ImplBgfx_RenderDrawData(ImGui::GetDrawData());

    // simple input code for orbit camera
    int mouse_x, mouse_y;
    const int buttons = SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
    if ((buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0)
    {
      int delta_x = mouse_x - prev_mouse_x;
      int delta_y = mouse_y - prev_mouse_y;

      cam_yaw += float(-delta_x) * rot_scale;
      cam_pitch += float(-delta_y) * rot_scale;
    }

    prev_mouse_x = mouse_x;
    prev_mouse_y = mouse_y;

    float cam_rotation[16];
    bx::mtxRotateXYZ(cam_rotation, cam_pitch, cam_yaw, 0.0f);

    float cam_translation[16];
    bx::mtxTranslate(cam_translation, 0.0f, 0.0f, -5.0f);

    float cam_transform[16];
    bx::mtxMul(cam_transform, cam_translation, cam_rotation);

    float view[16];
    bx::mtxInverse(view, cam_transform);

    float proj[16];
    bx::mtxProj(
        proj, 60.0f, float(width) / float(height), 0.1f, 100.0f,
        bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewTransform(0, view, proj);

    float model[16];
    bx::mtxIdentity(model);
    bgfx::setTransform(model);

    bgfx::setVertexBuffer(0, vbh);
    bgfx::setIndexBuffer(ibh);

    bgfx::submit(0, program);

    bgfx::frame();
  }

  bgfx::destroy(vbh);
  bgfx::destroy(ibh);
  bgfx::destroy(program);

  ImGui_ImplSDL2_Shutdown();
  ImGui_ImplBgfx_Shutdown();

  ImGui::DestroyContext();
  bgfx::shutdown();

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}