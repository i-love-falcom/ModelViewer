/**
 * @file fw_path.cpp
 * @author Kamai Masayoshi
 */
#include "precompiled.h"
#include "file/fw_path.h"

#include <shlobj.h>

BEGIN_NAMESPACE_FW
BEGIN_NAMESPACE_NONAME
static const char_t s_invalidFileNameChars[] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    _T('\"'), _T('<'), _T('>'), _T('|'), _T('\0'),
    _T(':'), _T('*'), _T('?'), _T('/'), _T('\\')
};

static const char_t s_invalidPathChars[] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    _T('\"'), _T('<'), _T('>'), _T('|'), _T('\0')
};


// ファイル名禁則文字が含まれるか調べる
static bool CheckValidFileName(const str_t path) {
    assert(path != nullptr);

    const size_t pathLen = tstring::Length(path);

    bool valid = true;
    for (size_t i = 0; i < pathLen; ++i) {
        for (size_t k = 0; k < ARRAY_SIZEOF(s_invalidFileNameChars); ++k) {
            if (path[i] == s_invalidFileNameChars[k]) {
                valid = false;
                break;
            }
        }
        if (!valid) {
            break;
        }
    }
    return valid;
}

// パス禁則文字が含まれるか調べる
static bool CheckValidPath(const str_t path) {
    assert(path != nullptr);

    const size_t pathLen = tstring::Length(path);

    bool valid = true;
    for (size_t i = 0; i < pathLen; ++i) {
        for (size_t k = 0; k < ARRAY_SIZEOF(s_invalidPathChars); ++k) {
            if (path[i] == s_invalidPathChars[k]) {
                valid = false;
                break;
            }
        }
        if (!valid) {
            break;
        }
    }
    return valid;
}

static str_t AddPath(str_t buffer, const size_t numOfElements, const str_t path) {
    assert(CheckValidPath(path));

    if (Path::IsPathRooted(path)) {
        tstring::Copy(buffer, numOfElements, path);
    } else {
        const size_t bufferLen = tstring::Length(buffer);

        if (bufferLen + 2 <= numOfElements &&
            buffer[bufferLen - 1] != Path::DirSeparator() &&
            buffer[bufferLen - 1] != Path::AltDirSeparator() &&
            buffer[bufferLen - 1] != Path::VolumeSeparator()) {
            buffer[bufferLen]     = Path::DirSeparator();
            buffer[bufferLen + 1] = _T('\0');
        }
        tstring::Concat(buffer, numOfElements, path);
    }
    return buffer;
}

END_NAMESPACE_NONAME

const char_t Path::DirSeparator() {
    return _T('\\');
}

const char_t Path::AltDirSeparator() {
    return _T('/');
}

const char_t Path::PathSeparator() {
    return _T(';');
}

const char_t Path::VolumeSeparator() {
    return _T(':');
}

const char_t * Path::GetInvalidFileNameChars() {
    return s_invalidFileNameChars;
}

size_t Path::GetNumInvalidFileNameChars() {
    return ARRAY_SIZEOF(s_invalidFileNameChars);
}

const char_t * Path::GetInvalidPathChars() {
    return s_invalidPathChars;
}

size_t Path::GetNumInvalidPathChars() {
    return ARRAY_SIZEOF(s_invalidPathChars);
}

str_t Path::GetUserDir(str_t buffer, const size_t numOfElements) {
    if (numOfElements >= MAX_PATH) {
        HRESULT hr = SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, buffer);
        assert(hr == S_OK);
    } else {
        char_t out[MAX_PATH];
        HRESULT hr = SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, out);
        assert(hr == S_OK);

        tstring::Copy(buffer, numOfElements, out);
    }

    // DirSeparatorで終わらないなら付加する
    size_t num = tstring::Length(buffer);
    if (buffer[num - 1] != DirSeparator() && num + 2 < numOfElements) {
        buffer[num]     = DirSeparator();
        buffer[num + 1] = _T('\0');
    }
    return buffer;
}

str_t Path::GetTempDir(str_t buffer, const size_t numOfElements) {
    size_t num = ::GetTempPath((DWORD)numOfElements, buffer);

    // DirSeparatorで終わらないなら付加する
    if (buffer[num - 1] != DirSeparator() && num + 2 < numOfElements) {
        buffer[num]     = DirSeparator();
        buffer[num + 1] = _T('\0');
    }
    return buffer;
}

