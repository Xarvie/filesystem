//
// Created by caspase on 6/17/2021.
//

#include "PurePath.h"
#include <cstdio>

class StackBytes {
public:
    StackBytes() = delete;

    StackBytes(void *p) : Ptr(p) {

    }

    ~StackBytes() {
        _freea(Ptr);
    }

    void *Ptr;
};

#define __AsciiToWide(lpa) (___A2W((LPWSTR)StackBytes(_malloca(strlen(lpa)*2+1)).Ptr, lpa, (lstrlenA(lpa)+1), CP_ACP))

inline LPWSTR WINAPI ___A2W(LPWSTR lpw, LPCSTR lpa, int nChars, UINT acp) {
    return ((lpa && lpw) ? void(0) : abort()), lpw[0] = 0, MultiByteToWideChar(acp, 0, lpa, -1, lpw, nChars), lpw;
}

PurePath::PurePath() {

}

std::string trimstr(std::string s) {
    size_t n = s.find_last_not_of(" \r\n\t");
    if (n != std::string::npos) {
        s.erase(n + 1, s.size() - n);
    }
    n = s.find_first_not_of(" \r\n\t");
    if (n != std::string::npos) {
        s.erase(0, n);
    }
    return s;
}

void PurePath::toSystemSeparator() {
    for (auto &E :_path) {
        if (E == '\\' || E == '/')
            E = preferred_separator;
    }
}

//void toSystemSeparator2(std::string &str) {
//    for (auto &E :str) {
//        if (E == '\\' || E == '/')
//            E = PurePath::preferred_separator;
//    }
//}

void toWindowsSeparator(std::string &str) {
    for (auto &E :str) {
        if (E == '/')
            E = PurePath::preferred_separator;
    }
}

void toPosixSeparator(std::string &str) {
    for (auto &E :str) {
        if (E == '\\')
            E = PurePath::posix_separator;
    }
}

PurePath::PurePath(const std::string &c) {
    PurePath::NormalRet ret;
    PurePath::join(ret.strs, ret.wstrs, ret.protoHead, ret.protoType, c);
    this->parts = ret.strs;
    this->protoType = ret.protoType;
    this->protoHead = ret.protoHead;
}

void parse(const std::string &_str) {
    std::string pathStr = _str;
}

bool isAbs(const PurePath::NormalRet &ret) {
    return ret.protoType != PurePath::PROTO_TYPE_RELATIVE;
}

PurePath::PurePath(const char *c) {
    PurePath::NormalRet ret;
    PurePath::join(ret.strs, ret.wstrs, ret.protoHead, ret.protoType, std::string(c));
    this->parts = ret.strs;
    this->parts_w = ret.wstrs;
    this->protoType = ret.protoType;
    this->protoHead = ret.protoHead;
}

std::string PurePath::drive() const {

}

std::string PurePath::root() const {
}

std::string PurePath::anchor() const {

}

std::vector<std::string> PurePath::parents() const {

}

std::string PurePath::parent() const {

}

std::string PurePath::name() const {
    int k = -1;
    switch (protoType) {
        case PROTO_TYPE_UNC: {
            k = 2;
            break;
        }
        case PROTO_TYPE_FILE_URI_WINDOWS: {
            for (int i = parts.size() - 1; i >= 0; i--) {
                if (std::string::npos != parts[i].find_first_of("/:\\")) {
                    k = i;
                    break;
                }
            }
            break;
        }
        case PROTO_TYPE_FILE_URI_UNIX:
            for (int i = parts.size() - 1; i >= 0; i--) {
                if (std::string::npos != parts[i].find_first_of("/:\\")) {
                    k = i;
                    break;
                }
            }
            break;
        case PROTO_TYPE_HTTP:
            k = 1;
            break;
        case PROTO_TYPE_HTTPS:
            k = 1;
            break;
        case PROTO_TYPE_SFTP:
            k = 1;
            break;
        case PROTO_TYPE_FTP:
            k = 1;
            break;
        case PROTO_TYPE_WIN_DRIVER:
            k = 0;
            break;
        case PROTO_TYPE_WIN_ABS:
            for (int i = parts.size() - 1; i >= 0; i--) {
                if (std::string::npos != parts[i].find_first_of("/:\\")) {
                    k = i;
                    break;
                }
            }
            break;
        case PROTO_TYPE_UNIX_ABS:
            for (int i = parts.size() - 1; i >= 0; i--) {
                if (std::string::npos != parts[i].find_first_of("/:\\")) {
                    k = i;
                    break;
                }
            }
            break;
        case PROTO_TYPE_RELATIVE:
            for (int i = parts.size() - 1; i >= 0; i--) {
                if (std::string::npos != parts[i].find_first_of("/:\\")) {
                    k = i;
                    break;
                }
            }
            break;
        default:
            break;

    }

    int i = parts.size() - 1;
    for (; i > k; i--) {
        return parts[i];
    }
    return "";
}

