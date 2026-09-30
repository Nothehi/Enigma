#pragma once

namespace signals
{
    // Installs handlers for SIGINT, SIGTERM and SIGHUP that request a graceful stop.
    // Handlers are installed without SA_RESTART, so a blocking read() returns EINTR
    // and the main loop gets a chance to check stopRequested().
    void installStopHandlers();

    [[nodiscard]] bool stopRequested() noexcept;
}
