#include "MimeTypesParser.hpp"

/**
 * 파싱된 config 클래스를 생성자 인자로 받아 mime.types 파일의 경로를 찾음. 없으면 기본으로 설정한 경로로 추가 후 mime.types 파싱
 *
 * @param config
 */
MimeTypesParser::MimeTypesParser(Config &config)
{
    config.getAllDirectives(this->includes, config.getDirectives(), "include");
    std::string mimeTypesPath = "." + getMimeTypesPath(this->includes).substr(0, getMimeTypesPath(this->includes).length() - 1);
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

/**
 * mime.types 파일을 열어 한 줄씩 map 자료구조로 저장
 *
 * @param filename mime.types가 존재하는 위치
 */
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

/**
 * 인자가 들어왔을 때, 미리 파싱해둔 mime.types에서 인자에 대응하는 값을 찾아 반환해준다
 *
 * @param extension 파일 확장자
 * @return 인자에 대항하는 값 (ex. text/html, image/gif ...)
 */
std::string MimeTypesParser::getMimeType(const std::string &extension)
{
    std::string ext = extension.substr(extension.find_last_of(".") + 1);
    for (std::map<std::string, std::string>::iterator iter = this->mimeMap.begin(); iter != this->mimeMap.end(); iter++)
        if (iter->first == ext)
            return iter->second;
    return "application/octet-stream";
}

/**
 * include에 존재하는 mime.types에 대한 경로를 찾아준다.
 *
 * @param directive 파싱된 configure file
 * @return mime.types 파일 경로
 */
std::string MimeTypesParser::getMimeTypesPath(std::vector<Directive> directive)
{
    for (size_t i = 0; i < directive.size(); i++)
        if (directive[i].name == "include")
            if (directive[i].value.substr(directive[i].value.length() - 11) == "mime.types;")
                return directive[i].value;
    return "/assets/conf/mime.types";
}
