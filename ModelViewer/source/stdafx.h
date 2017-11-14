// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include "targetver.h"

#if defined(WIN32) || defined(WIN64)
    #define WIN32_LEAN_AND_MEAN     // Windows ヘッダーから使用されていない部分を除外します。
    #define NOSHLWAPI               // Shlwapi.hを読み込まないように
    #include <Windows.h>
    #include <atlbase.h>
#endif

// C ランタイム ヘッダー ファイル
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "framework.h"


// TODO: プログラムに必要な追加ヘッダーをここで参照してください
