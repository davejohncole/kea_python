// File created from messages.mes on Mon Dec 30 2019 23:53

#include <cstddef>
#include <log/message_types.h>
#include <log/message_initializer.h>

namespace isc {
namespace log {

extern const isc::log::MessageID LOG_KEAPY_HOOK = "LOG_KEAPY_HOOK";
extern const isc::log::MessageID LOG_KEAPY_PYTHON = "LOG_KEAPY_PYTHON";

} // namespace log
} // namespace isc

namespace {

const char* values[] = {
    "LOG_KEAPY_HOOK", "%1",
    "LOG_KEAPY_PYTHON", "%1",
    NULL
};

const isc::log::MessageInitializer initializer(values);

} // Anonymous namespace

