/**
 * @file fw_task.h
 * @author Kamai Masayoshi
 */
#ifndef FW_TASK_H_
#define FW_TASK_H_

#include "container/fw_vector.h"


BEGIN_NAMESPACE_FW

enum class FwTaskState : sint32_t {
    kInit,              ///< �������
    kWaitingToRun,      ///< �X�P�W���[��������s�҂����
    kRunning,           ///< ���s���
    kRanToCompletion,   ///< ���s�������
};

enum class FwTaskGroupState : sint32_t {
    kInit,                          ///< �������
    kReady,                         ///< �^�X�N�󂯓���\���
    kWaitingToRun,                  ///< �^�X�N�����o������s�҂����
    kRunning,                       ///< ���s���
    kWaitingForChildrenToComplete,  ///< �q�^�X�N�̊����҂�
    kRanToCompletion,               ///< ���s�������
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
    sint32_t            numSubmittedTasks;              ///< ���o�����S�^�X�N��
    sint32_t            numSubmittedChildGroups;        ///< ���o�����q�O���[�v��
    std::atomic_int32_t numRanToCompletionTasks;        ///< ���������^�X�N��
    std::atomic_int32_t numRanToCompletionChildGroups;  ///< ���������q�O���[�v��

    /**
     * @brief ������
     */
    void Init(FwTaskGroup * taskGroup, const sint32_t numTasks, const sint32_t numChildren);

    /**
     * @brief �^�X�N���s�����L�҂ɒʒm����
     */
    void NotifyOwnerToRun();

    /**
     * @brief �^�X�N���������L�҂ɂ�ʒm����
     */
    void NotifyOwnerToCompletion();

    /**
     * @name ���Z�q
     */
    //! @{
    const FwTaskGroupSharedData & operator =(const FwTaskGroupSharedData & value);
    FwTaskGroupSharedData & operator =(FwTaskGroupSharedData && value);
    //! @}

    /**
     * @name �R���X�g���N�^
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
     * @brief ��Ԃ��擾
     */
    FW_INLINE FwTaskState GetState() const { return state; }

    /**
     * @brief �^�X�N�ԍ����擾
     */
    FW_INLINE sint32_t GetIndex() const { return index; }

    /**
     * @brief ���O���擾
     */
    FW_INLINE str_t GetName() const { return name; }
    
    /**
     * @name ���Z�q
     */
    //! @{
    const FwTask & operator =(const FwTask & value);
    FwTask & operator =(FwTask && value);
    //! @}

    /**
     * @name �R���X�g���N�^
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
     * @brief �^�X�N�ǉ�
     * @param[in] args �^�X�N�ɓn�����[�U����
     * @param[in] func �^�X�N�̃G���g���|�C���g
     */
    FwTask * AddTask(FwTaskArgType * args, FwTaskFunctionType & func);
    
    /**
     * @brief �^�X�N�ǉ�
     * @param[in] func �^�X�N�̃G���g���|�C���g
     */
    FW_INLINE FwTask * AddTask(FwTaskFunctionType & func) { return AddTask(nullptr, func); }

    /**
     * @brief �o�^�����^�X�N���X�P�W���[���֑��o
     */
    void Submit();

    /**
     * @brief �S�Ẵ^�X�N���I������܂ő҂�
     */
    void Wait();

    /**
     * @brief �������\�[�X���J�����čēx��t�\��Ԃɂ���
     */
    void Reset();

    /**
     * @brief �s�v�ȓ������\�[�X���J�����čēx��t�\��Ԃɂ���
     * @param[in] _maxTasks    �ێ��\�ȍő�^�X�N��
     * @param[in] _maxChildren �ێ��\�ȍő�q�O���[�v��
     * @param[in] _maxChains   �ێ��\�ȍő�`�F�C���O���[�v��
     */
    void Reset(const uint32_t _maxTasks, const uint32_t _maxChildren, const uint32_t _maxChains);

    /**
     * @brief ���̃^�X�N�̏�ԁiFwTaskGroupState�j���擾
     * @param[in] order �������I�[�_�[
     */
    FW_INLINE FwTaskGroupState GetState(const std::memory_order order = std::memory_order_seq_cst) const { return state.load(order); }

    /**
     * @brief ���
     */
    const FwTaskGroup & operator =(const FwTaskGroup & value);

    /**
     * @brief �ړ�
     */
    FwTaskGroup & operator =(FwTaskGroup && value);


private:
    /**
     * @brief ���̃^�X�N�̏�ԁiFwTaskGroupState�j��ݒ肵�܂�
     * @param[in] state �ݒ肷����
     */
    FW_INLINE void SetState(const FwTaskGroupState newState, const std::memory_order order = std::memory_order_seq_cst) { return state.store(newState, order); }

    /**
     * @brief ���̃^�X�N�̏�ԁiFwTaskGroupState�j��ύX���܂�
     * @param[in] state �ݒ肷����
     */
    void ChangeState(const FwTaskGroupState newState, const std::memory_order order = std::memory_order_seq_cst);

    /**
     * @name �R���X�g���N�^
     */
    //! @{
    FwTaskGroup();
    FwTaskGroup(const uint32_t _maxTasks, const uint32_t _maxChildren, const uint32_t _maxChains, const str_t _name);
    FwTaskGroup(const FwTaskGroup & value);
    FwTaskGroup(FwTaskGroup && value);
    //! @}

    /**
     * @brief �f�X�g���N�^
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
