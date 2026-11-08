#include <Cora/Driver/CompilationSession.h>

#include <utility>

namespace cora::driver
{

    CompilationSession::CompilationSession(BuildConfig config)
        : config_(std::move(config)),
          cirCache_(config_.cacheRoot, config_.cachePolicy),
          objectCache_(config_.cacheRoot, config_.cachePolicy)
    {}

} // namespace cora::driver
