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
    enum ProtoType {
        PROTO_TYPE_RELATIVE = 0,
        PROTO_TYPE_UNC,
        PROTO_TYPE_FILE_URI_WINDOWS,
        PROTO_TYPE_FILE_URI_UNIX,
        PROTO_TYPE_HTTP,
        PROTO_TYPE_HTTPS,
        PROTO_TYPE_SFTP,
        PROTO_TYPE_FTP,
        PROTO_TYPE_WIN_DRIVER,
        PROTO_TYPE_WIN_ABS,
        PROTO_TYPE_UNIX_ABS
    };
    std::string _path = "";
    std::string protoHead = "";
    ProtoType protoType = PROTO_TYPE_RELATIVE;

    void setPath(std::string newPath) {
        _path = newPath;
    }
#ifdef OS_WINDOWS
    bool windowsStyle = true;
#else
    bool windowsStyle = false;
#endif

    typedef char char_type;
    enum format {
        generic_format,  ///< The generic format, internally used by
        ///< ghc::filesystem::path with slashes
        native_format,   ///< The format native to the current platform this code
        ///< is build for
        auto_format,     ///< Try to auto-detect the format, fallback to native
    };
    int _prefixLength = 0;
    static constexpr char_type posix_separator = '/';
#ifdef OS_WINDOWS
    static constexpr char_type preferred_separator = '\\';
    static constexpr bool IS_WINDOWS = true;
#else
    static constexpr bool IS_WINDOWS = false;
    static constexpr char_type preferred_separator = '/';
