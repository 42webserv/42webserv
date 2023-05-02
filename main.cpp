#include <iostream>
#include <cstdlib> // for getenv

int main()
{
    // Content-type header
    std::cout << "Content-type:text/html\r\n\r\n";

    // Query string
    char *query_string = getenv("QUERY_STRING");
    if (query_string != nullptr)
    {
        std::cout << "<p>Query string: " << query_string << "</p>";
    }
    // std::cout << "<p>Query string: " << query_string << "</p>";

    // HTTP_USER_AGENT environment variable
    std::string input;
    if (std::getline(std::cin, input))
    {
        std::cout << "<p>Input: " << input << "</p>";
    }
    char *user_agent = getenv("HTTP_USER_AGENT");
    // std::cout << "<p>User agent: " << user_agent << "</p>";

    // Form data
    // std::string input;
    // std::getline(std::cin, input);
    // std::cout << "<p>Input: " << input << "</p>";

    return 0;
}