//-----------------------------------------------------------------------------
// [fs.class.file_status] class file_status
// [fs.file_status.cons] constructors and destructor
file_status::file_status() noexcept
        : file_status(file_type::none) {
}

file_status::file_status(file_type ft, int16_t prms) noexcept
        : _type(ft), _perms(prms) {
}

file_status::file_status(const file_status &other) noexcept
        : _type(other._type), _perms(other._perms) {
}

file_status::file_status(file_status &&other) noexcept
        : _type(other._type), _perms(other._perms) {
}

file_status::~file_status() {}

// assignments:
file_status &file_status::operator=(const file_status &rhs) noexcept {
    _type = rhs._type;
    _perms = rhs._perms;
    return *this;
}

file_status &file_status::operator=(file_status &&rhs) noexcept {
    _type = rhs._type;
    _perms = rhs._perms;
    return *this;
}

// [fs.file_status.mods] modifiers
void file_status::type(file_type ft) noexcept {
    _type = ft;
}

void file_status::permissions(int16_t prms) noexcept {
    _perms = prms;
}

// [fs.file_status.obs] observers
file_type file_status::type() const noexcept {
    return _type;
}

int16_t file_status::permissions() const noexcept {
    return _perms;
}

std::string PurePath::suffix() const {
    std::string name_ = name();
    if (name_.empty())
        return "";

    int pos = name_.find_last_of('.');
    if (pos == name_.npos)
        return "";
    else
        return name_.substr(pos, name_.length() - pos);
}

std::vector<std::string> PurePath::suffixes() const {

}

std::string PurePath::stem() const {

}

void updateParts(const std::vector<std::string> &s1, std::vector<std::wstring> &s2) {
    s2.clear();
    for (auto &E: s1) {
        do {
            s2.emplace_back(__AsciiToWide(E.c_str()));
        } while (false);
    }
}

PurePath PurePath::operator+(const PurePath &path) {
    auto ret = path;
    join(ret.parts, ret.parts_w, ret.protoHead, ret.protoType, path);
    return ret;
}

PurePath &PurePath::operator+=(const PurePath &path) {
    join(this->parts, this->parts_w, this->protoHead, this->protoType, path);
    return *this;
}

std::wstring PurePath::wstr() const {
    std::wstring ret = __AsciiToWide(str().c_str());
    return ret;
}

PurePath::PurePath(const PurePath &c) {
    this->_path = c._path;
}

void
PurePath::join(std::vector<std::string> &strs, std::vector<std::wstring> &wstrs, std::string &protoHead,
               ProtoType &protoType, const std::string &s) {
    auto ret = normal(s);
    if (ret.strs.empty())
        return;
    if (strs.empty()) {
        strs = ret.strs;
        updateParts(strs, wstrs);
        protoHead = ret.protoHead;
        protoType = ret.protoType;
    } else if (isAbs(ret)) {
        return;
    } else {
        strs.insert(strs.end(), ret.strs.begin(), ret.strs.end());
        updateParts(strs, wstrs);
    }
}

void
PurePath::join(std::vector<std::string> &strs, std::vector<std::wstring> &wstrs, std::string &protoHead,
               ProtoType &protoType, const PurePath &path) {
    PurePath::NormalRet ret;
    ret.strs = path.parts;
    ret.protoType = path.protoType;
    ret.protoHead = path.protoHead;
    if (ret.strs.empty())
        return;
    if (strs.empty()) {
        strs = ret.strs;
        protoHead = ret.protoHead;
        protoType = ret.protoType;
        updateParts(strs, wstrs);
    } else if (isAbs(ret)) {
        return;
    } else {
        strs.insert(strs.end(), ret.strs.begin(), ret.strs.end());
        updateParts(strs, wstrs);
    }
}

