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

#include "core/fw_build_config.h"
#include "core/fw_define.h"
#include "core/fw_error.h"
#include "core/fw_types.h"
#include "core/fw_allocator.h"

#include "debug/fw_debug_log.h"
#include "debug/fw_assert.h"

#include "misc/fw_noncopyable.h"

#include "container/fw_container_allocator.h"
#include "container/fw_array.h"
#include "container/fw_deque.h"
#include "container/fw_string.h"
#include "container/fw_vector.h"

#endif  // PRECOMPILED_H_
