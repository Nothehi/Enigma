#include "Signals.hpp"

#include <cerrno>
#include <csignal>
#include <initializer_list>
#include <system_error>

namespace
{
    volatile std::sig_atomic_t s_stopRequested = 0;

    void onStopSignal(int)
    {
        s_stopRequested = 1;
    }
}

void signals::installStopHandlers()
{
    struct sigaction action{};
    action.sa_handler = onStopSignal;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    for (const int signal : {SIGINT, SIGTERM, SIGHUP})
    {
        if (sigaction(signal, &action, nullptr) != 0)
            throw std::system_error(errno, std::generic_category(), "sigaction");
    }
}

bool signals::stopRequested() noexcept
{
    return s_stopRequested != 0;
}
