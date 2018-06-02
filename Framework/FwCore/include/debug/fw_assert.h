/**
 * @file fw_assert.h
 * @author Kamai Masayoshi
 */
#ifndef FW_ASSERT_H_
#define FW_ASSERT_H_

#if !defined(FW_NDEBUG) || FW_BUILD_CONFIG_FORCE_ENABLE_ASSERT
    #define FW_ENABLE_ASSERT        (1)
#else
    #define FW_ENABLE_ASSERT        (0)
#endif

#if FW_ENABLE_ASSERT
    #if defined(FW_PLATFORM_WIN32)
        #define abort_process()     {DebugBreak();}
    #else
        #define abort_process()     {while(true) {}}
    #endif

    
FW_INLINE void fwassert(const bool expr, const char * msg, const char * file, int line) {
    static bool s_skipAssertion = false;
    if (s_skipAssertion) {
        return;
    }
    if (!expr) {
        NAMESPACE_FW DebugLog::Error((str_t)(_T("Assertion failed: %s %d : %s\n")), file, line, msg);
        abort_process();
    }
}
    #define FwAssertMessage(expr__, msg__)      fwassert((expr__), msg__, __FILE__, __LINE__)
    #define FwAssert(expr__)                    fwassert((expr__), # expr__, __FILE__, __LINE__)
#else   // not FW_DEBUG
    #define FwAssert(exp__)
    #define FwAssertMessage(exp__, msg__)
#endif  // not FW_DEBUG

#define assert_align4(ptr__)                            FwAssert(((ptr__) & 0x3) == 0)
#define assert_align8(ptr__)                            FwAssert(((ptr__) & 0x7) == 0)
#define assert_align16(ptr__)                           FwAssert(((ptr__) & 0xf) == 0)
#define assert_align32(ptr__)                           FwAssert(((ptr__) & 0x1f) == 0)
#define assert_align64(ptr__)                           FwAssert(((ptr__) & 0x3f) == 0)
#define assert_align128(ptr__)                          FwAssert(((ptr__) & 0x7f) == 0)
#define assert_align256(ptr__)                          FwAssert(((ptr__) & 0xff) == 0)

#define compile_time_assert(expr__)                     static_assert(expr__, # expr__);
#define compile_time_assert_message(expr__, msg__)      static_assert(expr__, msg__);


#endif  // FW_ASSERT_H_
