#include "credentials.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

void Credentials::initialize()
{
    std::ifstream stream("credentials.json");
    if (!stream.good())
    {
        std::cerr << "credentials.json file not found or is not a valid JSON file." << std::endl;
        exit(EXIT_FAILURE);
    }

    try
    {
        nlohmann::json reader;
        stream >> reader;
        m_token = reader["token"].template get<std::string>();
    }
    catch (const nlohmann::json::exception& e)
    {
        std::cerr << "Failure parsing credentials.json: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
