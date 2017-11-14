// ModelViewer.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "app_wndproc.h"
#include "app.h"

#include <shellapi.h>


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DROPFILES:
        {
            HDROP hdrop = reinterpret_cast<HDROP>(wParam);

            DropFileArgs args;
            DragQueryPoint(hdrop, &args.dropPoint);

            const UINT numFiles = DragQueryFile(hdrop, -1, NULL, 0);
            args.dropFiles.reserve(numFiles);

            DragFinish(hdrop);

            for (UINT i = 0; i < numFiles; ++i) {
                const UINT num = DragQueryFile(hdrop, i, NULL, 0) + 1;
                args.dropFiles[i].resize(num);

                DragQueryFile(hdrop, i, &args.dropFiles[i].front(), num);
            }

            App::Instance().OnDropFiles(&args);
        }
        break;
    case WM_CHAR:
    case WM_DEADCHAR:
        {
        }
        break;
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        {
            // マウス左ボタン
        }
        break;
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        {
            // マウス右ボタン
        }
        break;
    case WM_MBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        {
            // マウス中央ボタン
        }
        break;
    case WM_MOUSEMOVE:
        {
            // マウス移動
        }
        break;
    case WM_MOUSEWHEEL:
        {
            // マウスホイール
        }
        break;
    case WM_SETFOCUS:
        {
            App::Instance().OnSetFocus(nullptr);
        }
        break;
    case WM_KILLFOCUS:
        {
            App::Instance().OnKillFocus(nullptr);
        }
        break;
    case WM_ACTIVATE:
        {
            const int active = LOWORD(wParam);

            if (active == WA_INACTIVE) {
                App::Instance().OnSuspend(nullptr);
            } else {
                App::Instance().OnResume(nullptr);
            }
        }
        break;
    case WM_SIZE:
        {
            if (wParam == SIZE_RESTORED) {
                App::Instance().clientWidth = LOWORD(lParam);
                App::Instance().clientHeight = HIWORD(lParam);
            }
        }
        break;
    case WM_MOVE:
        {
            // ウィンドウ移動
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
