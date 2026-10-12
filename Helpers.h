#pragma once

size_t WriteCB(void* contents, size_t size, size_t nmemb, std::string* usrdat);
std::string getTagText(std::string startTag, std::string endTag, std::string inputHtml);
std::vector<std::string> getAllTagText(const std::string& startTag, const std::string& endTag, std::string& inputHtml);
std::vector<std::string> getAllHrefLinks(const std::string& inputHtml);
std::string stramp(std::string inputString, std::string toRemove);
bool endsWith(const std::string& inputString, const std::string& ending);
bool isBlocked(std::string website);
