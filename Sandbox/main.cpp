#include "Graphics.h"
#include <imgui.h>

int main()
{

    Pixels::Graphics graphics(800, 600);
    
    // main loop
    while (true)
    {

        if (Pixels::Window::ProcessMessages()) break;

        graphics.beginFrame();

        graphics.putPixel(100, 100, 255, 0, 0);
        graphics.putPixel(101, 101, 0, 255, 0);
        graphics.putPixel(102, 102, 0, 0, 255);

        graphics.drawLine(300, 200, 200, 300, { 255, 0, 255 });

        graphics.endFrame();
    }

}