#ifndef CONFIG_TASKSCONFIG_H_
#define CONFIG_TASKSCONFIG_H_

#include <config/FreeRTOSConfig.h>
#include <cstdint>
#include <cstring>

namespace tasks {

namespace config {

/* Common */
static constexpr uint32_t MIN_TASK_STACK_SIZE = configMINIMAL_STACK_SIZE;
static constexpr uint32_t MIN_TASK_PRIORITY = 1;
static constexpr uint32_t MAX_TASK_PRIORITY = configMAX_PRIORITIES;


struct LedBlinkingTask {
    static constexpr uint32_t STACK_SIZE = MIN_TASK_STACK_SIZE;
    static constexpr const char NAME[] = "Led Blinking Task";
    static constexpr uint32_t PRIORITY = MIN_TASK_PRIORITY;

    static constexpr uint32_t SLEEP_TIME = 1000; //ms
};

struct UsbDeviceTask {
    static constexpr uint32_t STACK_SIZE = MIN_TASK_STACK_SIZE;
    static constexpr const char NAME[] = "Usb device Task";
    static constexpr uint32_t PRIORITY = MIN_TASK_PRIORITY;
};

struct UsbHostTask {
    static constexpr uint32_t STACK_SIZE = MIN_TASK_STACK_SIZE * 2;
    static constexpr const char NAME[] = "Usb host Task";
    static constexpr uint32_t PRIORITY = MIN_TASK_PRIORITY;

    static constexpr uint32_t SLEEP_TIME = 10; //ms
};

struct SwitcherTask {
    static constexpr uint32_t STACK_SIZE = MIN_TASK_STACK_SIZE;
    static constexpr const char NAME[] = "Switcher Task";
    static constexpr uint32_t PRIORITY = MIN_TASK_PRIORITY + 1;

    static constexpr uint32_t FIFO_SIZE = 64;
};

struct LogicTask {
    static constexpr uint32_t STACK_SIZE = MIN_TASK_STACK_SIZE;
    static constexpr const char NAME[] = "Logic Task";
    static constexpr uint32_t PRIORITY = MIN_TASK_PRIORITY + 1;

    static constexpr uint32_t FIFO_SIZE = 64;
    static constexpr size_t MAX_PASSWORD_SIZE = 32;
};

struct OutputManagerTask {
    static constexpr uint32_t STACK_SIZE = MIN_TASK_STACK_SIZE * 2;
    static constexpr const char NAME[] = "Output Manager Task";
    static constexpr uint32_t PRIORITY = MIN_TASK_PRIORITY + 1;

    static constexpr uint32_t FIFO_SIZE = 64;
    static constexpr uint32_t MAX_OUTPUT_SRTING_SIZE = 64;
    static constexpr uint32_t PROCESSING_PCKG_BUFFER_SIZE = 64;
    static constexpr uint32_t MAX_MESSAGE_STRING_SIZE = 32;
};


struct DatabaseTask {
    static constexpr uint32_t STACK_SIZE = MIN_TASK_STACK_SIZE * 3;
    static constexpr const char NAME[] = "Database Task";
    static constexpr uint32_t PRIORITY = MIN_TASK_PRIORITY + 1;

    static constexpr uint32_t FIFO_SIZE = 64;
    static constexpr const char DB_NAME[] = "db.kdbx";

	static constexpr uint32_t STRING_POOL_SIZE = 1024 * 55;
	static constexpr uint32_t NODE_COUNT = 350;
};

struct WatchdogTask {
    static constexpr uint32_t STACK_SIZE = MIN_TASK_STACK_SIZE;
    static constexpr const char NAME[] = "Watchdog Task";
    static constexpr uint32_t PRIORITY = MAX_TASK_PRIORITY;
	static constexpr uint32_t WATCHDOG_PERIOD = 5000;
	static constexpr uint32_t RESET_PERIOD    = 200;
};

} /* namespace config */

} /* namespace tasks */


#endif /* CONFIG_TASKSCONFIG_H_ */