size_t PurePath::root_name_length() const noexcept {
#ifdef OS_WINDOWS
    if (_path.length() >= _prefixLength + 2 && std::toupper(static_cast<unsigned char>(_path[_prefixLength])) >= 'A' &&
        std::toupper(static_cast<unsigned char>(_path[_prefixLength])) <= 'Z' && _path[_prefixLength + 1] == ':') {
        return 2;
    }
#endif
    if (_path.length() > _prefixLength + 2 && _path[_prefixLength] == preferred_separator &&
        _path[_prefixLength + 1] == preferred_separator && _path[_prefixLength + 2] != preferred_separator &&
        std::isprint(_path[_prefixLength + 2])) {
        std::string::size_type pos = _path.find(preferred_separator, _prefixLength + 3);
        if (pos == std::string::npos) {
            return _path.length();
        } else {
            return pos;
        }
    }
    return 0;
}

bool PurePath::has_root_directory() const {
    auto rootLen = _prefixLength + root_name_length();
    return (_path.length() > rootLen && _path[rootLen] == preferred_separator);
}

bool PurePath::is_absolute() const {
    return protoType != PROTO_TYPE_RELATIVE;
}

bool PurePath::empty() const noexcept {
    return _path.empty();
}

bool PurePath::is_relative() const {
    return !is_absolute();
}

bool PurePath::has_root_name() const {
    return root_name_length() > 0;
}

#ifdef OS_WINDOWS

std::error_code make_system_error(uint32_t err) {
    return std::error_code(err ? static_cast<int>(err) : static_cast<int>(::GetLastError()), std::system_category());
}

#else
std::error_code make_system_error(int err)
    {
        return std::error_code(err ? err : errno, std::system_category());
    }
#endif

PurePath PurePath::current_path() {
//    ec.clear();
#ifdef OS_WINDOWS
    DWORD pathlen = ::GetCurrentDirectory(0, 0);
    std::unique_ptr<char[]> buffer(new char[size_t(pathlen) + 1]);
    if (::GetCurrentDirectory(pathlen, buffer.get()) == 0) {
//        ec = make_system_error(__LINE__);
        return PurePath();
    }
    return PurePath(std::string(buffer.get()));
#else
    size_t pathlen = static_cast<size_t>(std::max(int(::pathconf(".", _PC_PATH_MAX)), int(PATH_MAX)));
    std::unique_ptr<char[]> buffer(new char[pathlen + 1]);
    if (::getcwd(buffer.get(), pathlen) == nullptr) {
        ec = detail::make_system_error();
        return path();
    }
    return path(buffer.get());
#endif
}

bool PurePath::current_path(const PurePath &p) noexcept {
//
//#ifdef OS_WINDOWS
//    if (!::SetCurrentDirectory(p.c_str())) {
//        return false;
//    }
//#else
//    if (::chdir(p.string().c_str()) == -1) {
//       return false;
//    }
//#endif

    return true;
}


std::string PurePath::basename() const {
//    if (empty())
//        return "";
//    //auto& path2 = this->_path;
//    auto path2 = PurePath::normal(this->_path);
//    size_t i = path2.length() - 1;
//    if (path2[i] == '.') {
//        while (i-- > 0) {
//            if (path2[i] != preferred_separator && path2[i] != '.') {
//                int pos = path2.find_last_of(preferred_separator);
//                std::string s(path2.substr(pos + 1));
//                return s;
//            }
//
//        }
//    }
//    return path2;
}

//PurePath PurePath::abspath(const PurePath &p) {
//
//#ifdef OS_WINDOWS
//    if (p.empty()) {
//        return PurePath().joinpath(abspath(current_path()), PurePath(""));
//    }
//    ULONG size = ::GetFullPathName(p.c_str(), 0, 0, 0);
//    if (size) {
//        std::vector<char_type> buf(size, 0);
//        ULONG s2 = GetFullPathName(p.c_str(), size, buf.data(), nullptr);
//        if (s2 && s2 < size) {
//            PurePath result = PurePath(std::string(buf.data(), s2));
//            if (p.basename() == ".") {
//                result = PurePath().joinpath(result, PurePath("."));
//            }
//            return result;
//        }
//    }
//    return PurePath();
//#else
//    path base = current_path(ec);
//    if (!ec) {
//        if (p.empty()) {
//            return base / p;
//        }
//        if (p.has_root_name()) {
//            if (p.has_root_directory()) {
//                return p;
//            }
//            else {
//                return p.root_name() / base.root_directory() / base.relative_path() / p.relative_path();
//            }
//        }
//        else {
//            if (p.has_root_directory()) {
//                return base.root_name() / p;
//            }
//            else {
//                return base / p;
//            }
//        }
//    }
//    ec = detail::make_system_error();
//    return path();
//#endif
//}

