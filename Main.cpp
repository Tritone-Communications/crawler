#include <iostream>
#include <string>
#include <curl/curl.h>
#include <ctime>
#include <vector>
#include <regex>
#include <fstream>
using namespace std;

size_t WriteCB(void* contents, size_t size, size_t nmemb, std::string* usrdat) {
    size_t totalsize = size * nmemb;
    usrdat->append(static_cast<char*>(contents), totalsize);
    return totalsize;
}

std::string getTagText(std::string startTag, std::string endTag, std::string inputHtml) {
    size_t startTitle = inputHtml.find(startTag); // 'Title' because it was originally for titles. Sorry
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
int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    std::string seeds[] = {"https://clutch.neocities.org/","https://boplaid.neocities.org/","https://forum.frutigeraeroarchive.org/"}; // make it read from a text file later
    ofstream Index("index.tif");
    for (std::string &seed : seeds) {
        CURL *curl = curl_easy_init();
        if (curl) {
            CURLcode result;
            std::string response;
            curl_easy_setopt(curl, CURLOPT_URL, seed.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCB);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            result = curl_easy_perform(curl);
            std::cout << "URL: " << seed << std::endl;
            std::cout << "Title: " << getTagText("<title>","</title>",response) << std::endl;
            std::cout << "Snippet: " << getTagText("<p>","</p>",response) << std::endl;
            std::time_t timestamp = std::time(nullptr);

            Index << "<page><title>" << getTagText("<title>","</title>",response) << "</title><url>" << seed << "</url><snippet>" << getTagText("<p>","</p>",response) << "</snippet><timestamp>" << std::ctime(&timestamp) << "</timestamp></page>";
            std::vector<std::string> allhref = getAllHrefLinks(response);
            for (const auto& href : allhref) {
                if (href.find(seed) != std::string::npos) {
                    std::cout << "url in sub-url [seed]: " << href << std::endl;
                } else {
                    if (href.find("https://") != std::string::npos) {
                        std::cout << "url NOT in sub-url: " << href << std::endl;
                    } else {
                        std::cout << "url in sub-url [seed]: " << href << std::endl;
                        if (href.rfind("/", 0) == 0 || href.rfind("./", 0) == 0) {
                            std::string full = seed + href;
                        }
                    }
                }
            }
            curl_easy_cleanup(curl);
        }
    }
    Index.close();
}
