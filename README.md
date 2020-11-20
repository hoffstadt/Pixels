# Pixels
<sub>(This library is available under a free and permissive license)</sub>

Simple Framework to put pixels onto a window. Useful for testing fundamental rasterization and graphics techniques.

## Usage
```cpp
#include "Graphics.h"

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

        graphics.endFrame();
    }

}
```

## License

_Pixels_ is licensed under the [MIT License](https://github.com/hoffstadt/Pixels/blob/master/LICENSE).
