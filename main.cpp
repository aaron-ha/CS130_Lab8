#include "application.h"

int main(int argc, char* argv[])
{
    int width = 1000;
    int height = 600;

    application app;
    app.init(argc, argv, width, height);
    app.run();
}
