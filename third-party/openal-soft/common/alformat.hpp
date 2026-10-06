#ifndef AL_FORMAT_HPP
#define AL_FORMAT_HPP

#if defined(__APPLE__)
#include <AvailabilityMacros.h>
#endif

/* On Apple platforms, std::format requires floating-point std::to_chars,
 * which isn't available prior to macOS 13.3 / iOS 16.3. Older versions of
 * libstdc++ also lack the <format> header.
 */
#if (defined(MAC_OS_X_VERSION_MIN_REQUIRED) && MAC_OS_X_VERSION_MIN_REQUIRED < 130300) \
    || (defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED < 160300) \
    || (defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) \
        && __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ < 160300) \
    || !__has_include(<format>)
#include "fmt/format.h"

namespace al {

using fmt::format;
using fmt::formatter;
using fmt::format_args;
using fmt::format_string;
using fmt::make_format_args;
using fmt::string_view;
using fmt::vformat;

} /* namespace al */

#else

#include <format>

namespace al {

using std::format;
using std::formatter;
using std::format_args;
using std::format_string;
using std::make_format_args;
using std::string_view;
using std::vformat;

} /* namespace al */
#endif

#endif /* AL_FORMAT_HPP */