str_t Path::GetCurrentDir(str_t buffer, const size_t numOfElements) {
    size_t num = ::GetCurrentDirectory((DWORD)numOfElements, buffer);

    // DirSeparatorで終わらないなら付加する
    if (buffer[num - 1] != DirSeparator() && num + 2 < numOfElements) {
        buffer[num]     = DirSeparator();
        buffer[num + 1] = _T('\0');
    }
    return buffer;
}

BEGIN_NAMESPACE_NONAME
template<typename T, size_t N>
class Stack {
public:
    void push(T& data) {
        buffer_[sp_++] = data;
    }
    
    T pop() {
        if (sp_ > 0) {
            --sp_;
        }
        return buffer_[sp_];
    }

    Stack() : sp_(0) {
    }

private:
    T buffer_[N];
    size_t sp_;
};
END_NAMESPACE_NONAME

str_t Path::Normalize(str_t buffer, const size_t numOfElements, const str_t path) {
    const size_t pathLen = Min(numOfElements, tstring::Length(path));

    size_t srcIdx = 0;
    size_t dstIdx = 0;

    // PathRootを含むか
    if (pathLen >= 1 && (path[srcIdx] == DirSeparator() || path[srcIdx] == AltDirSeparator())) {
        buffer[dstIdx++] = path[srcIdx++];
    } else if (pathLen >= 3 &&
        (_T('a') <= path[srcIdx] && path[srcIdx] <= _T('z')) || (_T('A') <= path[srcIdx] && path[srcIdx] <= _T('Z')) &&
        (path[srcIdx + 1] == Path::VolumeSeparator()) &&
        (path[srcIdx + 2] == Path::DirSeparator() || path[srcIdx + 2] == AltDirSeparator())) {
        buffer[dstIdx++] = path[srcIdx++];
        buffer[dstIdx++] = path[srcIdx++];
        buffer[dstIdx++] = path[srcIdx++];
    }
    
    // @todo sjis/unicode対応

    Stack<size_t, Path::kMaxPathLen> dirSepStack;
    
    // Rootの位置を積んでおく
    dirSepStack.push(dstIdx);
    
    size_t tmpPathLen = 0;
    char_t tmpPath[Path::kMaxPathLen];
    while (srcIdx < pathLen) {
        buffer[dstIdx++] = path[srcIdx];
        tmpPath[tmpPathLen++] = path[srcIdx];
        
        if (path[srcIdx] == Path::DirSeparator() || path[srcIdx] == Path::AltDirSeparator()) {
            if (tmpPathLen == 2 && tmpPath[0] == _T('.')) {
                // _T('.')は一つ戻る
                dstIdx = dirSepStack.pop();
            } else if (tmpPathLen == 3 && tmpPath[0] == _T('.') && tmpPath[1] == _T('.')) {
                // _T('..')は二つ戻る
                dstIdx = dirSepStack.pop();
                dstIdx = dirSepStack.pop();
            } else {
                dirSepStack.push(dstIdx);
            }
            tmpPathLen = 0;
        }
        srcIdx++;
    }
    buffer[Min(dstIdx, numOfElements - 1)] = _T('\0');

    // AltDirSeparatorをDirSeparatorに置き換え
    for (size_t i = 0; i < pathLen; ++i) {
        if (buffer[i] == Path::AltDirSeparator()) {
            buffer[i] = Path::DirSeparator();
        }
    }

    return buffer;
}

str_t Path::Combine(str_t buffer, const size_t numOfElements, const str_t path1, const str_t path2) {
    assert(buffer != nullptr);
    assert(numOfElements > 0);

    AddPath(buffer, numOfElements, path1);
    AddPath(buffer, numOfElements, path2);

    return buffer;
}

str_t Path::Combine(str_t buffer, const size_t numOfElements, const str_t path1, const str_t path2, const str_t path3) {
    assert(buffer != nullptr);
    assert(numOfElements > 0);

    AddPath(buffer, numOfElements, path1);
    AddPath(buffer, numOfElements, path2);
    AddPath(buffer, numOfElements, path3);

    return buffer;
}

str_t Path::Combine(str_t buffer, const size_t numOfElements, const str_t path1, const str_t path2, const str_t path3, const str_t path4) {
    assert(buffer != nullptr);
    assert(numOfElements > 0);

    AddPath(buffer, numOfElements, path1);
    AddPath(buffer, numOfElements, path2);
    AddPath(buffer, numOfElements, path3);
    AddPath(buffer, numOfElements, path4);

    return buffer;
}

