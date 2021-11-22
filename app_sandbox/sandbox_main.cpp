// dear imgui - Sandbox: experiments, showcase of third party extensions

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "editor_assets_browser.h"
#include "editor_widgets.h"
#include "sandbox_styles.h"
#include "sandbox_tests.h"
#include "imgui_test_engine/thirdparty/Str/Str.h"
#include "thirdparty/imgui_memory_editor/imgui_memory_editor.h"
#include "shared/IconsFontAwesome5.h"

// imgui_app is a helper to wrap multiple Dear ImGui platform/renderer backends
#ifndef IMGUI_APP_IMPLEMENTATION
#define IMGUI_APP_IMPLEMENTATION 1  // Include the backend .cpp files
#endif
#include "shared/imgui_app.h"

#ifdef IMGUI_SANDBOX_ENABLE_NATIVE_FILE_DIALOG
#include "thirdparty/nativefiledialog/nfd.h"
#endif

#ifdef IMGUI_ENABLE_TEST_ENGINE
#include "imgui_test_engine/imgui_te_engine.h"
#include "imgui_test_engine/imgui_te_coroutine.h"
#include "imgui_test_engine/imgui_te_ui.h"
#include "imgui_test_engine/imgui_capture_tool.h"
#include "imgui_test_engine/thirdparty/implot/implot.h"
#endif

void EditorRenderScene();

// FIXME-SANDBOX FIXME-FONT: This is looking very poor with DpiScale == 1.0f, switch to FreeType?
// FIXME-SANDBOX FIXME-FONT: This is looking very poor with DpiScale == 1.0f, switch to FreeType?
static void LoadFonts(ImGuiApp* app)
{
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig font_cfg;
    //font_cfg.GlyphRanges = io.Fonts->GetGlyphRangesJapanese();
    //font_cfg.PixelSnapH = true;
    //font_cfg.SizePixels = 17.0f * app->DpiScale;
    //io.Fonts->AddFontFromFileTTF("../data/fonts/selawk.ttf", 16.0f, &font_cfg);
    //io.Fonts->AddFontFromFileTTF("../data/fonts/selawksb.ttf", 16.0f);

    static const ImWchar icon_fa_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig font_cfg_icons;
    font_cfg_icons.MergeMode = true;
    font_cfg_icons.GlyphMinAdvanceX = 18.0f * app->DpiScale; // Make icon font more monospace looking to simplify alignment
    font_cfg_icons.GlyphRanges = icon_fa_ranges;

    // FIXME-SANDBOX FIXME-FONT: Font size should be rounded?
    // FIXME-SANDBOX FIXME-FONT: FontAwesome5 is rasterized multiple times needlessly!
#if 0
    // Default font
    font_cfg.SizePixels = 13.0f * app->DpiScale;
    io.Fonts->AddFontDefault(&font_cfg);
    font_cfg.SizePixels = 0.0f;
    io.Fonts->AddFontFromFileTTF("../data/fonts/fa5-regular-400.otf", 13.0f * app->DpiScale, &font_cfg_icons);
    io.Fonts->AddFontFromFileTTF("../data/fonts/fa5-solid-900.otf", 13.0f * app->DpiScale, &font_cfg_icons);
#else
    // Font 0
    io.Fonts->AddFontFromFileTTF("../data/fonts/NotoSans-Regular.ttf", 16.0f * app->DpiScale, &font_cfg);
    io.Fonts->AddFontFromFileTTF("../data/fonts/fa5-regular-400.otf", 13.0f * app->DpiScale, &font_cfg_icons);
    io.Fonts->AddFontFromFileTTF("../data/fonts/fa5-solid-900.otf", 13.0f * app->DpiScale, &font_cfg_icons);

    // Font 1
    io.Fonts->AddFontFromFileTTF("../data/fonts/NotoSans-Regular.ttf", 24.0f * app->DpiScale, &font_cfg);
    io.Fonts->AddFontFromFileTTF("../data/fonts/fa5-regular-400.otf", 19.5f * app->DpiScale, &font_cfg_icons);
    io.Fonts->AddFontFromFileTTF("../data/fonts/fa5-solid-900.otf", 19.5f * app->DpiScale, &font_cfg_icons);

    // Font 2 (Mono)
    io.Fonts->AddFontFromFileTTF("../data/fonts/NotoSansMono-Regular.ttf", 16.0f * app->DpiScale, &font_cfg);
    io.Fonts->AddFontFromFileTTF("../data/fonts/fa5-regular-400.otf", 13.0f * app->DpiScale, &font_cfg_icons);
    io.Fonts->AddFontFromFileTTF("../data/fonts/fa5-solid-900.otf", 13.0f * app->DpiScale, &font_cfg_icons);
#endif

    //io.Fonts->AddFontFromFileTTF("../data/fonts/Roboto-Medium.ttf", 16.0f * app->DpiScale, &font_cfg);
    //io.Fonts->AddFontFromFileTTF("../data/fonts/fa5-regular-400.otf", 14.0f * app->DpiScale, &font_cfg_icons);
    //io.Fonts->AddFontFromFileTTF("../data/fonts/fa5-solid-900.otf", 14.0f * app->DpiScale, &font_cfg_icons);
}

