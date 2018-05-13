/**
 * @file core_file_win32.cpp
 * @author Kamai Masayoshi
 */
#include "precompiled.h"
#include "file/fw_file.h"
#include "file/fw_file_types.h"
#include "file/fw_path.h"

BEGIN_NAMESPACE_FW

sint32_t FwFileOpen(const str_t name, const sint32_t options, FwFile & fp) {
    if (name == nullptr || (options & kFileOptAccessMask) == 0) {
        return ERR_INVALID_PARMS;
    }
    
    DWORD desiredAccess = 0;
    DWORD sharedModel = 0;
    DWORD creationDisposition = 0;
    DWORD flagsAndAttr = 0;
    
    if ((options & kFileOptAccessRead) != 0) {
        desiredAccess |= GENERIC_READ;
    }
    if ((options & kFileOptAccessWrite) != 0) {
        desiredAccess |= GENERIC_WRITE;
    }

    if ((options & kFileOptAccessRW) == kFileOptAccessRW) {
        creationDisposition |= OPEN_ALWAYS;
    } else if ((options & kFileOptAccessRead) != 0) {
        creationDisposition |= OPEN_EXISTING;
    } else if ((options & kFileOptAccessWrite) != 0) {
        creationDisposition |= OPEN_ALWAYS;
    }

    if ((options & kFileOptSharedRead) != 0) {
        sharedModel |= FILE_SHARE_READ;
    }
    if ((options & kFileOptSharedWrite) != 0) {
        sharedModel |= FILE_SHARE_WRITE;
    }

    if ((options & kFileOptFlagRandomAccess) != 0) {
        flagsAndAttr |= FILE_FLAG_RANDOM_ACCESS;
    } else if ((options & kFileOptFlagSequential) != 0) {
        flagsAndAttr |= FILE_FLAG_SEQUENTIAL_SCAN;
    }
    if ((options & kFileOptFlagDeleteOnClose) != 0) {
        flagsAndAttr |= FILE_FLAG_DELETE_ON_CLOSE;
    }

    bool isAttrNormal = true;
    if ((options & kFileOptAttributeEncrypted) != 0) {
        flagsAndAttr |= FILE_ATTRIBUTE_ENCRYPTED;
        isAttrNormal = false;
    }
    if ((options & kFileOptAttributeHidden) != 0) {
        flagsAndAttr |= FILE_ATTRIBUTE_HIDDEN;
        isAttrNormal = false;
    }
    if ((options & kFileOptAttributeReadOnly) != 0) {
        flagsAndAttr |= FILE_ATTRIBUTE_READONLY;
        isAttrNormal = false;
    }
    if ((options & kFileOptAttributeTemporary) != 0) {
        flagsAndAttr |= FILE_ATTRIBUTE_TEMPORARY;
        flagsAndAttr |= FILE_ATTRIBUTE_NOT_CONTENT_INDEXED; // インデックスサービス対象外
        isAttrNormal = false;
    }
    if (isAttrNormal) {
        flagsAndAttr |= FILE_ATTRIBUTE_NORMAL;
    }

    HANDLE hFile = CreateFile(
        name,
        desiredAccess,
        sharedModel,
        NULL,   // security attributes
        creationDisposition,
        flagsAndAttr,
        NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        return ERR_INVALID;
    }

    fp.nativeHandle = hFile;
    fp.options = options;
    
    return FW_OK;
}

sint32_t FwFileClose(FwFile & fp) {
    if (fp.nativeHandle == nullptr) {
        return ERR_INVALID_PARMS;
    }
    if ((fp.options & kFileOptFlagNoClose) == 0) {
        HANDLE hFile = fp.nativeHandle;

        fp.nativeHandle = nullptr;
        fp.options = 0;

        CloseHandle(hFile);
    }
    return FW_OK;
}

sint32_t FwFileDelete(const str_t name) {
    if (name == nullptr) {
        return ERR_INVALID_PARMS;
    }
    if (::DeleteFile(name) != 0) {
        return ERR_FAILED;
    }
    return FW_OK;
}

