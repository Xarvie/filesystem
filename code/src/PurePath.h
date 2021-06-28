//
// Created by caspase on 6/17/2021.
//

#ifndef EM_PUREPATH_H
#define EM_PUREPATH_H
#define OS_WINDOWS

#ifdef OS_WINDOWS
#include <cassert>
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
#include <iostream>


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
    std::vector<char> readFile(const char* str);
    std::vector<char> readFile2(FILE* fp, size_t size);
    int writeFile(const char* str);
    int writeFile2(FILE* f, const char* dataPtr, int size);
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
		std::vector<std::wstring> wstrs;
		ProtoType protoType = PROTO_TYPE_RELATIVE;
	};

	void toSystemSeparator();

	PurePath();

	explicit PurePath(const std::string &c);

	PurePath(const char *c);
	~PurePath()
	{
//        std::cout << this->parts[0] << std::endl;
	}
	PurePath(const PurePath &c);
	PurePath operator+(const PurePath& path);
	PurePath& operator+=(const PurePath& path);
	PurePath &joinpath_(const PurePath &p);

	bool has_root_directory() const;

	size_t root_name_length() const noexcept;

	bool empty() const noexcept;

	bool is_relative() const;

	bool has_root_name() const;

	PurePath current_path();

	bool current_path(const PurePath &p) noexcept;

//    PurePath abspath(const PurePath &p);

	std::string basename() const;

	PurePath& asWindows(){
		windowsStyle = true;
		return *this;
	}
	PurePath& asPosix(){
		windowsStyle = false;
		return *this;
	}
	std::string str() const {
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

	std::wstring wstr() const;

	void PurePath2(const std::string &c) {
		PurePath::join(this->parts,this->parts_w, this->protoHead, this->protoType, c);
	}

	static void
	join(std::vector<std::string> &strs, std::vector<std::wstring>&wstrs, std::string &protoHead, ProtoType &protoType, const std::string &s);
	static void
	join(std::vector<std::string> &strs, std::vector<std::wstring>&wstrs, std::string &protoHead, ProtoType &protoType, const PurePath &path);



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
	std::vector<std::wstring> parts_w; //一个元组，可以访问路径的多个组件
	std::string drive() const; //一个表示驱动器盘符或命名的字符串，如果存在: PureWindowsPath('c:/Program Files/').drive-> 'c:'  PureWindowsPath('//host/share/foo.txt').drive -> '\\\\host\\share'
	std::string root() const;// PureWindowsPath('c:/Program Files/').root   -> '\\'   >>> PurePosixPath('/etc').root  ->'/'    PureWindowsPath('//host/share').root   -> '\\'
	std::string anchor() const;
	std::vector<std::string> parents() const;
	std::string parent() const;
	std::string name() const;
	std::string suffix() const;
	std::vector<std::string> suffixes() const;
	std::string stem() const;

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
};

inline bool operator==(const PurePath& left, const PurePath& right) {
	bool ret = (left.parts == right.parts);
	return ret;
}

