#include <iostream>
#include <string>
#include <curl/curl.h>
#include <ctime>
#include <vector>
#include <regex>
#include <set>
#include <fstream>
#include "Helpers.h"
using namespace std;

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    std::vector<std::string> seeds = {"https://clutch.neocities.org/","https://boplaid.neocities.org/","https://forum.frutigeraeroarchive.org/"}; // make it read from a text file later
    std::set<std::string> visited;
    ofstream Index("index.tif");
    for (size_t i = 0; i < seeds.size(); i++) {
        std::string seed = seeds[i];
        if (visited.count(seed)) continue;
        visited.insert(seed);
        CURL *curl = curl_easy_init();
        if (curl) {
            CURLcode result;
            std::string response;
            curl_easy_setopt(curl, CURLOPT_URL, seed.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCB);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            result = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            std::cout << "URL: " << seed << std::endl;
            std::cout << "Title: " << getTagText("<title>","</title>",response) << std::endl;
            std::cout << "Snippet: " << getTagText("<p>","</p>",response) << std::endl;
            std::time_t timestamp = std::time(nullptr);

            Index << "<page><title>" << getTagText("<title>","</title>",response) << "</title><url>" << seed << "</url><snippet>" << getTagText("<p>","</p>",response) << "</snippet><timestamp>" << timestamp << "</timestamp></page>\n";
            std::vector<std::string> allhref = getAllHrefLinks(response);
            std::vector<std::string> newlinks;
            for (const auto& href : allhref) {
                std::string fullurl = "";
                if (href.find(seed) != std::string::npos) {
                    std::cout << "url in sub-url [seed]: " << href << std::endl;
                } else {
                    if (href.find("https://") != std::string::npos) {
                        std::cout << "url NOT in sub-url: " << href << std::endl;
                    } else {
                        std::cout << "url in sub-url [seed]: " << href << std::endl;
                        if (href.rfind("/", 0) == 0 || href.rfind("./", 0) == 0) {
                            if (href.rfind("/", 0) == 0) {
                                if (endsWith(stramp(seed,"://"),"/")) {
                                    std::string full = seed + stramp(href,"/");
                                    fullurl = full;
                                }
                            } else if (href.rfind("./", 0) == 0) {
                                if (endsWith(stramp(seed,"://"),"./")) {
                                    std::string full = seed + stramp(href,"./");
                                    fullurl = full;
                                }
                            }
                        } else {
                                std::string full = seed + href;
                                fullurl = full;
                            }
                    }
                }
                if (!fullurl.empty() && !visited.count(fullurl)) {
                    newlinks.push_back(fullurl);
                }
            }
            seeds.insert(seeds.end(), newlinks.begin(), newlinks.end());
        }
    }
    Index.close();
    curl_global_cleanup();
}