int main(int argc, char** argv)
{
    IM_UNUSED(argc);
    IM_UNUSED(argv);

    // Setup application backend
    ImGuiApp* app = ImGuiApp_ImplDefault_Create();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#ifdef IMGUI_HAS_DOCK
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif
#ifdef IMGUI_HAS_VIEWPORT
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif

    // Setup application
    app->DpiAware = true;
    app->SrgbFramebuffer = false;
    app->ClearColor = ImVec4(0.120f, 0.120f, 0.120f, 1.000f);
    app->InitCreateWindow(app, "Dear ImGui: Sandbox", ImVec2(1600, 1000));
    app->InitBackends(app);

#ifdef IMGUI_ENABLE_TEST_ENGINE
    // Initialize test engine
    ImGuiTestEngine* engine = ImGuiTestEngine_CreateContext(ImGui::GetCurrentContext());
    ImGuiTestEngineIO& test_io = ImGuiTestEngine_GetIO(engine);
    test_io.ConfigVerboseLevel = ImGuiTestVerboseLevel_Info;
    test_io.ConfigVerboseLevelOnError = ImGuiTestVerboseLevel_Debug;
    test_io.ScreenCaptureFunc = ImGuiApp_ScreenCaptureFunc;
    test_io.ScreenCaptureUserData = (void*)app;

    // Register tests
    RegisterSandboxTests(engine);
    ImGuiTestEngine_Start(engine);
#endif

    // Setup style, load fonts
    // FIXME-SANDBOX FIXME-DPI: Reload at runtime according to changing DPI.
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark();
    DarkTheme(&style);
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    style.WindowRounding = 0.0f;
    style.ScaleAllSizes(app->DpiScale);
    LoadFonts(app);
    ImFont* font_monospace = ImGui::GetIO().Fonts->Fonts[2]; // FIXME-SANDBOX: store in a structure

    // Load README
#if 0
    Str64 imgui_folder;
    ImFileFindInParents("imgui/", 4, &imgui_folder);
    size_t readme_md_size = 0;
    char* readme_md = (char*)ImFileLoadToMemory(Str128f("%s/docs/README.md", imgui_folder.c_str()).c_str(), "rb", &readme_md_size, +1);
#else
    size_t readme_md_size = 0;
    char* readme_md = (char*)ImFileLoadToMemory("docs/MarkdownTest.md", "rb", &readme_md_size, +1);
#endif

    bool show_imgui_demo = true;
    bool show_dockspace = true;
    bool show_assets_browser = false;
    bool show_markdown = true;
    bool show_memory_editor = false;
    bool show_implot_demo = false;
    bool show_test_engine = false;

    // Spawn a capture tool independent of ImGuiTestEngine's own instance (as you may use capture tool without the test engine)
    bool show_capture_tool = false;
    ImGuiCaptureTool capture_tool;
    capture_tool.Context.ScreenCaptureFunc = ImGuiApp_ScreenCaptureFunc;
    capture_tool.Context.ScreenCaptureUserData = app;

    // Main loop
    bool aborted = false;
    while (!aborted)
    {
        if (!aborted && !app->NewFrame(app))
            aborted = true;
        if (app->Quit)
            aborted = true;

#ifdef IMGUI_ENABLE_TEST_ENGINE
        if (aborted && ImGuiTestEngine_TryAbortEngine(engine))
            break;
#endif

        ImGui::NewFrame();

#ifdef IMGUI_HAS_DOCK
        if (show_dockspace)
            ImGui::DockSpaceOverViewport(NULL, ImGuiDockNodeFlags_PassthruCentralNode);
#endif

        if (show_imgui_demo)
            ImGui::ShowDemoWindow();

        // Contents
        if (ImGui::Begin("Game"))
            EditorRenderScene();
        ImGui::End();

        // Demo/Skeleton editor
        if (ImGui::BeginMainMenuBar())
        {
            // FIXME-SANDBOX: Actual shortcut key handling
            // FIXME-SANDBOX: Input dispatching with multiple levels of focus (e.g. selected window could want to use Ctrl+O)
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                {
                }
                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {
#ifdef IMGUI_SANDBOX_ENABLE_NATIVE_FILE_DIALOG
                    // Testing https://github.com/mlabbe/nativefiledialog
                    // in order to test a long blocking call mid-frame, trying to repro problem discussed in #3575 (currently cannot repro)
                    nfdchar_t* outPath = NULL;
                    nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);
                    if (result == NFD_OKAY)
                        free(outPath);
#endif
                }
                ImGui::Separator();
                if (ImGui::BeginMenu("Options"))
                {
#ifdef IMGUI_ENABLE_TEST_ENGINE
                    bool vsync = !ImGuiTestEngine_GetIO(engine).ConfigNoThrottle;
                    if (ImGui::MenuItem("Throttle/Vsync", NULL, &vsync))
                        ImGuiTestEngine_GetIO(engine).ConfigNoThrottle = !vsync;
#else
                    ImGui::MenuItem("Throttle/Vsync", NULL, &app->Vsync);
#endif
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Ctrl+W"))
                    app->Quit = true;
                ImGui::EndMenu();
            }

            // FIXME: The MenuItem alignment system could provide a left-most slot for icons? See TODO
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem(ICON_FA_UNDO " Undo", "Ctrl+Z"))
                {
                }
                if (ImGui::MenuItem(ICON_FA_REDO" Redo", "Ctrl+Y"))
                {
                }
                ImGui::Separator();
                if (ImGui::MenuItem(ICON_FA_CUT " Cut", "Ctrl+X"))
                {
                }
                if (ImGui::MenuItem(ICON_FA_COPY " Copy", "Ctrl+C"))
                {
                }
                if (ImGui::MenuItem(ICON_FA_PASTE " Paste", "Ctrl+V"))
                {
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                // Demo
                ImGui::MenuItem("Demo: Dear ImGui Demo", NULL, &show_imgui_demo);
#ifdef IMGUI_HAS_DOCK
                ImGui::MenuItem("Demo: Dockspace", NULL, &show_dockspace);
#else
                ImGui::MenuItem("Demo: Dockspace", NULL, &show_dockspace, false);
#endif
                ImGui::MenuItem("Demo: Assets Browser", NULL, &show_assets_browser);
                ImGui::MenuItem("Demo: Markdown", NULL, &show_markdown);
                ImGui::MenuItem("Demo: Memory Editor", NULL, &show_memory_editor);
                ImGui::MenuItem("Demo: ImPlot", NULL, &show_implot_demo);
                ImGui::Separator();

                // Tools
                ImGui::MenuItem("Capture Tool", NULL, &show_capture_tool);
#ifdef IMGUI_ENABLE_TEST_ENGINE
                ImGui::MenuItem("Test Engine", NULL, &show_test_engine);
#else
                ImGui::MenuItem("Test Engine", NULL, &show_test_engine, false);
#endif
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                ImGui::MenuItem("About Sandbox", "", false, false);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (show_assets_browser)
            ShowExampleAppAssetBrowser(&show_assets_browser);

        if (show_markdown)
        {
            ImGui::Begin("Markdown", &show_markdown);
            if (readme_md)
                RenderMarkdown(readme_md, readme_md + readme_md_size);
            ImGui::End();
        }

        if (show_memory_editor)
        {
            static MemoryEditor me;
            static char buf[0x10000];
            ImGui::PushFont(font_monospace);
            me.DrawWindow("Memory Editor", buf, sizeof(buf));
            ImGui::PopFont();
        }

        if (show_capture_tool)
            capture_tool.ShowCaptureToolWindow(&show_capture_tool);

#ifdef IMGUI_ENABLE_TEST_ENGINE
        if (show_test_engine)
            ImGuiTestEngine_ShowTestWindows(engine, &show_test_engine);
        app->Vsync = test_io.RenderWantMaxSpeed ? false : true;
#endif

        if (show_implot_demo)
            ImPlot::ShowDemoWindow(NULL);

        ImGui::Render();

        app->Render(app);

#ifdef IMGUI_ENABLE_TEST_ENGINE
        ImGuiTestEngine_PostSwap(engine);
#endif
    }

    // Shutdown
#ifdef IMGUI_ENABLE_TEST_ENGINE
    ImGuiTestEngine_Stop(engine);
#endif
    app->ShutdownBackends(app);
    app->ShutdownCloseWindow(app);
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
#ifdef IMGUI_ENABLE_TEST_ENGINE
    ImGuiTestEngine_ShutdownContext(engine);
#endif
    app->Destroy(app);

    return 0;
}