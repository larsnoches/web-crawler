#include <iostream>
#include "httpclient.h"

using namespace std;

int main()
{
//    cout << "Hello World!" << endl;
    string url = "http://yandex.ru/";
    HttpClient httpClient(url);
    httpClient.run();
    return 0;
}