void splitWithStl(const std::string &str, const char &pattern, std::vector<std::string> &resVec) {
    if (str.empty()) {
        return;
    }
    std::string strs = str + pattern;

    size_t pos = strs.find(pattern);
    size_t size = strs.size();

    while (pos != std::string::npos) {
        std::string x = strs.substr(0, pos);
        resVec.push_back(x);
        strs = strs.substr(pos + 1, size);
        pos = strs.find(pattern);
    }


}

PurePath::NormalRet PurePath::normal(const std::string &s) {
    PurePath::NormalRet ret;
    auto path = trimstr(s);
    if (s.empty())
        return ret;
    toPosixSeparator(path);
    //
    std::string disk = "";
    std::string root = "";
    std::string unc = "";

    std::vector<std::string> tmpParts;
    tmpParts.clear();
    if (path.length() > 1 && path[0] == posix_separator && path[1] == posix_separator) {
        //UNC      \\a\b
        tmpParts.push_back({posix_separator, posix_separator});
        path = path.substr(2);
        ret.protoType = PROTO_TYPE_UNC;

    } else if (path.find(std::string("file:") + posix_separator + posix_separator + posix_separator) == 0) {
        //  file:///c:/Windows
        path = path.substr(7);
        tmpParts.push_back(std::string("file:"));


        do {
            if (path.length() > 3) {
                if ((path[1] >= 'a' && path[1] <= 'z' || path[1] >= 'A' && path[1] <= 'Z') && path[2] == ':') {
                    disk = {path[1], path[2]};//"c:"
                    tmpParts.push_back({path[1], path[2]});
                    path = path.substr(3);
                    if (path.length() > 0 && path[0] == posix_separator) {
                        tmpParts.push_back({path[0]});
                        path = path.substr(1);
                    } else {
                        return ret; // abs only!
                    }
                    ret.protoType = PROTO_TYPE_FILE_URI_WINDOWS;
                    break;
                }
            }

            if (path.length() > 0) {
                if (path[0] == '/') {
                    tmpParts.push_back({path[0]});
                    path = path.substr(1);
                    ret.protoType = PROTO_TYPE_FILE_URI_UNIX;
                } else {
                    return ret;// abs only!
                }
            }
        } while (false);
    } else if (path.find(std::string("http:") + posix_separator + posix_separator) == 0) {
        tmpParts.push_back(std::string("http:") + posix_separator + posix_separator);
        path = path.substr(7);
        ret.protoType = PROTO_TYPE_HTTP;
        //  http://a.com/

    } else if (path.find(std::string("https:") + posix_separator + posix_separator) == 0) {
        tmpParts.push_back(std::string("https:") + posix_separator + posix_separator);
        path = path.substr(8);
        ret.protoType = PROTO_TYPE_HTTPS;
        //  https://a.com/

    } else if (path.find(std::string("sftp:") + posix_separator + posix_separator) == 0) {
        //  sftp://
        path = path.substr(7);
        tmpParts.push_back(std::string("sftp:") + posix_separator + posix_separator);
        ret.protoType = PROTO_TYPE_SFTP;
    } else if (path.find(std::string("ftp:") + posix_separator + posix_separator) == 0) {
        //  ftp://
        tmpParts.push_back(std::string("ftp:") + posix_separator + posix_separator);
        path = path.substr(6);
        ret.protoType = PROTO_TYPE_FTP;
    } else if (path.length() > 1 &&
               ((path[0] >= 'a' && path[0] <= 'z' || path[0] >= 'A' && path[0] <= 'Z') && path[1] == ':')) {
        {
            disk = {path[0], path[1]};//"c:"
            tmpParts.push_back({path[0], path[1]});
            path = path.substr(2);
            ret.protoType = PROTO_TYPE_WIN_DRIVER;
            if (path.length() > 0 && path[0] == posix_separator) {
                //"c:\"
                tmpParts.push_back({path[0]});
                path = path.substr(1);
                ret.protoType = PROTO_TYPE_WIN_ABS;
            }
        }
    } else if (path[0] == posix_separator) {
        //      \a\b\c
        tmpParts.push_back({path[0]});
        path = path.substr(1);
        ret.protoType = PROTO_TYPE_UNIX_ABS;
    }

    std::vector<std::string> vec;
    splitWithStl(path, posix_separator, vec);

    std::string end_posix_separator = "";

    for (auto it = vec.begin(); it != vec.end();) {

        if ((*it == "." /*&& it != vec.begin()*/) || (*it).empty()) {
            it = vec.erase(it++);
            continue;
        } else {
            it++;
        }
    }

    std::vector<std::string> retVec;
    for (auto i = 0; i < vec.size(); i++) {
        if (vec[i] == ".." && i == 0) {
            retVec.push_back(vec[i]);
            continue;
        }
        if (vec[i] == "..") {
            if (i - 1 >= 0 && vec[i - 1] != ".." && vec[i - 1] != "\\" && vec[i - 1] != ".") {

                retVec.pop_back();
                continue;
            }
            if (retVec.size() >= 2) {

                if (retVec[retVec.size() - 1 - 1] == "." && retVec.size() - 1 - 1 != 0) {
                    retVec.pop_back();
                    retVec.pop_back();
                    continue;
                }

            } else {

            }
            retVec.push_back(vec[i]);
            continue;
        } else {
            if (!vec[i].empty())
                retVec.push_back(vec[i]);
        }
    }
    for (auto &E: retVec)
        tmpParts.push_back(E);
    ret.strs = tmpParts;
    ret.protoType = ret.protoType;
    ret.protoHead = ret.protoHead;
//    if (ret.protoType == PROTO_TYPE_RELATIVE && !ret.strs.empty())
//        ret.protoType = PROTO_TYPE_RELATIVE;
    return ret;
    std::string retStr;
    for (auto &E: retVec) {
        retStr = retStr + E;
        if (retStr[retStr.length() - 1] != posix_separator)
            retStr += posix_separator;
    }
    while (!retStr.empty() && retStr[retStr.length() - 1] == posix_separator) {
        retStr.pop_back();
    }

}

