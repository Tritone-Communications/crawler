#include <iostream>
#include <string>
#include <curl/curl.h>
#include <ctime>
#include <vector>
#include <regex>
#include "robotstxt/robots.h"
#include <fstream>
using namespace std;

size_t WriteCB(void* contents, size_t size, size_t nmemb, std::string* usrdat) {
    size_t totalsize = size * nmemb;
    usrdat->append(static_cast<char*>(contents), totalsize);
    return totalsize;
}

std::string getTagText(std::string startTag, std::string endTag, std::string inputHtml) {
    size_t startTitle = inputHtml.find(startTag); // 'Title' because it was originally for titles. Sorry
    if (startTitle == std::string::npos) {
        return "";
    }
    startTitle += startTag.length();
    size_t endTitle = inputHtml.find(endTag, startTitle);
    std::string curPageTitle = inputHtml.substr(startTitle, endTitle - startTitle);
    return curPageTitle;
}

std::vector<std::string> getAllTagText(const std::string& startTag, const std::string& endTag, std::string& inputHtml) {
    std::vector<std::string> results;
    size_t pos = 0;
    while (true) {
        size_t start = inputHtml.find(startTag, pos);
        if (start == std::string::npos) break;
        start += startTag.length();
        size_t end = inputHtml.find(endTag, start);
        if (end == std::string::npos) break;
        results.push_back(inputHtml.substr(start, end - start));
        pos = end + endTag.length();
    }
    return results;
}

std::vector<std::string> getAllHrefLinks(const std::string& inputHtml) {
    std::vector<std::string> results;
    const std::regex anchorRegex(
        R"(<a\b[^>]*>)",
        std::regex_constants::icase
    );
    const std::regex hrefRegex(
        R"(href\s*=\s*(['"])(.*?)\1)",
        std::regex_constants::icase
    );
    auto begin = std::sregex_iterator(
        inputHtml.begin(), inputHtml.end(), anchorRegex
    );
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
        std::string tag = it->str();
        std::smatch match;
        if (std::regex_search(tag, match, hrefRegex)) {
            results.push_back(match[2].str());
        }
    }
    
    return results;
}

std::string stramp(std::string inputString, std::string toRemove) {
    if (inputString.empty() || inputString.size() < 7) {
        return "";
    }
    size_t rpos = inputString.find(toRemove);
    if (rpos != std::string::npos) {
        inputString.erase(rpos, toRemove.length());
    }
    return inputString;
}

bool endsWith(const std::string& inputString, const std::string& ending) {
    if (ending.length() > inputString.length()) return false;
    return inputString.rfind(ending) == (inputString.length() - ending.length());
}

// Robots stuff
bool isBlocked(std::string website) {
    std::string robotsContent = "";
    CURL* robotsCurl = curl_easy_init();
    std::string robotsUrl = website + "robots.txt";
    curl_easy_setopt(robotsCurl, CURLOPT_URL, robotsUrl);
    curl_easy_setopt(robotsCurl, CURLOPT_WRITEFUNCTION, WriteCB);
    curl_easy_setopt(robotsCurl, CURLOPT_WRITEDATA, &robotsContent);
    curl_easy_setopt(robotsCurl, CURLOPT_USERAGENT, "Tritone Communications World Wide Web Crawler (triserv.jet198@slmail.me)");
    CURLcode robotsResponse = curl_easy_perform(robotsCurl);
    curl_easy_cleanup(robotsCurl);
    if (robotsResponse != CURLE_OK) {
        return false;
    }
    googlebot::RobotsMatcher matcher;
    const std::vector<std::string> userAgentLand = { "Tritone Communications World Wide Web Crawler (triserv.jet198@slmail.me)" };
    bool doICrawlIt = matcher.AllowedByRobots(robotsContent, &userAgentLand, website);
    return !doICrawlIt;
}
