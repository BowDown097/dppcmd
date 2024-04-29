#ifndef TYPEREADER_H
#define TYPEREADER_H
#include "dpp-command-handler/results/typereaderresult.h"
#include <span>
#include <vector>

namespace dpp
{
    class cluster;
    class message_create_t;

    template<typename T>
    class type_reader_value
    {
    public:
        type_reader_value(T* value, float weight) : m_value(value), m_weight(weight) {}
        T* value() const { return m_value; }
        float weight() const { return m_weight; }
    private:
        T* m_value;
        float m_weight;
    };

    template<typename T>
    class type_reader
    {
    public:
        virtual type_reader_result read(cluster* cluster, const message_create_t* context, std::string_view input) = 0;

        // max_element isn't used here to avoid pulling in <algorithm>
        T* top_result() const
        {
            const type_reader_value<T>* topValue{};

            for (const type_reader_value<T>& value : m_results)
                if (!topValue || topValue->weight() < value.weight())
                    topValue = &value;

            return topValue ? topValue->value() : nullptr;
        }

        bool has_result() const { return !m_results.empty(); }
        explicit operator bool() const { return has_result(); }

        std::span<const type_reader_value<T>> results() const { return m_results; }
    protected:
        void add_result(T* value, float weight = 1.0f) { m_results.emplace_back(value, weight); }
    private:
        std::vector<type_reader_value<T>> m_results;
    };
}

#endif // TYPEREADER_H
