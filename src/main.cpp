#include "app.h"
#include "query_system.h"

int main() {

    // ioUtil::print_system_type();

    const auto myApp = new App("HydroSim", 640, 480);
    myApp->run();
    delete myApp;

    return 0;
}