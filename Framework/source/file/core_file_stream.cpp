/**
 * @file core_file_stream.cpp
 * @author Kamai Masayoshi
 */
#include "stdafx.h"
#include "system/file/core_file_stream.h"

namespace core {
FileStream::FileStream() {
}

FileStream::~FileStream() {
}

CORE_RESULT FileStream::Read(void * dst, const sint64_t dstSize, const sint64_t dstOffset, const sint64_t readSize) {
    return DoRead(dst, dstSize, dstOffset, readSize, nullptr, nullptr);
}

CORE_RESULT FileStream::Read(void * dst, const sint64_t dstSize, const sint64_t dstOffset, const sint64_t readSize, FileStreamEndCallback callback, void * callbackParms) {
    return DoRead(dst, dstSize, dstOffset, readSize, callback, callbackParms);
}

CORE_RESULT FileStream::Write(const void * src, const sint64_t srcSize, const sint64_t srcOffset, const sint64_t writeSize) {
    return DoWrite(src, srcSize, srcOffset, writeSize, nullptr, nullptr);
}
    
CORE_RESULT FileStream::Write(const void * src, const sint64_t srcSize, const sint64_t srcOffset, const sint64_t writeSize, FileStreamEndCallback callback, void * callbackParms) {
    return DoWrite(src, srcSize, srcOffset, writeSize, callback, callbackParms);
}
    
void FileStream::Close() {
    DoClose();
}
    
CORE_RESULT FileStream::Seek(const sint64_t offset, const SreamSeekOrigin origin) {
    return DoSeek(offset, origin);
}
    
sint64_t FileStream::Length() {
    return DoLength();
}

CORE_RESULT FileStream::Wait(const uint32_t milliseconds) {
    return DoWait(milliseconds);
}

}   // namespace "core"