enum class file_type {
    none,
    not_found,
    regular,
    directory,
    symlink,
    block,
    character,
    fifo,
    socket,
    unknown,
};
#ifdef OS_WINDOWS
#ifndef REPARSE_DATA_BUFFER_HEADER_SIZE
typedef struct _REPARSE_DATA_BUFFER
{
    ULONG ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;
    union
    {
        struct
        {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            ULONG Flags;
            WCHAR PathBuffer[1];
        } SymbolicLinkReparseBuffer;
        struct
        {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR PathBuffer[1];
        } MountPointReparseBuffer;
        struct
        {
            UCHAR DataBuffer[1];
        } GenericReparseBuffer;
    } DUMMYUNIONNAME;
} REPARSE_DATA_BUFFER;
#ifndef MAXIMUM_REPARSE_DATA_BUFFER_SIZE
#define MAXIMUM_REPARSE_DATA_BUFFER_SIZE (16 * 1024)
#endif
#endif
#endif
#define GHC_NATIVEWP(p) p.c_str()
inline std::shared_ptr<REPARSE_DATA_BUFFER> getReparseData(const PurePath& p, std::error_code& ec)
{

    std::shared_ptr<void> file(CreateFileW(GHC_NATIVEWP(p.wstr()), 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, 0), CloseHandle);
    if (file.get() == INVALID_HANDLE_VALUE) {
        std::cout << "error" << __LINE__ << std::endl;
        return nullptr;
    }

    std::shared_ptr<REPARSE_DATA_BUFFER> reparseData((REPARSE_DATA_BUFFER*)std::calloc(1, MAXIMUM_REPARSE_DATA_BUFFER_SIZE), std::free);
    ULONG bufferUsed;
    if (DeviceIoControl(file.get(), FSCTL_GET_REPARSE_POINT, 0, 0, reparseData.get(), MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &bufferUsed, 0)) {
        return reparseData;
    }
    else {
        std::cout << "error" << __LINE__ << std::endl;
    }
    return nullptr;
}

// [fs.enum.perms]
enum perms : uint16_t {
	none = 0,

	owner_read = 0400,
	owner_write = 0200,
	owner_exec = 0100,
	owner_all = 0700,

	group_read = 040,
	group_write = 020,
	group_exec = 010,
	group_all = 070,

	others_read = 04,
	others_write = 02,
	others_exec = 01,
	others_all = 07,

	all = 0777,
	set_uid = 04000,
	set_gid = 02000,
	sticky_bit = 01000,

	mask = 07777,
	unknown = 0xffff
};

// [fs.enum.perm.opts]
enum class perm_options : uint16_t {
	replace = 3,
	add = 1,
	remove = 2,
	nofollow = 4,
};

// [fs.enum.copy.opts]
enum class copy_options : uint16_t {
	none = 0,

	skip_existing = 1,
	overwrite_existing = 2,
	update_existing = 4,

	recursive = 8,

	copy_symlinks = 0x10,
	skip_symlinks = 0x20,

	directories_only = 0x40,
	create_symlinks = 0x80,
#ifndef GHC_OS_WEB
	create_hard_links = 0x100
#endif
};

// [fs.enum.dir.opts]
enum class directory_options : uint16_t {
	none = 0,
	follow_directory_symlink = 1,
	skip_permission_denied = 2,
};

class file_status
{
public:
	// [fs.file_status.cons] constructors and destructor
	file_status() noexcept;
	explicit file_status(file_type ft, int16_t prms = perms::unknown) noexcept;
	file_status(const file_status&) noexcept;
	file_status(file_status&&) noexcept;
	~file_status();
	// assignments:
	file_status& operator=(const file_status&) noexcept;
	file_status& operator=(file_status&&) noexcept;
	// [fs.file_status.mods] modifiers
	void type(file_type ft) noexcept;
	void permissions(int16_t prms) noexcept;
	// [fs.file_status.obs] observers
	file_type type() const noexcept;
	int16_t permissions() const noexcept;
	friend bool operator==(const file_status& lhs, const file_status& rhs) noexcept { return lhs.type() == rhs.type() && lhs.permissions() == rhs.permissions(); }
private:
	file_type _type;
	int16_t _perms;
};
#ifdef OS_WINDOWS
inline time_t timeFromFILETIME(const FILETIME& ft)
{
    ULARGE_INTEGER ull;
    ull.LowPart = ft.dwLowDateTime;
    ull.HighPart = ft.dwHighDateTime;
    return static_cast<time_t>(ull.QuadPart / 10000000ULL - 11644473600ULL);
}

inline void timeToFILETIME(time_t t, FILETIME& ft)
{
    LONGLONG ll;
    ll = Int32x32To64(t, 10000000) + 116444736000000000;
    ft.dwLowDateTime = static_cast<DWORD>(ll);
    ft.dwHighDateTime = static_cast<DWORD>(ll >> 32);
}
#endif

