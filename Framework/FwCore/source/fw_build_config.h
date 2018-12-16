/**
 * @file fw_build_config.h
 */
#ifndef FW_BUILD_CONFIG_H_
#define FW_BUILD_CONFIG_H_

//! FwAssert �������I�ɗL����
#define FW_BUILD_CONFIG_FORCE_ENABLE_ASSERT             (0)

//! FwThread �̓��������������I�� std::thread �ɂ���
#define FW_BUILD_CONFIG_FORCE_USE_STD_THREAD            (0)

//! �t�@�C������̓��������������I�ɕW��C�֐��ɂ���
#define FW_BUILD_CONFIG_FORCE_USE_LIBC_FILE             (0)

#endif  // FW_BUILD_CONFIG_H_
