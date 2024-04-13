#ifndef TYPEREADER_H
#define TYPEREADER_H
#include "dpp-command-handler/results/typereaderresult.h"
#include <vector>

namespace dpp { class cluster; class message_create_t; }

template<typename T>
class TypeReaderValue
{
public:
    TypeReaderValue(T* value, float weight) : m_value(value), m_weight(weight) {}
    T* value() const { return m_value; }
    float weight() const { return m_weight; }
private:
    T* m_value;
    float m_weight;
};

template<typename T>
class TypeReader
{
public:
    virtual TypeReaderResult read(dpp::cluster* cluster, const dpp::message_create_t* context, std::string_view input) = 0;

    // max_element isn't used here to avoid pulling in <algorithm>
    T* topResult() const
    {
        const TypeReaderValue<T>* topValue{};

        for (const TypeReaderValue<T>& value : m_results)
            if (!topValue || topValue->weight() < value.weight())
                topValue = &value;

        return topValue ? topValue->value() : nullptr;
    }

    bool hasResult() const { return !m_results.empty(); }
    explicit operator bool() const { return hasResult(); }

    std::vector<TypeReaderValue<T>> results() const { return m_results; }
protected:
    void addResult(T* value, float weight = 1.0f) { m_results.emplace_back(value, weight); }
private:
    std::vector<TypeReaderValue<T>> m_results;
};

#endif // TYPEREADER_H