template <typename INFO>
inline DWORD reparse_tag_from_INFO(const INFO*)
{
	return 0;
}

template <>
inline DWORD reparse_tag_from_INFO(const WIN32_FIND_DATAW* info)
{
	return info->dwReserved0;
}

template <typename INFO>
inline file_status status_from_INFO(const PurePath& p, const INFO* info, std::error_code& ec, uintmax_t* sz = nullptr, time_t* lwt = nullptr)
{
	file_type ft = file_type::unknown;
	if (sizeof(INFO) == sizeof(WIN32_FIND_DATAW)) {
		if (reparse_tag_from_INFO(info) == IO_REPARSE_TAG_SYMLINK) {
			ft = file_type::symlink;
		}
	}
	else {
		if ((info->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
			auto reparseData = getReparseData(p, ec);
			if (!ec && reparseData && IsReparseTagMicrosoft(reparseData->ReparseTag) && reparseData->ReparseTag == IO_REPARSE_TAG_SYMLINK) {
				ft = file_type::symlink;
			}
		}
	}
	if (ft == file_type::unknown) {
		if ((info->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			ft = file_type::directory;
		}
		else {
			ft = file_type::regular;
		}
	}
	int16_t prms = perms::owner_read | perms::group_read | perms::others_read;
	if (!(info->dwFileAttributes & FILE_ATTRIBUTE_READONLY)) {
		prms = prms | perms::owner_write | perms::group_write | perms::others_write;
	}
	std::string ext = p.suffix();
	if (!ext.empty() && (ext == "exe" || ext == "bat" || ext == "msi" || ext == "run" || ext == "com")) {
		prms = prms | perms::owner_exec | perms::group_exec | perms::others_exec;
	}
	if (sz) {
		*sz = static_cast<uintmax_t>(info->nFileSizeHigh) << (sizeof(info->nFileSizeHigh) * 8) | info->nFileSizeLow;
	}
	if (lwt) {
		*lwt = timeFromFILETIME(info->ftLastWriteTime);
	}
	return file_status(ft, prms);
}

inline file_status status_ex(const PurePath& p, std::error_code& ec, file_status* sls = nullptr, uintmax_t* sz = nullptr, uintmax_t* nhl = nullptr, time_t* lwt = nullptr, int recurse_count = 0) noexcept
{
    ec.clear();
#ifdef OS_WINDOWS
    if (recurse_count > 16) {
        std::cout << "error" << __LINE__ << std::endl;//ec = detail::make_system_error(0x2A9 /*ERROR_STOPPED_ON_SYMLINK*/);
        return file_status(file_type::unknown);
    }
    WIN32_FILE_ATTRIBUTE_DATA attr;
    if (!::GetFileAttributesExW(p.wstr().c_str(), GetFileExInfoStandard, &attr)) {
//        ec =         std::cout << "error" << __LINE__ << std::endl;;
    }
    else if (attr.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
//        auto reparseData = getReparseData(p, ec);
//        if (!ec && reparseData && IsReparseTagMicrosoft(reparseData->ReparseTag) && reparseData->ReparseTag == IO_REPARSE_TAG_SYMLINK) {
//			PurePath target = resolveSymlink(p, ec);
//            file_status result;
//            if (!ec && !target.empty()) {
//                if (sls) {
//                    *sls = status_from_INFO(p, &attr, ec);
//                }
//                return status_ex(target, ec, nullptr, sz, nhl, lwt, recurse_count + 1);
//            }
//            return file_status(file_type::unknown);
//        }
    }

    if (nhl) {
        *nhl = 0;
    }
    return status_from_INFO(p, &attr, ec, sz, lwt);
#else
    (void)recurse_count;
    struct ::stat st;
    auto result = ::lstat(p.c_str(), &st);
    if (result == 0) {
        ec.clear();
        file_status fs = detail::file_status_from_st_mode(st.st_mode);
        if (sls) {
            *sls = fs;
        }
        if (fs.type() == file_type::symlink) {
            result = ::stat(p.c_str(), &st);
            if (result == 0) {
                fs = detail::file_status_from_st_mode(st.st_mode);
            }
            else {
                ec =         std::cout << "error" << __LINE__ << std::endl;;
                if (detail::is_not_found_error(ec)) {
                    return file_status(file_type::not_found, perms::unknown);
                }
                return file_status(file_type::none);
            }
        }
        if (sz) {
            *sz = static_cast<uintmax_t>(st.st_size);
        }
        if (nhl) {
            *nhl = st.st_nlink;
        }
        if (lwt) {
            *lwt = st.st_mtime;
        }
        return fs;
    }
    else {
        ec =         std::cout << "error" << __LINE__ << std::endl;;
        if (detail::is_not_found_error(ec)) {
            return file_status(file_type::not_found, perms::unknown);
        }
        return file_status(file_type::none);
    }
#endif
}

class Path {
public:
    Path(PurePath& pp){
        this->_pp = pp;
    }
    PurePath _pp;
	struct stat_result {
		//os.stat_result(st_mode=33206, st_ino=9007199254766095, st_dev=3423312902, st_nlink=1, st_uid=0, st_gid=0, st_size=377733, st_atime=1624714487, st_mtime=1617370153, st_ctime=1617365121)
		int st_mode;
		//File mode: file type and file mode bits (permissions).
		int64_t st_ino;
		//Platform dependent, but if non-zero, uniquely identifies the file for a given value of st_dev. Typically:
		//the inode number on Unix, the file index on Windows
		int64_t st_dev;
		//Identifier of the device on which this file resides.
		int st_nlink; //Number of hard links.
		int64_t st_uid;//User identifier of the file owner.
		int64_t st_gid;//Group identifier of the file owner.
		int64_t st_size;//Size of the file in bytes, if it is a regular file or a symbolic link. The size of a symbolic link is the length of the pathname it contains, without a terminating null byte.
		int64_t st_atime;//Time of most recent access expressed in seconds.
		int64_t st_mtime;//Time of most recent content modification expressed in seconds.
		int64_t st_ctime;
		//Platform dependent:the time of most recent metadata change on Unix,
		//the time of creation on Windows, expressed in seconds.
		int64_t st_atime_ns;//Time of most recent access expressed in nanoseconds as an integer.
		int64_t st_mtime_ns;//Time of most recent content modification expressed in nanoseconds as an integer.
		int64_t st_ctime_ns;
		//Platform dependent:
		//the time of most recent metadata change on Unix,
		//the time of creation on Windows, expressed in nanoseconds as an integer.
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

    std::vector<char> read_bytes();

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

    bool fopen(const char* mode );
    int fclose();
    int fflush();
    void setbuf(char* buffer);
    int setvbuf(char* buffer, int mode, std::size_t size);

    std::size_t fread( void* buffer, std::size_t size, std::size_t count);
    std::size_t fwrite( const void* buffer, std::size_t size, std::size_t count);
    int fgetc();
    int fputc(int ch);
    char* fgets(char* str, int count);
    int fputs(const char* str);

    long ftell();
    int fgetpos(std::fpos_t* pos );
    int fseek(long offset, int origin);
    int fsetpos(const std::fpos_t* pos);
    void rewind();

    static int remove(const char* fname);
    static int rename(const char *old_filename, const char *new_filename);
    static std::FILE* tmpfile();
    static char* tmpnam( char* filename);

    static std::vector<char> readFile(const char *str);
    static std::vector<char> readFile2(FILE *fp, size_t size);
    static int writeFile(const char *str, const char *dataPtr, int size,
                  const char *mode = "rb+");
    static int writeFile2(FILE *f, const char *dataPtr, int size);
    FILE* fp = nullptr;
};

#endif //EM_PUREPATH_H
