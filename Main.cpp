#include <iostream>
#include <string>
#include <curl/curl.h>
size_t WriteCB(void* contents, size_t size, size_t nmemb, std::string* usrdat) {
    size_t totalsize = size * nmemb;
    usrdat->append(static_cast<char*>(contents), totalsize);
    return totalsize;
}
int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    std::string seeds[] = {"https://clutch.neocities.org/"}; // make it read from a text file later
    for (std::string &seed : seeds) {
        CURL *curl = curl_easy_init();
        if (curl) {
            CURLcode result;
            std::string response;
            curl_easy_setopt(curl, CURLOPT_URL, seed.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCB);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            result = curl_easy_perform(curl);
            std::cout << response << std::endl;
            std::string tag = "<title>";
            size_t startTitle = response.find(tag);
            startTitle += tag.length();
            tag = "</title>";
            size_t endTitle = response.find(tag, startTitle);
            std::string curPageTitle = response.substr(startTitle, endTitle - startTitle);
            std::cout << "Current page title: " << curPageTitle << std::endl;

            curl_easy_cleanup(curl);
        }
    }
}
