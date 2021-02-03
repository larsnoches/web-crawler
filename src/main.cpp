#include <iostream>
#include "httpclient.h"

using namespace std;

int main()
{
    string url = "https://www.google.ru";
    HttpClient httpClient(url, true);
    httpClient.run();
    return 0;
}
