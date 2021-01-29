#include <iostream>
#include "httpclient.h"

using namespace std;

int main()
{
    string url = "https://ya.ru";
    HttpClient httpClient(url);
    httpClient.run();
    return 0;
}