str_t Combine(str_t buffer, const size_t numOfElements, const str_t * paths, const size_t numOfPaths) {
    for (size_t i = 0; i < numOfPaths; ++i) {
        AddPath(buffer, numOfElements, paths[i]);
    }

    return buffer;
}

str_t Path::MakeTempFileName(str_t buffer, const size_t numOfElements, const str_t name, const uint32_t index) {
    assert(buffer != nullptr);
    assert(numOfElements > 0);
    assert(name != nullptr);

    tstring::SPrintf(buffer, sizeof(char_t) * numOfElements, _T("%s%08x"), name, index);

    return buffer;
}

str_t Path::MakeTempFileNameWithDir(str_t buffer, const size_t numOfElements, const str_t name) {
    assert(buffer != nullptr);
    assert(numOfElements > 0);
    assert(name != nullptr);

    char_t tmpPath[kMaxPathLen];
    GetTempDir(tmpPath, ARRAY_SIZEOF(tmpPath));

    tstring::SPrintf(buffer, sizeof(char_t) * numOfElements, _T("%s%s"), tmpPath, name);

    return buffer;
}

str_t Path::MakeTempFileNameWithDir(str_t buffer, const size_t numOfElements, const str_t name, const uint32_t index) {
    assert(buffer != nullptr);
    assert(numOfElements > 0);
    assert(name != nullptr);

    char_t tmpPath[kMaxPathLen];
    GetTempDir(tmpPath, ARRAY_SIZEOF(tmpPath));

    tstring::SPrintf(buffer, sizeof(char_t) * numOfElements, _T("%s%s%08x"), tmpPath, name, index);

    return buffer;
}

str_t Path::GetDirName(str_t buffer, const size_t numOfElements, const str_t path) {
    assert(buffer != nullptr);
    assert(numOfElements > 0);
    assert(CheckValidPath(path));

    const size_t pathLen = tstring::Length(path);
    if (pathLen == 3) {
        // ドライブレター + VolumeSeparator() + DirSeparator() の時
        if (((_T('a') <= path[0] && path[0] <= _T('z')) || (_T('A') <= path[0] && path[0] <= _T('Z'))) &&
            path[1] == VolumeSeparator() &&
            path[2] == DirSeparator()) {
            buffer[0] = _T('\0');
        } else {
            assert(0);
        }
    } else {
        // DirSeparator()を探す
        size_t idx;
        for (size_t i = 1; i <= pathLen; ++i) {
            idx = pathLen - i;
            if (path[idx] == DirSeparator()) {
                break;
            }
        }

        if (idx == 0) {
            // 見つからなかった
            buffer[0] = _T('\0');
        } else {
            // 先頭から最後の区切り文字の手前までコピー
            tstring::CopyN(buffer, numOfElements, path, idx);

            // 終端文字を付加
            buffer[Min(idx, numOfElements - 1)] = _T('\0');
        }
    }

    return buffer;
}
    
str_t Path::GetFileName(str_t buffer, const size_t numOfElements, const str_t path) {
    assert(buffer != nullptr);
    assert(numOfElements > 0);
    assert(CheckValidPath(path));

    const size_t pathLen = tstring::Length(path);
    
    // DirSeparator()を探す
    size_t idx;
    for (size_t i = 1; i <= pathLen; ++i) {
        idx = pathLen - i;
        if (path[idx] == DirSeparator()) {
            ++idx;
            break;
        }
    }

    if (idx == 0 || idx >= pathLen) {
        buffer[0] = _T('\0');
    } else {
        const size_t count = pathLen - idx + 1;

        tstring::CopyN(buffer, numOfElements, &path[idx], count);

        // 終端文字を付加
        buffer[Min(count, numOfElements - 1)] = _T('\0');
    }

    return buffer;
}
    
