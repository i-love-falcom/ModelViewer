/**
 * @file fw_noncopyable.h
 * @author Kamai Masayoshi
 */
#ifndef FW_NONCOPYABLE_H_
#define FW_NONCOPYABLE_H_

BEGIN_NAMESPACE_FW

/**
 * @class NonCopyable
 */
template<class T>
class NonCopyable {
protected:
    NonCopyable() {}
    ~NonCopyable() {}
    
private:
    NonCopyable(const NonCopyable &) = delete;
    T & operator =(const T &) = delete;
};

END_NAMESPACE_FW

#endif  // FW_NONCOPYABLE_H_
