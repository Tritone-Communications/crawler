#pragma once

size_t WriteCB(void* contents, size_t size, size_t nmemb, std::string* usrdat);
std::string getTagText(std::string startTag, std::string endTag, std::string inputHtml);
std::vector<std::string> getAllTagText(const std::string& startTag, const std::string& endTag, std::string& inputHtml);
std::vector<std::string> getAllHrefLinks(const std::string& inputHtml);
