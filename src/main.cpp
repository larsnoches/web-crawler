#include <iostream>
#include "httpclient.h"

using namespace std;

int main()
{
    string url = "https://nodejs.org/docs/latest-v13.x/api/addons.html";
    HttpClient httpClient(url, true);
    httpClient.start();
//    httpClient.run();
    return 0;
}
