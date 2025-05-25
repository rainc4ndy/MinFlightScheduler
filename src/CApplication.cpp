//
// Created by Administrator on 2025/5/7.
//

#include "CApplication.h"

#include "CGui.h"

CGui * CApplication::getGui() const {
    return pGui.get();
}

void CApplication::init() {
    pGui = std::make_unique<CGui>();
}
