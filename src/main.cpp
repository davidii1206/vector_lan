#include "app.h"

int main () {
    {
        App app(1280, 720);
        while (app.is_running()) {
            app.run();
        }
    }
    return 0;
}