#include "InvalidLookupPositionException.h"

#include <string>

InvalidLookupPositionException::InvalidLookupPositionException(const block_t block, const size_t offset)
    : m_block(block),
      m_offset(offset)
{
}

std::string InvalidLookupPositionException::DetailedMessage()
{
    return "Zone tried to lookup at block " + std::to_string(m_block) + ", offset "
        + std::to_string(m_offset) + " that was not recorded";
}

char const* InvalidLookupPositionException::what() const noexcept
{
    return "Zone tried to lookup at zone offset that is not recorded";
}
