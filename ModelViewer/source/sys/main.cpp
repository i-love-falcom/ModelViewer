/**
 * @file main.cpp
 * @author Kamai Masayoshi
 */
#include "stdafx.h"
#include "sys/main.h"
#include "sys/app.h"

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR lpCmdLine,
    _In_ int nCmdShow) {

    AppParms parms;
    parms.hInstance     = hInstance;
    parms.hPrevInstance = hPrevInstance;
    parms.cmdShow       = nCmdShow;


    CommandLineArgs args;
    //! @todo コマンドライン引数をパース

    App::Instance().Initialize(parms, args);
    int ret = App::Instance().Run();
    App::Instance().Shutdown();

    return ret;
}

