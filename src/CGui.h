//
// Created by Administrator on 2025/5/7.
//

#ifndef CGUI_H
#define CGUI_H
#include <random>

#include "CAlgorithm.h"
#include "CTestcase.h"
#include "HueCycler.h"
#include "GLFW/glfw3.h"

struct ImFont;

struct stInput {
    std::string a,b,c;
};
struct CompletedTask {
    int                     id;
    Result                  res;
    std::vector<ll>         D;
    std::vector<ll>         P;
};
struct PendingTask {
    int                id;
    std::future<Result> fut;
    std::vector<ll>    D;
    std::vector<ll>    P;
    std::vector<ll>    cap;
};

class CGui {
public:
    CGui();
    void setup();
    void shutdown();

    void render();
    bool isQuitEventTriggered();
    void renderMain();
    void check_tasks();

private:
    static const std::string window_title;

    bool inited;
    bool quit;
    int active_room_view_id;

    GLFWwindow* glfw_window;
    ImFont *regularFont, *largeFont;

    HueCycler hue_cycler;
    CAlgorithm algorithm;
    std::vector<CompletedTask> completed;
    std::vector<PendingTask> tasks;

    std::vector<stInput> testcases_input;
    void generate_testcases();
    void submitTestcase(int id);

    void import_test_cases();
};


#endif //CGUI_H