std::string PurePath::commonprefix(const std::vector<PurePath> &strs) {
//    if (strs.empty()) return "";
//#ifdef OS_WINDOWS
//    std::vector<PurePath> as = strs;
//    for (auto &E :as) {
//        E._path = normal(E._path);
//        std::for_each(E._path.begin(), E._path.end(), [](char &c) {
//            c = ::tolower(static_cast<unsigned char>(c));
//        });
//    }
//#else
//    const std::vector<path>& as = strs;
//#endif
//    std::string res = as[0]._path;
//    for (auto &s :as) {
//        for (int i = 0; i < res.size(); i++) {
//            if (s._path[i] != res[i]) {
//                res = res.substr(0, i);
//                break;
//            }
//        }
//    }
//    return res;
}

std::string PurePath::dirname(const std::string &strs) {
//    PurePath x(strs);
//    auto str = normal(x._path);
//
//    if (str.empty()) return "";
//    while (str[str.length() - 1] == preferred_separator)
//        str.pop_back();
//
//    int pos = str.find_last_of(preferred_separator);
//    if (pos == str.npos)
//        return "";
//    else
//        return str.substr(0, pos + 1);

}

#include <direct.h>

BOOL CheckDiskExist(const std::string &path) {
    ULONG uDriveMask = _getdrives();
    if (uDriveMask == 0) {
        return FALSE;
    }

    uDriveMask >>= (int) (path[0] - 'A');
    if (uDriveMask & 1) {
        return TRUE;
    }
    return FALSE;
}

bool FileIsExist(const char *pPath) {
    std::string retStr = PurePath(pPath)._path;
    if (retStr.length() == 2 && isalpha(retStr[0]) && retStr[1] == ':') {
        return CheckDiskExist(retStr);
    }
//    if (_access(pPath,0) != 0)
//        return TPI_ERR_DIRNOEXIST;

    WIN32_FIND_DATA finddata;
    HANDLE hfile = FindFirstFile(retStr.c_str(), &finddata);
    if (hfile == INVALID_HANDLE_VALUE) {
        return false;
    }
    FindClose(hfile);
    return true;
}

bool PurePath::exists(const std::string &strs) {
#ifdef OS_WINDOWS
    return FileIsExist(strs.c_str());
#else

    return FileIsExist(strs.c_str());
#endif
}

std::string &replace_all_ex(std::string &str, const std::string &old_value, const std::string &new_value) {
    while (true) {
        std::string::size_type pos(0);
        if ((pos = str.find(old_value)) != std::string::npos)
            str.replace(pos, old_value.length(), new_value);
        else break;
    }
    return str;
}

std::string &replace_all(std::string &str, const std::string &old_value, const std::string &new_value) {
    for (std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length()) {
        if ((pos = str.find(old_value, pos)) != std::string::npos)
            str.replace(pos, old_value.length(), new_value);
        else break;
    }
    return str;
}

