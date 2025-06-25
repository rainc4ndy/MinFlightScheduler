#ifndef C9629E7E_E7EE_4F75_A776_AE50AE62E6C2
#define C9629E7E_E7EE_4F75_A776_AE50AE62E6C2

#include <string>

class CBackground {
public:
    float zoom = 1.0f;
    float center_x = 0.0f, center_y = 0.0f;
    float transparency = 1.0f;
    void draw();
    void load_png(std::string path);
    
private:
    unsigned int texture_id = 0;
    int img_width = 0, img_height = 0;
};


#endif /* C9629E7E_E7EE_4F75_A776_AE50AE62E6C2 */