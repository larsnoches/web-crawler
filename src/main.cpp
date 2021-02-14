/**********************************************
*
* Copyright 2021 Cyril Selyanin
* cyr.selyanin@gmail.com
* https://github.com/larsnoches/web-crawler
*
*/

#include <iostream>
#include "httpclient.h"
#include "util.h"

using namespace std;

int main(int argc, char* argv[])
{
    cout << "First argument is a target web-site URL." << endl;
    cout << "Second argument is a path to folder on local disk for saving visited web-pages." << endl;

    try
    {
        string url = "https://nodejs.org/docs/latest-v13.x/api/addons.html";
        string appPath = argv[0];
        Util::substitute(appPath, "web-crawler.exe", "");
        string folderToSave = appPath;
#ifdef WIN32
        folderToSave += "\\saved";
#else
        folderToSave += "/saved";
#endif
        if (argc == 3)
        {
            url = argv[1];
            string folderSt = argv[2];
            if (Util::isPathExist(folderSt)) folderToSave = folderSt;
        }
        cout << "Working with url:" << url << endl;

        if (Util::isPathExist(folderToSave) == false)
        {
            cout << "Error. You should enter an exists path to folder for web-pages saving." << endl;
            return -1;
        }
        cout << "Path to save:" << folderToSave << endl;

        HttpClient httpClient(url, folderToSave);
        httpClient.start();
    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }
    return 0;
}