std::string PurePath::expanduser(const std::string &pPath) {
    std::string retStr = PurePath(pPath)._path;

    if (retStr.find_first_of("~") == retStr.npos && retStr.find_first_of("%userprofile%") == retStr.npos)
        return retStr;

    if (retStr.empty())
        return "";

    std::vector<std::string> vec;
    bool firstDot = false;
    if (retStr[0] == preferred_separator) {
        vec.push_back(std::string() += preferred_separator);
    }
    splitWithStl(retStr, preferred_separator, vec);


    std::string home = (getenv("USERPROFILE"));
    if (home.empty())
        return retStr;
    std::string retstr2 = "";
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] == "~")
            vec[i] = home;
        else if (vec[i] == "%userprofile%")
            vec[i] = home;
        if (i != vec.size() - 1)
            retstr2 = retstr2 + vec[i] + preferred_separator;
        else
            retstr2 = retstr2 + vec[i];
    }

    return retstr2;

#ifdef OS_WINDOWS

#else

#endif
}

std::string PurePath::expandvars(const std::string &pPath) {
    std::string retStr = PurePath(pPath)._path;

    if (retStr.find_first_of("~") == retStr.npos && retStr.find_first_of("%userprofile%") == retStr.npos)
        return retStr;

    if (retStr.empty())
        return "";

    std::vector<std::string> vec;
    bool firstDot = false;
    if (retStr[0] == preferred_separator) {
        vec.push_back(std::string() += preferred_separator);
    }
    splitWithStl(retStr, preferred_separator, vec);


    std::string home = (getenv("USERPROFILE"));
    if (home.empty())
        return retStr;
    std::string retstr2 = "";
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] == "~")
            vec[i] = home;
        else if (vec[i] == "%userprofile%")
            vec[i] = home;
        if (i != vec.size() - 1)
            retstr2 = retstr2 + vec[i] + preferred_separator;
        else
            retstr2 = retstr2 + vec[i];
    }

    return retstr2;

#ifdef OS_WINDOWS

#else

#endif
}



//=========================================================================


std::string PurePath::as_posix() {

}

std::string PurePath::as_uri() {

}

bool PurePath::is_relative_to(const std::string &other) {
}

bool PurePath::is_reserved() {
}

std::string PurePath::match(const std::string &str) {
}

std::string PurePath::relative_to(const std::string &str) {
}

std::string PurePath::with_name(const std::string &str) {
}

std::string PurePath::with_stem(const std::string &str) {
}

std::string PurePath::with_suffix(const std::string &str) {

}


//===============================================================




Path::stat_result Path::stat() {
    PurePath rr = R"(C:\Users\ftp\Desktop\SecureCRT_win64.7z)";
    std::cout << rr.name() << std::endl;
    std::cout << rr.suffix() << std::endl;
    std::error_code ec;
    file_status sls;
    uintmax_t sz;
    uintmax_t nhl;
    time_t lwt;
    status_ex(rr, ec, &sls, &sz, &nhl, &lwt, 0);
}

bool chmod(int mode) {//改变文件的模式和权限
}

std::string Path::exists() {//文件是否存在
}

std::string Path::expanduser() {//返回展开了包含 ~ 和 ~user 的构造
}

std::string Path::glob(const std::string &pattern) {//解析相对于此路径的通配符 pattern，产生所有匹配的文件
}

std::string Path::group() {//返回拥有此文件的用户组。如果文件的 GID 无法在系统数据库中找到，将抛出 KeyError 。
}

std::string
Path::is_dir() {//如果路径指向一个目录（或者一个指向目录的符号链接）则返回 True，如果指向其他类型的文件则返回 False。当路径不存在或者是一个破损的符号链接时也会返回 False；其他错误（例如权限错误）被传播。
}

std::string
Path::is_file() {//如果路径指向一个正常的文件（或者一个指向正常文件的符号链接）则返回 True，如果指向其他类型的文件则返回 False。 当路径不存在或者是一个破损的符号链接时也会返回 False；其他错误（例如权限错误）被传播
}

std::string
Path::is_mount() {//如果路径是一个 挂载点 <mount point>：在文件系统中被其他不同的文件系统挂载的地点。在 POSIX 系统，此函数检查 path 的父级 —— path/.. 是否处于一个和 path 不同的设备中，或者 file:path/.. 和 path 是否指向相同设备的相同 i-node —— 这能检测所有 Unix 以及 POSIX 变种上的挂载点。 Windows 上未实现。
}

std::string Path::is_symlink() {//如果路径指向符号链接则返回 True， 否则 False。如果路径不存在也返回 False；其他错误（例如权限错误）被传播。
}