#endif

    class NormalRet {
    public:

        std::vector<std::string> strs;
        std::string protoHead = "";
        ProtoType protoType = PROTO_TYPE_RELATIVE;
    };

    void toSystemSeparator();

    PurePath();

    explicit PurePath(const std::string &c);

    PurePath(const char *c);

    PurePath(const PurePath &c);

    PurePath &joinpath_(const PurePath &p);

    bool has_root_directory() const;

    size_t root_name_length() const noexcept;

    bool empty() const noexcept;

    bool is_relative() const;

    bool has_root_name() const;

    const PurePath::char_type *c_str() const noexcept;

    PurePath current_path();

    bool current_path(const PurePath &p) noexcept;

    PurePath abspath(const PurePath &p);

    std::string basename() const;

    PurePath& asWindows(){
        windowsStyle = true;
        return *this;
    }
    PurePath& asPosix(){
        windowsStyle = false;
        return *this;
    }
    std::string str() {
        std::string ret;
        switch (protoType) {
            case PROTO_TYPE_UNC: {
                for (int i = 0; i < parts.size(); i++) {
                    if (i == 0)
                        ret = "\\\\";
                    else if (i == parts.size() - 1)
                        ret = ret + parts[i];
                    else
                        ret = ret + parts[i] + '\\';
                }
                break;
            }
            case PROTO_TYPE_FILE_URI_WINDOWS: {
                for (int i = 0; i < parts.size(); i++) {
                    if (i == 0) {
                        ret = "file:///";
                    } else if (parts[i] == "/" || parts[i] == "\\") {
                        if (ret[ret.length() - 1] == '/')
                            continue;
                        ret = ret + parts[i];
                    } else if (i == parts.size() - 1) {
                        ret = ret + parts[i];
                    } else {
                        ret = ret + parts[i] + "/";
                    }

                }
                break;
            }
            case PROTO_TYPE_FILE_URI_UNIX:
                for (int i = 0; i < parts.size(); i++) {
                    if (i == 0) {
                        ret = "file://";
                    } else if (parts[i] == "/" || parts[i] == "\\") {
//						if(ret[ret.length()-1] == '/')
//							continue;
                        ret = ret + parts[i];
                    } else if (i == parts.size() - 1) {
                        ret = ret + parts[i];
                    } else {
                        ret = ret + parts[i] + "/";
                    }

                }
                break;
            case PROTO_TYPE_HTTP:
                ret = "http://";
                for (int i = 1; i < parts.size(); i++) {
                    if (i == parts.size() - 1) {
                        ret = ret + parts[i];
                    } else {
                        ret = ret + parts[i] + "/";
                    }
                }
                break;
            case PROTO_TYPE_HTTPS:
                ret = "https://";
                for (int i = 1; i < parts.size(); i++) {
                    if (i == parts.size() - 1) {
                        ret = ret + parts[i];
                    } else {
                        ret = ret + parts[i] + "/";
                    }
                }
                break;
            case PROTO_TYPE_SFTP:
                ret = "sftp://";
                for (int i = 1; i < parts.size(); i++) {
                    if (i == parts.size() - 1) {
                        ret = ret + parts[i];
                    } else {
                        ret = ret + parts[i] + "/";
                    }
                }
                break;
            case PROTO_TYPE_FTP:
                ret = "ftp://";
                for (int i = 1; i < parts.size(); i++) {
                    if (i == parts.size() - 1) {
                        ret = ret + parts[i];
                    } else {
                        ret = ret + parts[i] + "/";
                    }
                }
                break;
            case PROTO_TYPE_WIN_DRIVER:
                ret = parts[0];
                break;
            case PROTO_TYPE_WIN_ABS:
                ret = parts[0];
                for (int i = 1; i < parts.size(); i++) {
                    if (parts[i] == "/" || parts[i] == "\\") {
                        if (ret[ret.length() - 1] == '\\')
                            continue;
                        ret = ret + "\\";
                    } else if (i == parts.size() - 1) {
                        ret = ret + parts[i];
                    } else {
                        ret = ret + parts[i] + "\\";
                    }
                }
                break;
            case PROTO_TYPE_UNIX_ABS:
                ret = parts[0];
                for (int i = 1; i < parts.size(); i++) {
                    if (i == parts.size() - 1) {
                        ret = ret + parts[i];
                    } else {
                        ret = ret + parts[i] + "/";
                    }
                }
                break;
            case PROTO_TYPE_RELATIVE:
                if (parts.empty())
                    break;
                for (int i = 0; i < parts.size(); i++) {
                    if (i == parts.size() - 1) {
                        ret = ret + parts[i];
                    } else {
                        if(windowsStyle)
                            ret = ret + parts[i] + "\\";
                        else
                            ret = ret + parts[i] + "/";
                    }
                }
                break;

        }
        return ret;
    }


    void PurePath2(const std::string &c) {
        PurePath::join2(this->parts, this->protoHead, this->protoType, c);
    }

    static void join(PurePath::NormalRet &a, const std::string &s);

    static void
    join2(std::vector<std::string> &strs, std::string &protoHead, ProtoType &protoType, const std::string &s);

    template<typename... TAIL>
    void PurePath2(const std::string &v, TAIL... args) {
        PurePath2(v);
        PurePath2(args...);
    }

    template<typename T, typename... TAIL>
    PurePath(const T &v, TAIL... args) {
        PurePath2(v, args...);
    }


    static PurePath::NormalRet normal(const std::string &s);

    static std::string commonprefix(const std::vector<PurePath> &strs);

    static std::string dirname(const std::string &str);

    static bool exists(const std::string &str);

    static std::string expanduser(const std::string &str);

    static std::string expandvars(const std::string &str);//TODO


    std::vector<std::string> parts; //一个元组，可以访问路径的多个组件
    std::string drive; //一个表示驱动器盘符或命名的字符串，如果存在: PureWindowsPath('c:/Program Files/').drive-> 'c:'  PureWindowsPath('//host/share/foo.txt').drive -> '\\\\host\\share'
    std::string root;// PureWindowsPath('c:/Program Files/').root   -> '\\'   >>> PurePosixPath('/etc').root  ->'/'    PureWindowsPath('//host/share').root   -> '\\'
    std::string anchor;
    std::vector<std::string> parents;
    std::string parent;
    std::string name;
    std::string suffix;
    std::vector<std::string> suffixes;
    std::string stem;

    std::string as_posix();

    std::string as_uri();

    bool is_absolute() const;

    bool is_relative_to(const std::string &other);

    bool is_reserved();

    std::string match(const std::string &str);

    std::string relative_to(const std::string &str);

    std::string with_name(const std::string &str);

    std::string with_stem(const std::string &str);

    std::string with_suffix(const std::string &str);

    template<typename T>
    PurePath &joinpath(const T &v) {
        return joinpath_(v);
    }

    template<typename T, typename... TAIL>
    PurePath &joinpath(const T &v, TAIL... args) {
        joinpath(v);
        return joinpath(args...);
    }
};

