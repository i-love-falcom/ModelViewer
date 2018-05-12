/**
 * @file fw_path.h
 * @author Kamai Masayoshi
 */
#ifndef FW_PATH_H_
#define FW_PATH_H_

BEGIN_NAMESPACE_FW

/**
 * @class Path
 */
class FW_DLL Path {
public:
    enum {
        kMaxFNameLen    = 255,
        kMaxExtLen      = 255,
        kMaxPathLen     = 1023,
    };

    /**
     * @brief ディレクトリ区切り文字を取得
     */
    static const char_t DirSeparator();

    /**
     * @brief ディレクトリ区切り文字を取得
     */
    static const char_t AltDirSeparator();

    /**
     * @brief パス区切り文字を取得
     */
    static const char_t PathSeparator();

    /**
     * @brief ボリューム区切り文字を取得
     */
    static const char_t VolumeSeparator();

    /**
     * @brief ファイル名に使用できない文字を含む配列を取得
     * @return 配列
     */
    static const char_t * GetInvalidFileNameChars();

    /**
     * @brief パスに使用できない文字を含む配列の要素数を取得
     */
    static size_t GetNumInvalidFileNameChars();

    /**
     * @brief パスに使用できない文字を含む配列を取得
     * @return 配列
     */
    static const char_t * GetInvalidPathChars();

    /**
     * @brief パスに使用できない文字を含む配列の要素数を取得
     */
    static size_t GetNumInvalidPathChars();

    /**
     * @brief ユーザディレクトリへのパスを取得
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @return bufferを返す
     */
    static str_t GetUserDir(str_t buffer, const size_t numOfElements);

    /**
     * @brief 一時ディレクトリへのパスを取得
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @return bufferを返す
     */
    static str_t GetTempDir(str_t buffer, const size_t numOfElements);

    /**
     * @brief カレントディレクトリへのパスを取得
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @return bufferを返す
     */
    static str_t GetCurrentDir(str_t buffer, const size_t numOfElements);

    /**
     * @brief パス名を正規化
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @param[in]   path            入力パス
     */
    static str_t Normalize(str_t buffer, const size_t numOfElements, const str_t path);

    /**
     * @brief 2つの入力パスを1つに結合
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @param[in]   path1           1番目の入力パス
     * @param[in]   path2           2番目の入力パス
     * @return bufferを返す
     */
    static str_t Combine(str_t buffer, const size_t numOfElements, const str_t path1, const str_t path2);

    /**
     * @brief 3つの入力パスを1つに結合
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @param[in]   path1           1番目の入力パス
     * @param[in]   path2           2番目の入力パス
     * @param[in]   path3           3番目の入力パス
     * @return bufferを返す
     */
    static str_t Combine(str_t buffer, const size_t numOfElements, const str_t path1, const str_t path2, const str_t path3);

    /**
     * @brief 4つの入力パスを1つに結合
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @param[in]   path1           1番目の入力パス
     * @param[in]   path2           2番目の入力パス
     * @param[in]   path3           3番目の入力パス
     * @param[in]   path4           4番目の入力パス
     * @return bufferを返す
     */
    static str_t Combine(str_t buffer, const size_t numOfElements, const str_t path1, const str_t path2, const str_t path3, const str_t path4);

    /**
     * @brief 4つの入力パスを1つに結合
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @param[in]   paths           入力パスの配列
     * @param[in]   numOfPaths      入力パスの配列の要素数
     * @return bufferを返す
     */
    static str_t Combine(str_t buffer, const size_t numOfElements, const str_t * paths, const size_t numOfPaths);

    /**
     * @brief 一時ファイル用のファイル名を生成
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @param[in]   name            接頭辞となるファイル名
     * @param[in]   index           nameに続く16進数の番号
     */
    static str_t MakeTempFileName(str_t buffer, const size_t numOfElements, const str_t name, const uint32_t index);

    /**
     * @brief ディレクトリ付きで一時ファイル用のファイル名を生成
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @param[in]   name            接頭辞となるファイル名
     */
    static str_t MakeTempFileNameWithDir(str_t buffer, const size_t numOfElements, const str_t name);

    /**
     * @brief ディレクトリ付きで一時ファイル用のファイル名を生成
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @param[in]   name            接頭辞となるファイル名
     * @param[in]   index           nameに続く16進数の番号
     */
    static str_t MakeTempFileNameWithDir(str_t buffer, const size_t numOfElements, const str_t name, const uint32_t index);

    /**
     * @brief 指定したパスからディレクトリ名を取得
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @param[in]   path            入力パス
     * @return bufferを返す
     */
    static str_t GetDirName(str_t buffer, const size_t numOfElements, const str_t path);
    
    /**
     * @brief 指定したパスから拡張子を含むファイル名を取得
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @param[in]   path            入力パス
     * @return bufferを返す
     */
    static str_t GetFileName(str_t buffer, const size_t numOfElements, const str_t path);
    
    /**
     * @brief 指定したパスから拡張子を取得
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @param[in]   path            入力パス
     * @return bufferを返す
     */
    static str_t GetExtension(str_t buffer, const size_t numOfElements, const str_t path);
    
    /**
     * @brief 指定したパスから拡張子を抜いたファイル名を取得
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @param[in]   path            入力パス
     * @return bufferを返す
     */
    static str_t GetFileNameWithoutExtension(str_t buffer, const size_t numOfElements, const str_t path);
    
    /**
     * @brief 指定したパスからルートディレクトリを取得
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @param[in]   path            入力パス
     * @return bufferを返す
     */
    static str_t GetPathRoot(str_t buffer, const size_t numOfElements, const str_t path);

    /**
     * @brief 拡張子を変更する
     * @param[out]  buffer          格納先バッファ
     * @param[in]   numOfElements   格納先バッファに格納可能な文字数
     * @param[in]   path            入力パス
     * @param[in]   extension       変更する拡張子
     * @return bufferを返す
     */
    static str_t ChangeExtension(str_t buffer, const size_t numOfElements, const str_t path, const str_t extension);

    /**
     * @brief 指定したパスが拡張子を持つか調べる
     * @param[in]   path    入力パス
     * @return 拡張子が含まれるならtrue
     */
    static bool HasExtension(const str_t path);

    /**
     * @brief 指定したパスにルートディレクトリを含むか調べる
     * @param[in]   path    入力パス
     * @return ルートディレクトリが含まれるならtrue
     */
    static bool IsPathRooted(const str_t path);
};

END_NAMESPACE_FW

#endif  // FW_PATH_H_
