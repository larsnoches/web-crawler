#include <iostream>
#include "httpclient.h"

using namespace std;

int main()
{
    string url = "https://www.ya.ru";
    HttpClient httpClient(url, false);
    httpClient.run();
    return 0;
}