class Path {
public:
    struct stat_result {
        int64_t st_size;
        int64_t st_mtime;
        int32_t st_mode;
    };

    stat_result stat();//返回一个 os.stat_result 对象，其中包含有关此路径的信息，例如 os.stat()。 结果会在每次调用此方法时重新搜索。;
    bool chmod(int mode);//改变文件的模式和权限;
    std::string exists();//文件是否存在;
    std::string expanduser();//返回展开了包含 ~ 和 ~user 的构造;
    std::string glob(const std::string &pattern);//解析相对于此路径的通配符 pattern，产生所有匹配的文件:;
    std::string group();//返回拥有此文件的用户组。如果文件的 GID 无法在系统数据库中找到，将抛出 KeyError 。;
    std::string
    is_dir();//如果路径指向一个目录（或者一个指向目录的符号链接）则返回 True，如果指向其他类型的文件则返回 False。当路径不存在或者是一个破损的符号链接时也会返回 False；其他错误（例如权限错误）被传播。;
    std::string
    is_file();//如果路径指向一个正常的文件（或者一个指向正常文件的符号链接）则返回 True，如果指向其他类型的文件则返回 False。 当路径不存在或者是一个破损的符号链接时也会返回 False；其他错误（例如权限错误）被传播;
    std::string
    is_mount();//如果路径是一个 挂载点 <mount point>：在文件系统中被其他不同的文件系统挂载的地点。在 POSIX 系统，此函数检查 path 的父级 —— path/.. 是否处于一个和 path 不同的设备中，或者 file:path/.. 和 path 是否指向相同设备的相同 i-node —— 这能检测所有 Unix 以及 POSIX 变种上的挂载点。 Windows 上未实现。;
    std::string is_symlink();//如果路径指向符号链接则返回 True， 否则 False。如果路径不存在也返回 False；其他错误（例如权限错误）被传播。;
    std::string
    is_socket(); //如果路径指向一个 Unix socket 文件（或者指向 Unix socket 文件的符号链接）则返回 True，如果指向其他类型的文件则返回 False。当路径不存在或者是一个破损的符号链接时也会返回 False；其他错误（例如权限错误）被传播。;
    std::string
    is_fifo();//如果路径指向一个先进先出存储（或者指向先进先出存储的符号链接）则返回 True ，指向其他类型的文件则返回 False。当路径不存在或者是一个破损的符号链接时也会返回 False；其他错误（例如权限错误）被传播。;
    std::string
    is_block_device();//如果文件指向一个块设备（或者指向块设备的符号链接）则返回 True，指向其他类型的文件则返回 False。当路径不存在或者是一个破损的符号链接时也会返回 False；其他错误（例如权限错误）被传播。;
    std::string
    is_char_device();//如果路径指向一个字符设备（或指向字符设备的符号链接）则返回 True，指向其他类型的文件则返回 False。当路径不存在或者是一个破损的符号链接时也会返回 False；其他错误（例如权限错误）被传播。;
    std::string iterdir();

    std::string lchmod(int mode);

    std::string lstat();

    std::string mkdir(int mode = 0777, bool parents = false, bool exist_ok = false);

    std::string
    open(int mode = 'r', int buffering = -1, void *encoding = NULL, void *errors = NULL, void *newline = NULL);

    std::string owner();

    std::string read_bytes();

    std::string read_text(void *encoding = NULL, void *errors = NULL);

    std::string readlink();

    std::string rename(const std::string &target);

    std::string replace(const std::string &target);

    std::string resolve(bool strict = false);

    std::string rglob(const std::string &pattern);

    std::string rmdir();

    std::string samefile(const std::string &other_path);

    std::string symlink_to(const std::string &target, bool target_is_directory = false);

    std::string link_to(const std::string &target);

    std::string touch(int mode = 0666, bool exist_ok = true);

    std::string unlink(bool missing_ok = false);

    std::string write_bytes(std::vector<char> &data);

    std::string write_text(const std::string &data, void *encoding = NULL, void *errors = NULL);

    std::string cwd();

    std::string home();
};

#endif //EM_PUREPATH_H
