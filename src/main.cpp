#include <stdexcept>
#include <iostream>

#include "CGui.h"
#include "CApplication.h"

int main() {
    using namespace std;

    cout << "program inited" << endl;
    auto& app = CApplication::getInstance();

    auto gui = app.getGui();
    try {
        gui->setup();
    } catch (std::runtime_error& e) {
        cout << e.what() << endl;
        return -1;
    }
    while (!gui->isQuitEventTriggered()) {
        gui->render();
        gui->check_tasks();
    }
    gui->shutdown();

    cout << "exited" << endl;

    return 0;
}