/**
 * @file core_file_win32.cpp
 * @author Kamai Masayoshi
 */
#include "precompiled.h"
#include "file/fw_file.h"
#include "file/fw_file_types.h"
#include "file/fw_path.h"

BEGIN_NAMESPACE_FW

sint32_t FwFileOpen(const str_t name, const uint32_t options, FwFile & fp) {
    if (name == nullptr || (options & kFileOptAccessMask) == 0) {
        return ERR_INVALID_PARMS;
    }
    
#if FW_FILE == FW_FILE_WIN32
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

    auto nativeHandle = CreateFile(
        name,
        desiredAccess,
        sharedModel,
        NULL,   // security attributes
        creationDisposition,
        flagsAndAttr,
        NULL);
    
    if (nativeHandle == INVALID_HANDLE_VALUE) {
        return ERR_INVALID;
    }
#else
    tstring opt;

    if ((options & kFileOptAccessRW) == kFileOptAccessRW) {
        opt += _T("a+b");
    } else if ((options & kFileOptAccessRead) != 0) {
        opt += _T("rb");
    } else if ((options & kFileOptAccessWrite) != 0) {
        opt += _T("w+b");
    }
    if ((options & kFileOptFlagRandomAccess) != 0) {
        opt += _T("R");
    } else if ((options & kFileOptFlagSequential) != 0) {
        opt += _T("S");
    }
    if ((options & kFileOptFlagDeleteOnClose) != 0) {
        opt += _T("D");
    }
    if ((options & kFileOptAttributeTemporary) != 0) {
        opt += _T("T");
    }

    FILE * nativeHandle = nullptr;
    auto errorNo = _tfopen_s(&nativeHandle, name, opt.c_str());
    if (errorNo != 0) {
        return ERR_INVALID;
    }
#endif

    fp.nativeHandle = nativeHandle;
    fp.options = options;

    return FW_OK;
}

sint32_t FwFileClose(FwFile & fp) {
    if (fp.nativeHandle == nullptr) {
        return ERR_INVALID_PARMS;
    }
    if ((fp.options & kFileOptFlagNoClose) == 0) {
        auto nativeHandle = fp.nativeHandle;

        fp.nativeHandle = nullptr;
        fp.options = 0;

#if FW_FILE == FW_FILE_WIN32
        CloseHandle(nativeHandle);
#else
        fclose(nativeHandle);
#endif
    }
    return FW_OK;
}

sint32_t FwFileDelete(const str_t name) {
    if (name == nullptr) {
        return ERR_INVALID_PARMS;
    }
#if FW_FILE == FW_FILE_WIN32
    if (::DeleteFile(name) != 0) {
        return ERR_FAILED;
    }
#else
    if (_tremove(name) != 0) {
        return ERR_FAILED;
    }
#endif
    return FW_OK;
}

sint32_t FwFileRead(FwFile & fp, void * dst, const uint64_t toReadSize, uint64_t * readSize) {
    if (fp.nativeHandle == nullptr || dst == nullptr || toReadSize == 0) {
        return ERR_INVALID_PARMS;
    }
    
    uint64_t readOffset = 0;
#if FW_FILE == FW_FILE_WIN32
    for (;;) {
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
        *readSize = readOffset;
    }
    if (readOffset == 0) {
        return ERR_EOF;
    }
#else
    size_t numReads = fread(dst, 1, toReadSize, fp.nativeHandle);
    if (numReads < toReadSize) {
        if (ferror(fp.nativeHandle)) {
            return ERR_INVALID;
        }
        if (numReads == 0 && feof(fp.nativeHandle)) {
            return ERR_EOF;
        }
    }
    if (readSize != nullptr) {
        *readSize = numReads;
    }
#endif
    return FW_OK;
}

