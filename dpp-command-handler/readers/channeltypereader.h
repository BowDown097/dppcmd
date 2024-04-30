#ifndef CHANNELTYPEREADER_H
#define CHANNELTYPEREADER_H
#include "typereader.h"

namespace dpp
{
    class channel;

    class channel_in : public type_reader<channel*>
    {
    public:
        type_reader_result read(cluster* cluster, const message_create_t* context, std::string_view input) override;
    protected:
        void add_results_by_id(std::string_view input);
        void add_results_by_mention(std::string_view input);
        void add_results_by_name(std::string_view input);
    };
}

#endif // CHANNELTYPEREADER_H
