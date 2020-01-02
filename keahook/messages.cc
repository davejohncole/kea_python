// File created from messages.mes on Thu Jan 02 2020 00:21

#include <cstddef>
#include <log/message_types.h>
#include <log/message_initializer.h>

namespace isc {
namespace log {

extern const isc::log::MessageID LOG_PYTHON = "LOG_PYTHON";
extern const isc::log::MessageID LOG_PYTHON_HOOK = "LOG_PYTHON_HOOK";

} // namespace log
} // namespace isc

namespace {

const char* values[] = {
    "LOG_PYTHON", "%1",
    "LOG_PYTHON_HOOK", "%1",
    NULL
};

const isc::log::MessageInitializer initializer(values);

} // Anonymous namespace

