//
// Created by Administrator on 2025/5/7.
//

#include <stdexcept>

#include "CGui.h"
#include "CApplication.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

#include "IconsFontAwesome6.h"

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <imgui_impl_glfw.h>
#include <iomanip>
#include <iostream>
#include <bits/stl_algo.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <fstream>
#include <sstream>

#include "default_style.h"

const std::string CGui::window_title = "单维度有容量约束的最近停靠分配";

CGui::CGui() : hue_cycler(600)
{
    inited = false;
    quit = false;
}

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void CGui::setup()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char *glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char *glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    glfw_window = glfwCreateWindow(1280, 720, window_title.data(), nullptr, nullptr);
    if (glfw_window == nullptr)
        return;

    glfwMakeContextCurrent(glfw_window);
    glfwSwapInterval(1); // Enable vsync

    // setup bg
    background.load_png("bg.png");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    SetupImGuiStyle(THEME_MIKU);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    load_config();
    

    // 字符范围
    auto chinese_range = io.Fonts->GetGlyphRangesChineseFull();
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    // 1. 加载主字体（中文或英文）作为 base 字体
    regularFont = io.Fonts->AddFontFromFileTTF("font.ttf", font_size, nullptr, chinese_range);

    // 2. 设置合并模式以合并图标字体
    ImFontConfig icon_config;
    icon_config.MergeMode = true;           // ✅ 关键！合并字体
    icon_config.PixelSnapH = true;          // 建议开启

    // 3. 合并 Font Awesome 字体
    io.Fonts->AddFontFromFileTTF("fa-solid-900.ttf", font_size, &icon_config, icon_ranges);

    // 可选：手动构建字体图集（ImGui 通常会自动做）
    io.Fonts->Build();


    inited = true;
}

void CGui::shutdown()
{
    if (!inited)
        return;

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(glfw_window);
    glfwTerminate();
}

