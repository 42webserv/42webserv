#include "MimeTypesParser.hpp"

/**
 * 파싱된 config 클래스를 생성자 인자로 받아 mime.types 파일의 경로를 찾음. 없으면 기본으로 설정한 경로로 추가 후 mime.types 파싱
 *
 * @param config
 */
MimeTypesParser::MimeTypesParser(const Config &config)
{
    config.getAllDirectives(this->includes, config.getDirectives(), "types");
    parseMimeTypes();
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
 */
void MimeTypesParser::parseMimeTypes()
{
    for (size_t i = 0; i < this->includes[0].block.size(); i++)
    {
        std::vector<std::string> tokens;
        std::istringstream iss(this->includes[0].block[i].value);
        std::string token;

        while (iss >> token)
            tokens.push_back(token);

        std::string mime = this->includes[0].block[i].name;
        for (size_t j = 0; j < tokens.size(); j++)
            this->mimeMap[tokens[j]] = mime;
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
