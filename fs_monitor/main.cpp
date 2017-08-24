#include "monitor.hpp"
#include "inotify_monitor.hpp"

typedef fsm::inotify_monitor fs_monitor;

int main(int argc, char *argv[])
{
    monitor *fsm = new fs_monitor();

    return 0;
}