std::string
Path::is_socket() { //如果路径指向一个 Unix socket 文件（或者指向 Unix socket 文件的符号链接）则返回 True，如果指向其他类型的文件则返回 False。当路径不存在或者是一个破损的符号链接时也会返回 False；其他错误（例如权限错误）被传播。
}

std::string
Path::is_fifo() {//如果路径指向一个先进先出存储（或者指向先进先出存储的符号链接）则返回 True ，指向其他类型的文件则返回 False。当路径不存在或者是一个破损的符号链接时也会返回 False；其他错误（例如权限错误）被传播。
}

std::string
Path::is_block_device() {//如果文件指向一个块设备（或者指向块设备的符号链接）则返回 True，指向其他类型的文件则返回 False。当路径不存在或者是一个破损的符号链接时也会返回 False；其他错误（例如权限错误）被传播。
}

std::string
Path::is_char_device() {//如果路径指向一个字符设备（或指向字符设备的符号链接）则返回 True，指向其他类型的文件则返回 False。当路径不存在或者是一个破损的符号链接时也会返回 False；其他错误（例如权限错误）被传播。
}

std::string Path::iterdir() {
}

std::string Path::lchmod(int mode) {
}

std::string Path::lstat() {
}

std::string Path::mkdir(int mode, bool parents, bool exist_ok) {
}

std::string Path::open(int mode, int buffering, void *encoding, void *errors, void *newline) {
}

std::string Path::owner() {
}

std::vector<char> Path::read_bytes() {
    this->readFile(this->_pp.str().c_str());
}

std::string Path::read_text(void *encoding, void *errors) {
}

std::string Path::readlink() {
}

std::string Path::rename(const std::string &target) {
}

std::string Path::replace(const std::string &target) {
}

std::string Path::resolve(bool strict) {
}

std::string Path::rglob(const std::string &pattern) {
}

std::string Path::rmdir() {
}

std::string Path::samefile(const std::string &other_path) {
}

std::string Path::symlink_to(const std::string &target, bool target_is_directory) {
}

std::string Path::link_to(const std::string &target) {
}

std::string Path::touch(int mode, bool exist_ok) {
}

std::string Path::unlink(bool missing_ok) {
}

std::string Path::write_bytes(std::vector<char> &data) {
}

std::string Path::write_text(const std::string &data, void *encoding, void *errors) {
}

std::string Path::cwd() {
}

std::string Path::home() {

}

bool Path::fopen(const char *mode) {
    if (this->fp != nullptr) {
        std::fclose(this->fp);
        this->fp = nullptr;
    }

    std::cout << this->_pp.str().c_str() << std::endl;
    FILE *tmpFP = std::fopen(this->_pp.str().c_str(), mode);
    if (tmpFP == nullptr)
        return false;
    this->fp = tmpFP;
    return true;
}

int Path::fclose() {
    if (this->fp != nullptr) {
        auto ret = std::fclose(this->fp);
        if (ret != EOF) {
            this->fp = nullptr;
            return ret;
        }
        return ret;
    }
    return EOF;
}

int Path::fflush() {
    if (this->fp != nullptr) {
        auto ret = std::fflush(this->fp);
        if (ret != EOF) {
            this->fp = nullptr;
            return ret;
        }
        return ret;
    }
    return EOF;
}

void Path::setbuf(char *buffer) {
    std::setbuf(this->fp, buffer);
}

int Path::setvbuf(char *buffer, int mode, std::size_t size) {
    return std::setvbuf(this->fp, buffer, mode, size);
}

std::size_t Path::fread(void *buffer, std::size_t size, std::size_t count) {
    return std::fread(buffer, size, count, this->fp);

}

std::size_t Path::fwrite(const void *buffer, std::size_t size, std::size_t count) {
    return std::fwrite(buffer, size, count, this->fp);
}

int Path::fgetc() {
    return std::fgetc(this->fp);
}

int Path::fputc(int ch) {
    return std::fputc(ch, this->fp);
}

char *Path::fgets(char *str, int count) {
    return std::fgets(str, count, this->fp);
}

int Path::fputs(const char *str) {
    return std::fputs(str, this->fp);

}

long Path::ftell() {
    return std::ftell(this->fp);
}

int Path::fgetpos(std::fpos_t *pos) {
    return std::fgetpos(this->fp, pos);
}

int Path::fseek(long offset, int origin) {
    return std::fseek(this->fp, offset, origin);
}

int Path::fsetpos(const std::fpos_t *pos) {
    return std::fsetpos(this->fp, pos);
}

void Path::rewind() {
    std::rewind(this->fp);
}

int Path::remove(const char *fname) {
    return std::remove(fname);
}

