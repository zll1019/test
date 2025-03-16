#include "logger.h"

static void module_init()
{
    log_init("/var/log/storage/output.log", 0, 0, LOG_LEVEL_DEBUG);
}

int main() {

    module_init();
    
    LOG_DEBUG("Starting application");
    LOG_INFO("Initialized %d modules", 5);
    LOG_WARNING("Low memory: %.2f MB free", 12.5);
    LOG_ERROR("Failed to connect to database");
    LOG_CRITICAL("System overheating!");

    return 0;
}