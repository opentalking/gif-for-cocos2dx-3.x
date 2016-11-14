#include "main.h"
#include "AppDelegate.h"
// Cocos2dx
#include "platform/CCFileUtils.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(nCmdShow);
    CC_UNUSED(lpCmdLine);

    // Create the application instance
    AppDelegate app;
    std::vector<std::string> paths = cocos2d::FileUtils::getInstance()->getSearchPaths();
    paths.push_back("./Resources");
    paths.push_back(cocos2d::FileUtils::getInstance()->getWritablePath());
    cocos2d::FileUtils::getInstance()->setSearchPaths(paths);

    return cocos2d::Application::getInstance()->run();
}
