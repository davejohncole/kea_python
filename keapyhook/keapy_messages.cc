// File created from keapy_messages.mes on Tue Dec 24 2019 00:30

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

