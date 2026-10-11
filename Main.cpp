#include <iostream>
#include <string>
#include <curl/curl.h>
#include <ctime>
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
            
            curl_easy_cleanup(curl);
        }
    }
    Index.close();
}
