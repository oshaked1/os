#define RESET_PORT 0xcf9
#define WARM_RESET 0x6
#define COLD_RESET 0xe

void hardware_warm_reset();
void hardware_cold_reset();
#define hardware_reset hardware_warm_reset