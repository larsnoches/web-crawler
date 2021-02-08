#include <iostream>
#include "httpclient.h"

using namespace std;

int main()
{
    try
    {
        string url = "https://nodejs.org/docs/latest-v13.x/api/addons.html";
        HttpClient httpClient(url, true);
        httpClient.start();
    //    httpClient.run();
    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }
    return 0;
}
