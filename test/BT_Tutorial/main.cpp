#include "definitions.h"

extern "C" void app_main() {
    bt.begin("Batman");
    foco.setup(rgb_pins, rgb_ch,0);
    while(1){
        if(bt.available()){

            bt.read(buffer,8);
            sscanf(buffer,"%x", &color);
            foco.setColor(color);

        }
    }
}