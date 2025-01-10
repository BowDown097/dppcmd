#pragma once
#include "dppcmd/modules/module.h"

namespace dpp { class guild_member; }

class TestModule : public dppcmd::module<TestModule>
{
public:
    TestModule();
private:
    dppcmd::command_result commands(const std::optional<std::string>& filter);
    dppcmd::command_result testInput(const std::string& str, int num, double dnum,
        const std::optional<dppcmd::remainder<dpp::guild_member>> memberOpt);
};