sint32_t FwFileWrite(FwFile & fp, const void * src, const uint64_t toWriteSize, uint64_t * writeSize) {
    if (fp.nativeHandle == nullptr || src == nullptr || toWriteSize == 0) {
        return ERR_INVALID_PARMS;
    }
    
    uint64_t writeOffset = 0;
#if FW_FILE == FW_FILE_WIN32
    for (;;) {
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
        *writeSize = writeOffset;
    }
#elif FW_FILE == FW_FILE_LIBC
    size_t numWrites = fwrite(src, 1, toWriteSize, fp.nativeHandle);
    if (writeSize != nullptr) {
        *writeSize = static_cast<uint64_t>(numWrites);
    }
#endif
    return FW_OK;
}

sint32_t FwFileGetLengthByName(const str_t name, uint64_t * length) {
    if (length == nullptr) {
        return ERR_INVALID_PARMS;
    }
    
    FwFile fp;
    auto result = FwFileOpen(name, kFileOptAccessRead | kFileOptSharedRead, fp);
    if (result != FW_OK) {
        return result;
    }

    result = FwFileGetLength(fp, length);

    FwFileClose(fp);

    return result;
}

sint32_t FwFileGetLength(FwFile & fp, uint64_t * length) {
    if (length == nullptr) {
        return ERR_INVALID_PARMS;
    }

#if FW_FILE == FW_FILE_WIN32
    if (fp.nativeHandle == INVALID_HANDLE_VALUE) {
        return ERR_INVALID;
    }

    LARGE_INTEGER len;
    if (!GetFileSizeEx(fp.nativeHandle, &len)) {
        return ERR_FILE_NOEXIST;
    }
    *length = static_cast<uint64_t>(len.QuadPart);
#elif FW_FILE == FW_FILE_LIBC
    if (fp.nativeHandle == nullptr) {
        return ERR_INVALID;
    }

    fseek(fp.nativeHandle, 0, SEEK_END);

    fpos_t len;
    fgetpos(fp.nativeHandle, &len);

    fseek(fp.nativeHandle, 0, SEEK_SET);

    *length = static_cast<uint64_t>(len);
#endif

    return FW_OK;
}

bool FwFileIsExist(const str_t name) {
    if (name == nullptr) {
        return false;
    }

#if FW_FILE == FW_FILE_WIN32
    HANDLE nativeHandle = CreateFile(
        name,
        0,
        FILE_SHARE_READ,
        nullptr,    // security attributes
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    
    if (nativeHandle == INVALID_HANDLE_VALUE) {
        return false;
    }
    CloseHandle(nativeHandle);
#elif FW_FILE == FW_FILE_LIBC
    FILE * nativeHandle = nullptr;
    auto errorNo = _tfopen_s(&nativeHandle, name, _T("rb"));
    if (errorNo != 0) {
        return false;
    }
    fclose(nativeHandle);
#endif

    return true;
}

sint32_t FwFileSeek(FwFile & fp, const sint64_t offset, const FwSeekOrigin origin) {
    if (fp.nativeHandle == nullptr) {
        return ERR_INVALID_PARMS;
    }
    
#if FW_FILE == FW_FILE_WIN32
    LARGE_INTEGER liDistanceToMove;
    liDistanceToMove.QuadPart = offset;
    
    uint32_t oriTbl[] = {FILE_BEGIN, FILE_CURRENT, FILE_END};
    SetFilePointerEx(fp.nativeHandle, liDistanceToMove, NULL, oriTbl[origin]);
#elif FW_FILE == FW_FILE_LIBC
    int oriTbl[] = { SEEK_SET, SEEK_CUR, SEEK_END };
    _fseeki64(fp.nativeHandle, 0, oriTbl[origin]);
#endif

    return FW_OK;
}

sint32_t FwFileFlushBuffer(FwFile & fp) {
    if (fp.nativeHandle == nullptr) {
        return ERR_INVALID_PARMS;
    }
    
#if FW_FILE == FW_FILE_WIN32
    FlushFileBuffers(fp.nativeHandle);
#elif FW_FILE == FW_FILE_LIBC
    fflush(fp.nativeHandle);
#endif

    return FW_OK;
}

END_NAMESPACE_FW