sint32_t FwFileRead(FwFile & fp, void * dst, const uint64_t toReadSize, uint64_t * readSize) {
    if (fp.nativeHandle == nullptr || dst == nullptr || toReadSize == 0) {
        return ERR_INVALID_PARMS;
    }
    
    uint64_t readOffset = 0;
    while (true) {
        void * dstBuffer = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + readOffset);
        DWORD readSize = static_cast<DWORD>((toReadSize - readOffset) & 0xffffffff);

        DWORD numReads = 0;
        BOOL r = ReadFile(fp.nativeHandle, dstBuffer, readSize, &numReads, NULL);
        if (!r) {
            return ERR_INVALID;
        }

        readOffset += static_cast<uint64_t>(numReads);
        if (readOffset >= toReadSize) {
            break;
        }
    }
    if (readSize != nullptr) {
        *readSize = static_cast<uint64_t>(readOffset);
    }
    if (readOffset == 0) {
        return ERR_EOF;
    }
    return FW_OK;
}

sint32_t FwFileWrite(FwFile & fp, const void * src, const uint64_t toWriteSize, uint64_t * writeSize) {
    if (fp.nativeHandle == nullptr || src == nullptr || toWriteSize == 0) {
        return ERR_INVALID_PARMS;
    }
    
    uint64_t writeOffset = 0;
    while (true) {
        const void * srcBuffer = reinterpret_cast<const void *>(reinterpret_cast<uintptr_t>(src) + writeOffset);
        DWORD writeSize = static_cast<DWORD>((toWriteSize - writeOffset) & 0xffffffff);

        DWORD numWrites = 0;
        BOOL r = WriteFile(fp.nativeHandle, srcBuffer, writeSize, &numWrites, NULL);
        if (!r) {
            return ERR_INVALID;
        }

        writeOffset += static_cast<uint64_t>(numWrites);
        if (writeOffset >= toWriteSize) {
            break;
        }
    }
    if (writeSize != nullptr) {
        *writeSize = static_cast<uint64_t>(writeOffset);
    }
    return FW_OK;
}

sint32_t FwFileGetLength(const str_t name, uint64_t * length) {
    if (length == nullptr) {
        return ERR_INVALID_PARMS;
    }
    
    HANDLE hFile = CreateFile(
        name,
        0,
        0,          // no share
        nullptr,    // security attributes
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        return ERR_INVALID;
    }

    LARGE_INTEGER len;
    if (!GetFileSizeEx(hFile, &len)) {
        return ERR_FILE_NOEXIST;
    }
    *length = static_cast<uint64_t>(len.QuadPart);

    CloseHandle(hFile);

    return FW_OK;
}

bool FwFileIsExist(const str_t name) {
    if (name == nullptr) {
        return false;
    }

    HANDLE hFile = CreateFile(
        name,
        0,
        0,          // no share
        nullptr,    // security attributes
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }
    CloseHandle(hFile);
    
    return true;
}

sint32_t FwFileSeek(FwFile & fp, const sint64_t offset, const FwSeekOrigin origin) {
    if (fp.nativeHandle == nullptr) {
        return ERR_INVALID_PARMS;
    }
    
    LARGE_INTEGER liDistanceToMove;
    liDistanceToMove.QuadPart = offset;
    
    uint32_t oriTbl[] = {FILE_BEGIN, FILE_CURRENT, FILE_END};
    SetFilePointerEx(fp.nativeHandle, liDistanceToMove, NULL, oriTbl[origin]);
    
    return FW_OK;
}

sint32_t FwFileFlushBuffer(FwFile & fp) {
    if (fp.nativeHandle == nullptr) {
        return ERR_INVALID_PARMS;
    }
    
    FlushFileBuffers(fp.nativeHandle);
    
    return 0;
}

FwFile FwFileGetStdFileHandle(const FwStdDeviceHandle handle) {
    FwFile fp;
    fp.options |= kFileOptFlagNoClose;

#if defined(FW_PLATFORM_WIN32)
    switch (handle) {
    case kStdInputDeviceHandle:    fp.nativeHandle = GetStdHandle(STD_INPUT_HANDLE); break;
    case kStdOutputDeviceHandle:   fp.nativeHandle = GetStdHandle(STD_OUTPUT_HANDLE); break;
    case kStdErrorDeviceHandle:    fp.nativeHandle = GetStdHandle(STD_ERROR_HANDLE); break;
    }
#else
    switch (handle) {
    case kStdInputDeviceHandle:    fp.fileHandle = stdin; break;
    case kStdOutputDeviceHandle:   fp.fileHandle = stdout; break;
    case kStdErrorDeviceHandle:    fp.fileHandle = stderr; break;
    }
#endif
    return fp;
}

END_NAMESPACE_FW