str_t Path::GetExtension(str_t buffer, const size_t numOfElements, const str_t path) {
    assert(buffer != nullptr);
    assert(numOfElements > 0);
    assert(CheckValidPath(path));

    const size_t pathLen = tstring::Length(path);
    
    // DirSeparator()を探す
    size_t idx;
    for (size_t i = 1; i <= pathLen; ++i) {
        idx = pathLen - i;
        if (path[idx] == DirSeparator()) {
            ++idx;
            break;
        }
    }

    if (idx == 0 || idx >= pathLen) {
        buffer[0] = _T('\0');
    } else {
        // 拡張子区切り文字を探す
        while (idx < pathLen) {
            if (path[idx] == _T('.')) {
                break;
            }
            ++idx;
        }
        if (idx >= pathLen) {
            buffer[0] = _T('\0');
        } else {
            const size_t count = pathLen - idx;

            tstring::CopyN(buffer, numOfElements, &path[idx], count);

            // 終端文字を付加
            buffer[Min(count, numOfElements - 1)] = _T('\0');
        }
    }

    return buffer;
}
    
str_t Path::GetFileNameWithoutExtension(str_t buffer, const size_t numOfElements, const str_t path) {
    assert(buffer != nullptr);
    assert(numOfElements > 0);
    assert(CheckValidPath(path));

    const size_t pathLen = tstring::Length(path);
    
    // DirSeparator()を探す
    size_t startIdx;
    for (size_t i = 1; i <= pathLen; ++i) {
        startIdx = pathLen - i;
        if (path[startIdx] == DirSeparator()) {
            ++startIdx;
            break;
        }
    }

    if (startIdx == 0 || startIdx >= pathLen) {
        buffer[0] = _T('\0');
    } else {
        // 拡張子区切り文字を探す
        size_t endIdx = startIdx;
        while (endIdx < pathLen) {
            if (path[endIdx] == _T('.')) {
                break;
            }
            ++endIdx;
        }

        if (startIdx == endIdx) {
            buffer[0] = _T('\0');
        } else {
            if (endIdx >= pathLen) {
                endIdx = pathLen;
            }
            const size_t count = endIdx - startIdx;

            // 先頭から最後の区切り文字の手前までコピー
            tstring::CopyN(buffer, numOfElements, &path[startIdx], count);

            // 終端文字を付加
            buffer[Min(count, numOfElements - 1)] = _T('\0');
        }
    }

    return buffer;
}

str_t Path::GetPathRoot(str_t buffer, const size_t numOfElements, const str_t path) {
    assert(buffer != nullptr);
    assert(numOfElements > 0);
    assert(CheckValidPath(path));

    const size_t pathLen = tstring::Length(path);

    if (pathLen < 3) {
        buffer[0] = _T('\0');
    } else {
        if (numOfElements > 4 &&
            ((_T('a') <= path[0] && path[0] <= _T('z')) || (_T('A') <= path[0] && path[0] <= _T('Z'))) &&
            path[1] == VolumeSeparator() &&
            path[2] == DirSeparator()) {
            buffer[0] = path[0];
            buffer[1] = path[1];
            buffer[2] = path[2];
            buffer[3] = _T('\0');
        } else {
            buffer[0] = _T('\0');
        }
    }

    return buffer;
}

str_t Path::ChangeExtension(str_t buffer, const size_t numOfElements, const str_t path, const str_t extension) {
    GetFileNameWithoutExtension(buffer, numOfElements, path);
    assert(CheckValidPath(path));
    assert(CheckValidFileName(extension));
    
    if (extension[0] != _T('.')) {
        tstring::Concat(buffer, numOfElements, _T("."));
    }
    tstring::Concat(buffer, numOfElements, extension);

    return buffer;
}

bool Path::HasExtension(const str_t path) {
    assert(CheckValidPath(path));

    const size_t pathLen = tstring::Length(path);
    
    // DirSeparator()を探す
    size_t idx;
    for (size_t i = 1; i <= pathLen; ++i) {
        idx = pathLen - i;
        if (path[idx] == DirSeparator()) {
            ++idx;
            break;
        }
    }

    if (idx == 0 || idx >= pathLen) {
        return false;
    } else {
        // 拡張子区切り文字を探す
        while (idx < pathLen) {
            if (path[idx] == _T('.')) {
                break;
            }
            ++idx;
        }
        if (idx >= pathLen) {
            return false;
        }
    }
    return true;
}

bool Path::IsPathRooted(const str_t path) {
    assert(CheckValidPath(path));

    if ((path[0] == DirSeparator()) || 
        (path[0] == AltDirSeparator()) || 
        (((_T('a') <= path[0] && path[0] <= _T('z')) || (_T('A') <= path[0] && path[0] <= _T('Z'))) && path[1] == VolumeSeparator())) {
        return true;
    }
    return false;
}

END_NAMESPACE_FW
