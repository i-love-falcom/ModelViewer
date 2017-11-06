/**
 * @file core_file_win32.cpp
 * @author Kamai Masayoshi
 */
#include "precompiled.h"
#include "file/fw_file.h"
#include "file/fw_file_types.h"
#include "file/fw_path.h"

BEGIN_NAMESPACE_FW

sint32_t FileOpen(const str_t name, const sint32_t options, file_t & fp) {
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

    fp.fileHandle = hFile;
    fp.options = options;
    
    return FW_OK;
}

sint32_t FileClose(file_t & fp) {
    if (fp.fileHandle == nullptr) {
        return ERR_INVALID_PARMS;
    }

    HANDLE hFile = fp.fileHandle;
    
    fp.fileHandle = nullptr;
    fp.options = 0;
    
    CloseHandle(hFile);
    
    return FW_OK;
}

sint32_t FileDelete(const str_t name) {
    if (name == nullptr) {
        return ERR_INVALID_PARMS;
    }
    if (::DeleteFile(name) != 0) {
        return ERR_FAILED;
    }
    return FW_OK;
}

sint32_t FileRead(file_t & fp, void * dst, const sint64_t toReadSize, sint64_t * readSize) {
    if (fp.fileHandle == nullptr || dst == nullptr || toReadSize <= 0) {
        return ERR_INVALID_PARMS;
    }
    
    DWORD numReads = 0;
    BOOL r = ReadFile(fp.fileHandle, dst, static_cast<DWORD>(toReadSize), &numReads, NULL);
    if (!r) {
        return ERR_INVALID;
    }
    if (readSize != nullptr) {
        *readSize = static_cast<sint64_t>(numReads);
    }
    if (numReads == 0) {
        return ERR_EOF;
    }
    return FW_OK;
}

sint32_t FileWrite(file_t & fp, const void * src, const sint64_t toWriteSize, sint64_t * writeSize) {
    if (fp.fileHandle == nullptr || src == nullptr || toWriteSize <= 0) {
        return ERR_INVALID_PARMS;
    }
    
    DWORD numWrites = 0;
    BOOL r = WriteFile(fp.fileHandle, src, static_cast<DWORD>(toWriteSize), &numWrites, NULL);
    if (!r) {
        return ERR_INVALID;
    }
    if (writeSize != nullptr) {
        *writeSize = static_cast<sint64_t>(numWrites);
    }
    return FW_OK;
}

sint32_t GetFileLength(const str_t name, uint64_t * length) {
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

bool IsFileExist(const str_t name) {
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

sint32_t FileSeek(file_t & fp, const sint64_t offset, const SeekOrigin origin) {
    if (fp.fileHandle == nullptr) {
        return ERR_INVALID_PARMS;
    }
    
    LARGE_INTEGER liDistanceToMove;
    liDistanceToMove.QuadPart = offset;
    
    uint32_t oriTbl[] = {FILE_BEGIN, FILE_CURRENT, FILE_END};
    SetFilePointerEx(fp.fileHandle, liDistanceToMove, NULL, oriTbl[origin]);
    
    return FW_OK;
}

sint32_t FlushFileBuffer(file_t & fp) {
    if (fp.fileHandle == nullptr) {
        return ERR_INVALID_PARMS;
    }
    
    FlushFileBuffers(fp.fileHandle);
    
    return 0;
}

END_NAMESPACE_FW

