#include <iostream>
#include "httpclient.h"

using namespace std;

int main()
{
    string url = "http://www.google.ru";
    HttpClient httpClient(url);
//    httpClient.run();
    return 0;
}
