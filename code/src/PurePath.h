//
// Created by caspase on 6/17/2021.
//

#ifndef EM_PUREPATH_H
#define EM_PUREPATH_H
#define OS_WINDOWS

#ifdef OS_WINDOWS
#include <windows.h>
// additional includes
#include <shellapi.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <wchar.h>
#include <winioctl.h>
#else
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef GHC_OS_ANDROID
#include <android/api-level.h>
#if __ANDROID_API__ < 12
#include <sys/syscall.h>
#endif
#include <sys/vfs.h>
#define statvfs statfs
#else
#include <sys/statvfs.h>
#endif
#ifdef GHC_OS_CYGWIN
#include <strings.h>
#endif
#if !defined(__ANDROID__) || __ANDROID_API__ >= 26
#include <langinfo.h>
#endif
#endif
#ifdef GHC_OS_MACOS
#include <Availability.h>
#endif

#include <filesystem>
#define os_path PurePath()
#define __file__  PurePath(__FILE__).basename()
class PurePath {
public:
    std::string _path = "";

typedef char char_type;
    enum format {
        generic_format,  ///< The generic format, internally used by
        ///< ghc::filesystem::path with slashes
        native_format,   ///< The format native to the current platform this code
        ///< is build for
        auto_format,     ///< Try to auto-detect the format, fallback to native
    };
    int _prefixLength = 0;

#ifdef OS_WINDOWS
    static constexpr char_type preferred_separator = '\\';
#else
    static constexpr char_type preferred_separator = '/';
#endif
    void toSystemSeparator();
    PurePath();
    explicit PurePath(const std::string& c);
    PurePath(const char* c);
    PurePath(const PurePath& c);
    PurePath& joinpath_(const PurePath& p);
    bool has_root_directory() const;
    size_t root_name_length() const noexcept;
    bool is_absolute() const;
    bool empty() const noexcept;
    bool is_relative() const;
    bool has_root_name() const;
    const PurePath::char_type* c_str() const noexcept;
    PurePath current_path();
    bool current_path(const PurePath& p) noexcept;
    PurePath abspath(const PurePath& p);
    std::string basename() const;



    void PurePath2(const std::string & v){
        this->_path = join(this->_path, v);
    }

    static std::string join(const std::string &a, const std::string &p);

    template <typename... TAIL>
    void PurePath2(const std::string& v, TAIL... args){
        PurePath2(v);
        PurePath2(args...);
    }
    template <typename T,typename... TAIL>
    PurePath(const T& v, TAIL... args){
        PurePath2(v, args...);
    }
    template <typename T>
    PurePath& joinpath(const T& v){
        return joinpath_(v);
    }
    template <typename T,typename... TAIL>
    PurePath& joinpath(const T& v, TAIL... args){
        joinpath(v);
        return joinpath(args...);
    }

    static std::string normal(std::string s);
    static std::string commonprefix(const std::vector<PurePath>& strs);
    static std::string dirname(const std::string &str);
    static bool exists(const std::string &str);
    static std::string expanduser(const std::string &str) ;
    static std::string expandvars(const std::string &str) ;//TODO

    static bool is_absolute(const std::string& str);
    static bool is_relative_to(const std::string& str);
    static bool is_reserved();
    static bool match(const std::string& str);
    static std::string relative_to(const std::string& strParent, const std::string& strChild);
    static std::string with_name(const std::string& oldStr, const std::string& name);
    static std::string with_stem(const std::string& oldStr, const std::string& stem);
    static std::string with_suffix(const std::string& oldStr, const std::string& suffix);
    static std::stirng parts
    //PurePath('foo', 'some/path', 'bar') -> PurePosixPath('foo/some/path/bar')

    //    basename(path) // 	返回文件名
//    commonprefix(list) // 	返回list(多个路径) //中，所有path共有的最长的路径
//    dirname(path) // 	返回文件路径
//    exists(path) // 	路径存在则返回True,路径损坏返回False
//    lexists 	//路径存在则返回True,路径损坏也返回True
//    expanduser(path) // 	把path中包含的"~"和"~user"转换成用户目录
//    expandvars(path) // 	根据环境变量的值替换path中包含的"$name"和"${name}"
//    getatime(path) // 	返回最近访问时间（浮点型秒数）
//    getmtime(path) // 	返回最近文件修改时间
//    getctime(path) // 	返回文件 path 创建时间
//    getsize(path) // 	返回文件大小，如果文件不存在就返回错误
//    isabs(path) // 	判断是否为绝对路径
//    isfile(path) // 	判断路径是否为文件
//    isdir(path) // 	判断路径是否为目录
//    islink(path) // 	判断路径是否为链接
//    ismount(path) // 	判断路径是否为挂载点
//    join(path1[, path2[, ...]]) // 	把目录和文件名合成一个路径
//    normcase(path) // 	转换path的大小写和斜杠
//    normpath(path) // 	规范path字符串形式
//    realpath(path) // 	返回path的真实路径
//    relpath(path[, start]) // 	从start开始计算相对路径
//    samefile(path1, path2) // 	判断目录或文件是否相同
//    sameopenfile(fp1, fp2) // 	判断fp1和fp2是否指向同一文件
//    samestat(stat1, stat2) // 	判断stat tuple stat1和stat2是否指向同一个文件
//    split(path) // 	把路径分割成 dirname 和 basename，返回一个元组
//    splitdrive(path) // 	一般用在 windows 下，返回驱动器名和路径组成的元组
//    splitext(path) // 	分割路径中的文件名与拓展名
//    splitunc(path) // 	把路径分割为加载点与文件
//    walk(path, visit, arg) // 	遍历path，进入每个目录都调用visit函数，visit函数必须有3个参数(arg, dirname, names) //，dirname表示当前目录的目录名，names代表当前目录下的所有文件名，args则为walk的第三个参数
//    supports_unicode_filenames 	设置是否支持unicode路径名
};


#endif //EM_PUREPATH_H
