/**
 * @file main_thread.cpp
 */
#include "stdafx.h"
#include "main_thread.h"


USING_NAMESPACE_FW

namespace {
/**
 * @struct MainThreadArgs
 */
struct MainThreadArgs {
    bool            isTerminate;
    MainThread *    thread;

    void Init(MainThread * mainThread) {
        isTerminate = false;
        thread = mainThread;
    }
};

}   // namespace ""


MainThread::MainThread() {

}

MainThread::~MainThread() {

}

sint32_t MainThread::Invoke(void * userArgs) {
    // 初期化
    sint32_t result = Initialzie(userArgs);
    if (result != FW_OK) {
        return result;
    }

    MainThreadArgs * args = reinterpret_cast<MainThreadArgs *>(userArgs);
    MainThread * thread = args->thread;

    // メインループ
    do {
        thread->OnStartFrame();

        thread->OnFrameUpdate(0.0f);

        thread->OnEndFrame();
    } while (!args->isTerminate);

    // 終了処理
    Shutdown();

    return result;
}

sint32_t MainThread::Initialzie(void * userArgs) {
    MainThreadArgs * args = reinterpret_cast<MainThreadArgs *>(userArgs);

    return FW_OK;
}

void MainThread::Shutdown() {

}

void MainThread::StartMainThread(const str_t name) {
    args = FwMalloc(sizeof(MainThreadArgs), defaultFwMallocAttribute);
    assert(args != nullptr);

    reinterpret_cast<MainThreadArgs *>(args)->Init(this);

    ThreadDesc desc;
    desc.Init();
    desc.userArgs   = args;
    desc.stackSize  = MainThread::kDefaultStackSize;
    desc.priority   = kTheadPriorityNormal;
    desc.affinity   = DefaultThreadAffinity;
    tstring::SPrintf(desc.name, ARRAY_SIZEOF(desc.name), name);

    Start(&desc);
}

void MainThread::TerminateMainThread() {
    reinterpret_cast<MainThreadArgs *>(args)->isTerminate = true;
    this->TerminateThread();
    WaitThread();
}

void MainThread::OnStartFrame() {

}

void MainThread::OnFrameUpdate(const float delta) {

}

void MainThread::OnEndFrame() {

}