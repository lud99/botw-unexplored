#include <switch.h>

#include <string>

namespace Accounts
{
    AccountUid RequestProfileSelection();

    std::string GetNickname(AccountUid uid);
}