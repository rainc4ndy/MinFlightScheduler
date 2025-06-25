
#include <imgui.h>
#include <iostream>

#include "CBackground.h"
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include <GL/gl.h>
#include <GL/glu.h>



void CBackground::draw() {
    if (!texture_id) {
        std::cout << "NO TEXTURE!" << std::endl;
        return; // 没有纹理就跳过
    }
    auto ori = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(0, 0));

    ImVec2 windowSize = ImGui::GetWindowSize();

    float aspect_image = static_cast<float>(img_width) / img_height;
    float aspect_window = static_cast<float>(windowSize.x) / windowSize.y;

    float new_width, new_height;

    if (aspect_image > aspect_window) {
        new_width = windowSize.x * zoom;
        new_height = new_width / aspect_image;
    } else {
        new_height = windowSize.y * zoom;
        new_width = new_height * aspect_image;
    }

    float x = (windowSize.x - new_width) / 2 + center_x;
    float y = (windowSize.y - new_height) / 2 + center_y;

    ImGui::SetCursorPos(ImVec2(x, y));
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, transparency);
    ImGui::Image((ImTextureID)(intptr_t)texture_id, ImVec2(new_width, new_height));
    ImGui::PopStyleVar();
    ImGui::SetCursorPos(ori);
}

void CBackground::load_png(std::string path) {
    if (texture_id) {
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }

    int channels;
    unsigned char* data = stbi_load(path.c_str(), &img_width, &img_height, &channels, STBI_rgb_alpha);
    if (!data) {
        std::cerr << "Failed to load PNG: " << path << std::endl;
        return;
    }

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    using namespace std;
    cout << "img_width f= " << img_width << endl;
    cout << "img_height f= " << img_height << endl;
    // 替换成 gluBuild2DMipmaps，自动生成多级纹理
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img_width, img_height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  

    stbi_image_free(data);
}