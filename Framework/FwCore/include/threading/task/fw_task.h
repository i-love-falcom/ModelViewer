/**
 * @file fw_task.h
 * @author Kamai Masayoshi
 */
#ifndef FW_TASK_H_
#define FW_TASK_H_

#include "container/fw_vector.h"


BEGIN_NAMESPACE_FW

enum class FwTaskState : sint32_t {
    kInit,              ///< 初期状態
    kWaitingToRun,      ///< スケジュールされ実行待ち状態
    kRunning,           ///< 実行状態
    kRanToCompletion,   ///< 実行完了状態
};

enum class FwTaskGroupState : sint32_t {
    kInit,                          ///< 初期状態
    kReady,                         ///< タスク受け入れ可能状態
    kWaitingToRun,                  ///< タスクが送出され実行待ち状態
    kRunning,                       ///< 実行状態
    kWaitingForChildrenToComplete,  ///< 子タスクの完了待ち
    kRanToCompletion,               ///< 実行完了状態
};


using FwTaskArgType         = void *;
using FwTaskRetType         = void;
using FwTaskFunctionType    = std::function<FwTaskRetType(FwTaskArgType)>;

class FwTask;
class FwTaskGroup;
class FwTaskFactory;


/**
 * @class
 */
class FwTaskGroupSharedData {
public:
    FwTaskGroup *       owner;
    sint32_t            numSubmittedTasks;              ///< 送出した全タスク数
    sint32_t            numSubmittedChildGroups;        ///< 送出した子グループ数
    std::atomic_int32_t numRanToCompletionTasks;        ///< 完了したタスク数
    std::atomic_int32_t numRanToCompletionChildGroups;  ///< 完了した子グループ数

    /**
     * @brief 初期化
     */
    void Init(FwTaskGroup * taskGroup, const sint32_t numTasks, const sint32_t numChildren);

    /**
     * @brief タスク実行を所有者に通知する
     */
    void NotifyOwnerToRun();

    /**
     * @brief タスク完了を所有者にを通知する
     */
    void NotifyOwnerToCompletion();

    /**
     * @name 演算子
     */
    //! @{
    const FwTaskGroupSharedData & operator =(const FwTaskGroupSharedData & value);
    FwTaskGroupSharedData & operator =(FwTaskGroupSharedData && value);
    //! @}

    /**
     * @name コンストラクタ
     */
    //! @{
    FwTaskGroupSharedData();
    FwTaskGroupSharedData(const FwTaskGroupSharedData & value);
    FwTaskGroupSharedData(FwTaskGroupSharedData && value);
    //! @}
};

/**
 * @class FwTask
 */
class FwTask {
public:
    /**
     * @brief 状態を取得
     */
    FW_INLINE FwTaskState GetState() const { return state; }

    /**
     * @brief タスク番号を取得
     */
    FW_INLINE sint32_t GetIndex() const { return index; }

    /**
     * @brief 名前を取得
     */
    FW_INLINE str_t GetName() const { return name; }
    
    /**
     * @name 演算子
     */
    //! @{
    const FwTask & operator =(const FwTask & value);
    FwTask & operator =(FwTask && value);
    //! @}

    /**
     * @name コンストラクタ
     */
    //! @{
    FwTask();
    FwTask(const FwTask & value);
    FwTask(FwTask && value);
    //! @}

private:
    FwTaskState             state;
    FwTaskGroupSharedData * sharedData;

    FwTaskFunctionType  func;
    FwTaskArgType *     args;

    sint32_t    index;
    str_t       name;

    friend class FwTaskGroup;
    friend class FwTaskFactory;
};

/**
 * @class FwTaskGroup
 */
class FwTaskGroup {
public:
    /**
     * @brief タスク追加
     * @param[in] args タスクに渡すユーザ引数
     * @param[in] func タスクのエントリポイント
     */
    FwTask * AddTask(FwTaskArgType * args, FwTaskFunctionType & func);
    
    /**
     * @brief タスク追加
     * @param[in] func タスクのエントリポイント
     */
    FW_INLINE FwTask * AddTask(FwTaskFunctionType & func) { return AddTask(nullptr, func); }

    /**
     * @brief 登録したタスクをスケジューラへ送出
     */
    void Submit();

    /**
     * @brief 全てのタスクが終了するまで待つ
     */
    void Wait();

    /**
     * @brief 内部リソースを開放して再度受付可能状態にする
     */
    void Reset();

    /**
     * @brief 不要な内部リソースを開放して再度受付可能状態にする
     * @param[in] _maxTasks    保持可能な最大タスク数
     * @param[in] _maxChildren 保持可能な最大子グループ数
     * @param[in] _maxChains   保持可能な最大チェイングループ数
     */
    void Reset(const uint32_t _maxTasks, const uint32_t _maxChildren, const uint32_t _maxChains);

    /**
     * @brief このタスクの状態（FwTaskGroupState）を取得
     * @param[in] order メモリオーダー
     */
    FW_INLINE FwTaskGroupState GetState(const std::memory_order order = std::memory_order_seq_cst) const { return state.load(order); }

    /**
     * @brief 代入
     */
    const FwTaskGroup & operator =(const FwTaskGroup & value);

    /**
     * @brief 移動
     */
    FwTaskGroup & operator =(FwTaskGroup && value);


private:
    /**
     * @brief このタスクの状態（FwTaskGroupState）を設定します
     * @param[in] state 設定する状態
     */
    FW_INLINE void SetState(const FwTaskGroupState newState, const std::memory_order order = std::memory_order_seq_cst) { return state.store(newState, order); }

    /**
     * @brief このタスクの状態（FwTaskGroupState）を変更します
     * @param[in] state 設定する状態
     */
    void ChangeState(const FwTaskGroupState newState, const std::memory_order order = std::memory_order_seq_cst);

    /**
     * @name コンストラクタ
     */
    //! @{
    FwTaskGroup();
    FwTaskGroup(const uint32_t _maxTasks, const uint32_t _maxChildren, const uint32_t _maxChains, const str_t _name);
    FwTaskGroup(const FwTaskGroup & value);
    FwTaskGroup(FwTaskGroup && value);
    //! @}

    /**
     * @brief デストラクタ
     */
    ~FwTaskGroup();

    FwTaskFactory *                     owner;
    std::atomic<FwTaskGroupState>       state;
    FwTaskGroupSharedData               sharedData;

    NAMESPACE_FW vector<FwTask>         taskPool;
    NAMESPACE_FW vector<FwTaskGroup *>  childGroups;
    NAMESPACE_FW vector<FwTaskGroup *>  chainGroups;

    static const sint32_t kMaxNameLen = 63;
    char_t  name[kMaxNameLen + 1];

    friend class FwTask;
    friend class FwTaskGroupSharedData;
};




END_NAMESPACE_FW

#endif  // FW_TASK_H_
