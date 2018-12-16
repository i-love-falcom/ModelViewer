/**
 * @file fw_build_config.h
 */
#ifndef FW_BUILD_CONFIG_H_
#define FW_BUILD_CONFIG_H_

//! FwAssert を強制的に有効化
#define FW_BUILD_CONFIG_FORCE_ENABLE_ASSERT             (0)

//! FwThread の内部実装を強制的に std::thread にする
#define FW_BUILD_CONFIG_FORCE_USE_STD_THREAD            (0)

//! ファイル操作の内部実装を強制的に標準C関数にする
#define FW_BUILD_CONFIG_FORCE_USE_LIBC_FILE             (0)

#endif  // FW_BUILD_CONFIG_H_
