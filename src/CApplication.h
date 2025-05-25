//
// Created by Administrator on 2025/5/7.
//

#ifndef CAPPLICATION_H
#define CAPPLICATION_H
#include <memory>

#include "CGui.h"

class CApplication {
public:
    static CApplication &getInstance() {
        static CApplication instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    CGui* getGui() const;

private:
    void init();
    CApplication() {
        init();
    }
    CApplication(CApplication const &); // Don't Implement
    void operator=(CApplication const &); // Don't implement
    std::unique_ptr<CGui> pGui;
public:
};

#endif //CAPPLICATION_H
