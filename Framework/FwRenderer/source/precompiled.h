/**
* @file precompiled.h
* @author Kamai Masayoshi
*/
#ifndef PRECOMPILED_H_
#define PRECOMPILED_H_

#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>

#if defined(WIN32) || defined(WIN64)
	#define WIN32_LEAN_AND_MEAN     // Windows ヘッダーから使用されていない部分を除外します。
	#define NOSHLWAPI               // Shlwapi.hを読み込まないように
	#include <Windows.h>
	#include <atlbase.h>
#endif

#include <algorithm>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "fw_core.h"

#endif  // PRECOMPILED_H_
