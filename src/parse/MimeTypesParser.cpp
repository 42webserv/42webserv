#include "MimeTypesParser.hpp"

/*
 * A default constructor
 */
MimeTypesParser::MimeTypesParser(ParsedConfig &config)
{
    config.getAllDirectives(this->includes, config.getDirectives(), "include");
    std::string mimeTypesPath = "." + getMimeTypesPath(this->includes).substr(0, getMimeTypesPath(this->includes).length() - 1);
    std::cout << mimeTypesPath << std::endl;
    if (mimeTypesPath.length() == 1 && mimeTypesPath == ".")
        mimeTypesPath = "./assets/conf/default.conf";
    parseMimeTypes(mimeTypesPath);
}

/*
 * A copy constructor
 */
MimeTypesParser::MimeTypesParser(const MimeTypesParser &ref)
{
    includes = ref.includes;
    mimeMap = ref.mimeMap;
}

/*
 * A assignment operator overload
 */
MimeTypesParser &MimeTypesParser::operator=(const MimeTypesParser &ref)
{
    if (this != &ref)
    {
        includes = ref.includes;
        mimeMap = ref.mimeMap;
    }
    return *this;
}

/*
 * A destructor
 */
MimeTypesParser::~MimeTypesParser()
{
}

void MimeTypesParser::parseMimeTypes(const std::string &filename)
{
    std::ifstream file(filename);

    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            line = line.substr(0, line.length() - 1); // mime.types 각 줄 끝의 ; 제거
            if (line[0] != '#' && !line.empty())
            {
                std::vector<std::string> tokens;
                std::istringstream iss(line);
                std::string token;

                std::cout << line << std::endl;

                while (iss >> token)
                    tokens.push_back(token);

                if (tokens.size() > 1)
                {
                    std::string mime = tokens[0];
                    for (size_t i = 1; i < tokens.size(); i++)
                        this->mimeMap[tokens[i]] = mime;
                }
            }
        }
        file.close();
    }
}

std::string MimeTypesParser::getMimeType(const std::string &filename)
{
    std::string extension = filename.substr(filename.find_last_of(".") + 1);
    std::cout << extension << std::endl;
    for (std::map<std::string, std::string>::iterator iter = this->mimeMap.begin(); iter != this->mimeMap.end(); iter++)
        if (iter->first == extension)
            return iter->second;
    return "application/octet-stream";
}

std::string MimeTypesParser::getMimeTypesPath(std::vector<Directive> directive)
{
    for (size_t i = 0; i < directive.size(); i++)
        if (directive[i].name == "include")
            if (directive[i].value.substr(directive[i].value.length() - 11) == "mime.types;")
                return directive[i].value;
    return "";
}