void CGui::render()
{
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGuiIO &io = ImGui::GetIO();

    if (glfwWindowShouldClose(glfw_window))
    {
        quit = true;
        return;
    }
    glfwPollEvents();
    if (glfwGetWindowAttrib(glfw_window, GLFW_ICONIFIED) != 0)
    {
        ImGui_ImplGlfw_Sleep(10);
    }
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static float f = 0.0f;
    static int counter = 0;
    // 全屏窗口
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
    ImGui::SetNextWindowBgAlpha(1.f); // 设置背景半透明
    ImGui::Begin("main", 0,
                 ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse);
    background.draw();
    renderMain();
    ImGui::End();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(glfw_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
                 clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(glfw_window);
}

bool CGui::isQuitEventTriggered()
{
    return quit;
}

void CGui::renderMain()
{

    static float leftWidth = 300.0f;
    const float splitterWidth = 6.0f;
    const float minPanelWidth = 100.0f;
    const float styleItemSpacing = ImGui::GetStyle().ItemSpacing.y;
    const float headerHeight = ImGui::GetFontSize() * 2 + styleItemSpacing * 5;

    ImGui::BeginChild("Horizontal Split View");

    float contentWidth = ImGui::GetContentRegionAvail().x;
    float contentHeight = ImGui::GetContentRegionAvail().y;

    // —— 左侧面板，无边框 —— //
    ImGui::BeginChild("LeftPanel", ImVec2(leftWidth, contentHeight), true);

    // 1) 固定头部（无边框）
    ImGui::BeginChild("LeftHeader", ImVec2(0, headerHeight), false);

    ImGui::Text("测试点");
    ImGui::SameLine();
    if (ImGui::Button("新增测试点"))
    {
        generate_testcases();
    }
    ImGui::SameLine();
    if (ImGui::Button("运行所有测试点"))
    {
        for (int i = 0; i < (int)testcases_input.size(); ++i)
            submitTestcase(i);
    }
    if (ImGui::Button("导入data.txt多组样例"))
    {
        import_test_cases();
    }
    ImGui::Separator();
    ImGui::EndChild(); // LeftHeader

    // 2) 滚动列表区（无边框）
    float listHeight = contentHeight - headerHeight - styleItemSpacing - ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("LeftList", ImVec2(0, listHeight), false, ImGuiWindowFlags_None);

    hue_cycler += hue_speed;
    auto cyc = hue_cycler;

    std::vector<ColorHolder> cols(testcases_input.size());
    for (int i = 0; i < (int)testcases_input.size(); i++)
    {
        cyc += 10;
        cols[i] = cyc.currentRgb();
    }
    if (!hue_reversed)
    {
        std::reverse(cols.begin(), cols.end());
    }

    for (int i = 0; i < (int)testcases_input.size(); i++)
    {
        std::stringstream ss;
        ss << "测试点" << '#' << std::setw(3) << std::setfill('0') << i;
        auto &rgb = cols[i];
        ImVec4 headerColor = ImVec4(rgb.r, rgb.g, rgb.b, 1.0f);                            // 正常状态
        ImVec4 headerHoverColor = ImVec4(rgb.r * 1.1f, rgb.g * 1.1f, rgb.b * 1.1f, 1.0f);  // 悬浮时稍亮
        ImVec4 headerActiveColor = ImVec4(rgb.r * 0.9f, rgb.g * 0.9f, rgb.b * 0.9f, 1.0f); // 按下时稍暗
        ImGui::PushStyleColor(ImGuiCol_Header, headerColor);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, headerHoverColor);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, headerActiveColor);
        if (ImGui::CollapsingHeader(ss.str().c_str()))
        {
            float availW = ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x * 2;
            ImGui::Text("无人机位置");
            ImGui::InputTextMultiline(
                (std::string("##无人机位置") + std::to_string(i)).c_str(),
                &testcases_input[i].a,
                ImVec2(availW, 60));
            ImGui::Text("站点位置");
            ImGui::InputTextMultiline(
                (std::string("##站点位置") + std::to_string(i)).c_str(),
                &testcases_input[i].b,
                ImVec2(availW, 60));
            ImGui::Text("各站点容量");
            ImGui::InputTextMultiline(
                (std::string("##各站点容量") + std::to_string(i)).c_str(),
                &testcases_input[i].c,
                ImVec2(availW, 60));
            if (ImGui::Button((std::string("异步运行##") + std::to_string(i)).c_str()))
            {
                submitTestcase(i);
            }
        }
        ImGui::PopStyleColor(3);
    }
    ImGui::EndChild(); // LeftList
    ImGui::EndChild(); // LeftPanel

    // —— 分割条 —— //
    ImGui::SameLine();
    ImVec2 splitterStart = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("Splitter", ImVec2(splitterWidth, contentHeight));
    if (ImGui::IsItemActive())
    {
        leftWidth += ImGui::GetIO().MouseDelta.x;
        leftWidth = std::clamp(leftWidth, minPanelWidth, contentWidth - minPanelWidth);
    }
    ImVec2 splitterEnd = ImVec2(
        splitterStart.x + splitterWidth,
        splitterStart.y + contentHeight);
    ImU32 col = ImGui::IsItemActive()
                    ? IM_COL32(180, 180, 180, 255)
                : ImGui::IsItemHovered()
                    ? IM_COL32(140, 140, 140, 255)
                    : IM_COL32(100, 100, 100, 255);
    ImGui::GetWindowDrawList()->AddRectFilled(splitterStart, splitterEnd, col);

    // —— 右侧面板：已完成任务展示 —— //
    ImGui::SameLine();
    ImGui::BeginChild("RightPanel",
                      ImVec2(0, contentHeight),
                      true, // <== 关掉 border
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    const float buttonAreaHeight = ImGui::GetFontSize() + styleItemSpacing * 2;

    // 计算两部分之间的垂直间距
    float sepHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // Separator + 默认行高
    // 1) 结果区，高度正好 contentHeight - buttonAreaHeight - sepHeight
    ImGui::BeginChild("RightResults",
                      ImVec2(0, contentHeight - buttonAreaHeight - sepHeight),
                      false,
                      ImGuiWindowFlags_HorizontalScrollbar);

    static constexpr ImColor DroneColor = IM_COL32(64, 185, 127, 255);   // 清新绿
    static constexpr ImColor StationColor = IM_COL32(237, 137, 54, 255); // 活力橙

    if (completed.empty())
    {
        ImGui::TextDisabled("暂无完成的测试点");
    }
    else
    {
        int i = 0;
        for (auto &ct : completed)
        {
            ImGui::Separator();
            // 结果文本
            std::ostringstream oss;
            oss << ICON_FA_LOCATION_DOT << " 测试点 #" << std::setw(3) << std::setfill('0') << ct.id
                << " (" << ICON_FA_HELICOPTER << " 无人机:" << ct.D.size()
                << ", " << ICON_FA_TOWER_BROADCAST << " 站点:" << ct.P.size()
                << ") -> " << ICON_FA_SQUARE_ROOT_VARIABLE << " 最小飞行总距离 = " << ct.res.first
                << ", " << ICON_FA_STOPWATCH << " 用时 = " << ct.res.second << " ms";

            ImGui::TextUnformatted(oss.str().c_str());
            // 计算数轴最小/最大
            ll minX = LLONG_MAX, maxX = LLONG_MIN;
            for (ll x : ct.D)
            {
                minX = std::min(minX, x);
                maxX = std::max(maxX, x);
            }
            for (ll x : ct.P)
            {
                minX = std::min(minX, x);
                maxX = std::max(maxX, x);
            }
            double span = double(maxX - minX);
            if (span < 1)
                span = 1;
            minX = ll(double(minX) - span * 0.05);
            maxX = ll(double(maxX) + span * 0.05);

            ImGuiStyle &style = ImGui::GetStyle();
            float borderSize = style.WindowBorderSize; // 默认 1 像素
            ImVec2 padding = style.WindowPadding;      // 左右上下内边距
            // BeginChild("RightResults") 后，下一次 GetCursorScreenPos() 已是内边距外的位置
            ImVec2 contentScreenPos = ImGui::GetCursorScreenPos();

            // 可用内容大小（不含滚动条等）
            ImVec2 avail = ImGui::GetContentRegionAvail();
            // 计算数轴真正的绘制区域
            // 去掉左右两侧的 padding，和上下两侧 border
            ImVec2 regionPos = ImVec2(
                contentScreenPos.x + padding.x + borderSize,
                contentScreenPos.y + padding.y + borderSize);
            ImVec2 regionSize = ImVec2(
                avail.x - padding.x * 2 - borderSize * 2,
                60 // 你想要的高度
            );
            // 占位按钮，保证鼠标事件正常
            ImGui::SetCursorScreenPos(regionPos);
            ImGui::InvisibleButton(
                (std::string("axis##") + std::to_string(i)).c_str(),
                regionSize);
            // 获取画笔
            ImDrawList *dl = ImGui::GetWindowDrawList();
            // 画背景线：横跨整个 regionSize.x
            ImVec2 p0 = ImVec2(regionPos.x,
                               regionPos.y + regionSize.y * 0.5f);
            ImVec2 p1 = ImVec2(regionPos.x + regionSize.x,
                               regionPos.y + regionSize.y * 0.5f);
            dl->AddLine(p0, p1, IM_COL32(200, 200, 200, 255), 2.0f);
            // 投影函数：考虑修正后的 regionPos 和 regionSize
            auto project = [&](double x)
            {
                double t = (x - minX) / (maxX - minX);
                return regionPos.x + float(t * regionSize.x);
            };
            // 绘制点
            for (ll x : ct.D)
            {
                ImVec2 pt(project(x), p0.y);
                dl->AddCircleFilled(pt, point_size, DroneColor);
            }
            for (ll x : ct.P)
            {
                ImVec2 pt(project(x), p0.y);
                dl->AddCircleFilled(pt, point_size, StationColor);
            }
            // 恢复 Cursor，绘制标注
            ImGui::SetCursorScreenPos(
                ImVec2(regionPos.x,
                       regionPos.y + regionSize.y + 4));
            ImGui::TextColored(DroneColor, "绿-无人机");
            ImGui::SameLine();
            ImGui::TextColored(StationColor, "橙-站点");
            ImGui::Dummy(ImVec2(0, 10));
            i++;
        }
    }
    ImGui::EndChild();
    ImGui::BeginChild("RightButtons",
                      ImVec2(0, buttonAreaHeight),
                      false);
    ImGui::Text("操作：");
    ImGui::SameLine();
    if (ImGui::Button("清空已完成"))
    {
        completed.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("重新检查任务"))
    {
        check_tasks();
    }
    static bool bg_setting = false;
    ImGui::SameLine();
    ImGui::Checkbox("打开设置 " ICON_FA_GEAR, &bg_setting);

    if (bg_setting && ImGui::Begin("设置", &bg_setting))
    {
        ImGui::SliderFloat("点大小", &point_size, 0.1f, 12.0f);
        ImGui::SliderInt("HUE颜色迭代速度", &hue_speed, 1, 32);
        ImGui::Checkbox("反向迭代", &hue_reversed);
        ImGui::SliderFloat("缩放", &background.zoom, 0.1f, 3.0f);
        ImGui::SliderFloat("中心 - X", &background.center_x, -1000.0f, 1000.0f);
        ImGui::SliderFloat("中心 - Y", &background.center_y, -1000.0f, 1000.0f);
        ImGui::SliderFloat("不透明度", &background.transparency, 0.0f, 1.0f);
        ImGui::SliderFloat("字体大小", &font_size, 8.0f, 64.0f);
        if (ImGui::Button("Save"))
        {
            save_config();
        }
        ImGui::End();
    }
    ImGui::EndChild(); // ButtomChild

    ImGui::EndChild(); // Rightpanel
    ImGui::EndChild(); // Main
}