int Path::rename(const char *old_filename, const char *new_filename) {
    return std::rename(old_filename, new_filename);
}

std::FILE *Path::tmpfile() {
    return std::tmpfile();
}

char *Path::tmpnam(char *filename) {
    return std::tmpnam(filename);
}

std::vector<char> Path::readFile(const char *str) {
    FILE *f = std::fopen(str, "rb");
    if (f == NULL)
        return std::vector<char>();
    if (std::fseek(f, 0, SEEK_END))
    {
        std::fclose(f);
        return std::vector<char>();
    }
    size_t size = std::ftell(f);
    if (std::fseek(f, 0, SEEK_SET))
    {
        std::fclose(f);
        return std::vector<char>();
    }
    if (size == -1L || size == 0)
    {
        std::fclose(f);
        return std::vector<char>();
    }
    std::vector<char> vecData_(size);
    {
        char *ptr = vecData_.data();
        int64_t allSize = size;
        int wlen = allSize;
        const int flushBytes = 1024;
        if(allSize % flushBytes) {
            int ret = std::fread(ptr, allSize % flushBytes, 1, f);
            if (ret != 1)
            {
                std::fclose(f);
                return std::vector<char>();
            }
            wlen -= allSize % flushBytes;
        }

        if (wlen > 0)
            while (true) {
                int ret = std::fread(ptr + allSize - wlen, flushBytes, 1, f);
                if (ret > 0) {
                    wlen -= flushBytes;
                    if (wlen == 0)
                        break;
                }
                else{
                    std::fclose(f);
                    return std::vector<char>();
                }
            }
    }

    std::fclose(f);
    return vecData_;
}

std::vector<char> Path::readFile2(FILE *fp, size_t size) {
    if (fp == NULL)
        return std::vector<char>();
    if (size == -1L || size == 0)
        return std::vector<char>();
    std::vector<char> vecData_(size);
    {
        char *ptr = vecData_.data();
        int64_t allSize = size;
        int wlen = allSize;
        const int flushBytes = 1024;
        if(allSize % flushBytes) {
            int ret = std::fread(ptr, allSize % flushBytes, 1, fp);
            if (ret != 1)
            {
                return std::vector<char>();
            }
            wlen -= allSize % flushBytes;
        }

        if (wlen > 0)
            while (true) {
                int ret = std::fread(ptr + allSize - wlen, flushBytes, 1, fp);
                if (ret > 0) {
                    wlen -= flushBytes;
                    if (wlen == 0)
                        break;
                }
                else{
                    return std::vector<char>();
                }
            }
    }
    return vecData_;
}

int Path::writeFile(const char *str, const char *dataPtr, int size,
                    const char *mode) {//rb+ write wb+ cleanwrite ab+ append
    FILE *f = std::fopen(str, mode);
    if (f == NULL || size < 0)
        return -1;
    if (size > 0) {
        const char *ptr = dataPtr;
        int64_t allSize = size;
        int wlen = allSize;
        int flushBytes = 1024;
        if(allSize % flushBytes) {
            int ret = std::fwrite(ptr, allSize % flushBytes, 1, f);
            if (ret != 1) {
                std::fclose(f);
                return -1;
            }
            wlen -= allSize % flushBytes;
        }

        if (wlen > 0)
            while (true) {
                int ret = std::fwrite(ptr + allSize - wlen, flushBytes, 1, f);
                if (ret > 0) {
                    wlen -= flushBytes;
                    if (wlen == 0)
                        break;
                }
                else{
                    std::fclose(f);
                    return -1;

                }
            }
    }
    std::fclose(f);
    return 0;
}

int Path::writeFile2(FILE *f, const char *dataPtr, int size) {//rb+ write wb+ cleanwrite ab+ append
    if (f == NULL || size < 0)
        return -1;
    if (size > 0) {
        const char *ptr = dataPtr;
        int64_t allSize = size;
        int wlen = allSize;
        int flushBytes = 1024;
        if(allSize % flushBytes) {
            int ret = std::fwrite(ptr, allSize % flushBytes, 1, f);
            if (ret != 1) {
                std::fclose(f);
                return -1;
            }
            wlen -= allSize % flushBytes;
        }

        if (wlen > 0)
            while (true) {
                int ret = std::fwrite(ptr + allSize - wlen, flushBytes, 1, f);
                if (ret > 0) {
                    wlen -= flushBytes;
                    if (wlen == 0)
                        break;
                }
                else{
                    std::fclose(f);
                    return -1;

                }
            }
    }
    return 0;
}