void CGui::generate_testcases()
{
    testcases_input.push_back({});
}

void CGui::check_tasks()
{
    for (auto it = tasks.begin(); it != tasks.end();)
    {
        PendingTask &task = *it;
        if (task.fut.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
        {
            Result res = task.fut.get();
            completed.push_back({task.id, res, std::move(task.D), std::move(task.P)});
            it = tasks.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

std::vector<ll> parse_ll_vector(const std::string &s)
{
    std::vector<ll> res;
    std::istringstream iss(s);
    std::string token;
    while (iss >> token)
    {
        try
        {
            res.push_back(std::stoll(token));
        }
        catch (...)
        {
        }
    }
    return res;
}

void CGui::submitTestcase(int id)
{
    if (id < 0 || id >= (int)testcases_input.size())
        return;
    stInput &input = testcases_input[id];
    std::vector<ll> D = parse_ll_vector(input.a);
    std::vector<ll> P = parse_ll_vector(input.b);
    std::vector<ll> cap = parse_ll_vector(input.c);
    // 提交异步任务
    auto fut = algorithm.submitTask(&D, &P, &cap);
    tasks.push_back({id, std::move(fut), D, P, cap});
}

void CGui::import_test_cases()
{
    std::ifstream fin("data.txt");
    if (!fin.is_open())
    {
        return;
    }
    testcases_input.clear();
    std::string line;
    // 按组读取测试用例
    while (true)
    {
        // 读取第一行：n m
        if (!std::getline(fin, line))
            break;
        // 去除空行
        if (line.empty())
            continue;

        std::istringstream header(line);
        int n, m;
        if (!(header >> n >> m))
            continue; // 解析失败则跳过

        // 读取无人机位置行
        std::string drones, stations, caps;
        if (!std::getline(fin, drones))
            break;
        // 读取站点位置行
        if (!std::getline(fin, stations))
            break;
        // 读取站点容量行
        if (!std::getline(fin, caps))
            break;

        // 存入 testcases_input
        stInput si;
        si.a = drones;
        si.b = stations;
        si.c = caps;
        testcases_input.push_back(std::move(si));
    }
}

void CGui::load_config()
{
    std::fstream fs("config.txt");
    if (fs.is_open())
    {
        fs >> font_size >> background.center_x >> background.center_y >> background.transparency >> background.zoom >> point_size >> hue_speed >> hue_reversed;
        fs.close();
    }
}
void CGui::save_config()
{
    std::ofstream fs("config.txt", std::ios::out);
    if (fs.is_open())
    {
        fs << font_size << "\n"
           << background.center_x << "\n"
           << background.center_y << "\n"
           << background.transparency << "\n"
           << background.zoom << "\n"
           << point_size << "\n"
           << hue_speed << "\n"
           << hue_reversed << "\n";

        fs.close();
        std::cout << "Config saved.\n";
    }
    else
    {
        std::cerr << "Failed to open config file for writing.\n";
    }
}