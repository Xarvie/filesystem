//
// Created by caspase on 6/17/2021.
//

#include "PurePath.h"

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

PurePath::PurePath(const std::string &c) {
    this->_path = c;
    toSystemSeparator();
    this->_path = trimstr(this->_path);
    while (!_path.empty() && _path[_path.length() - 1] == PurePath::preferred_separator) {
        _path.pop_back();
    }
    std::for_each(_path.begin(), _path.end(), [](char &c) {
        c = ::tolower(static_cast<unsigned char>(c));
    });
}

PurePath::PurePath(const char *c) {
    this->_path = c;
    toSystemSeparator();
    this->_path = trimstr(this->_path);
    while (_path[_path.length() - 1] == PurePath::preferred_separator && _path.length() > 1) {
        _path.pop_back();
    }
    std::for_each(_path.begin(), _path.end(), [](char &c) {
        c = ::tolower(static_cast<unsigned char>(c));
    });
}

PurePath::PurePath(const PurePath &c) {
    this->_path = c._path;
}


std::string PurePath::join(const std::string &a, const std::string &p) {
    std::string _path = a;
    if (_path.empty()) {
        _path = p;
        return _path;
    }
    if (!_path.empty() && _path[_path.length() - 1] != PurePath::preferred_separator /*&& _path[_path.length() - 1] != ':'*/) {
        _path += PurePath::preferred_separator;
    }
    if (p.empty()) {
        // was: if ((!has_root_directory() && is_absolute()) || has_filename())
        return _path;
    }

    _path += p;
    return _path;
}

PurePath &PurePath::joinpath_(const PurePath &p) {
    if (this->empty()) {
        *this = p;
        return *this;
    }
    if (!_path.empty() && _path[_path.length() - 1] != preferred_separator /*&& _path[_path.length() - 1] != ':'*/) {
        _path += preferred_separator;
    }
    if (p.empty()) {
        // was: if ((!has_root_directory() && is_absolute()) || has_filename())
        return *this;
    }

    this->_path += p._path;
    return *this;
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
#ifdef OS_WINDOWS
    return has_root_name() && has_root_directory();
#else
    return has_root_directory();
#endif
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

const PurePath::char_type *PurePath::c_str() const noexcept {
    return this->_path.c_str();
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

#ifdef OS_WINDOWS
    if (!::SetCurrentDirectory(p.c_str())) {
        return false;
    }
#else
    if (::chdir(p.string().c_str()) == -1) {
       return false;
    }
#endif

    return true;
}


std::string PurePath::basename() const {
    if (empty())
        return "";
    //auto& path2 = this->_path;
    auto path2 = PurePath::normal(this->_path);
    size_t i = path2.length() - 1;
    if (path2[i] == '.') {
        while (i-- > 0) {
            if (path2[i] != preferred_separator && path2[i] != '.') {
                int pos = path2.find_last_of(preferred_separator);
                std::string s(path2.substr(pos + 1));
                return s;
            }

        }
    }
    return path2;
}

PurePath PurePath::abspath(const PurePath &p) {

#ifdef OS_WINDOWS
    if (p.empty()) {
        return PurePath().joinpath(abspath(current_path()), PurePath(""));
    }
    ULONG size = ::GetFullPathName(p.c_str(), 0, 0, 0);
    if (size) {
        std::vector<char_type> buf(size, 0);
        ULONG s2 = GetFullPathName(p.c_str(), size, buf.data(), nullptr);
        if (s2 && s2 < size) {
            PurePath result = PurePath(std::string(buf.data(), s2));
            if (p.basename() == ".") {
                result = PurePath().joinpath(result, PurePath("."));
            }
            return result;
        }
    }
    return PurePath();
#else
    path base = current_path(ec);
    if (!ec) {
        if (p.empty()) {
            return base / p;
        }
        if (p.has_root_name()) {
            if (p.has_root_directory()) {
                return p;
            }
            else {
                return p.root_name() / base.root_directory() / base.relative_path() / p.relative_path();
            }
        }
        else {
            if (p.has_root_directory()) {
                return base.root_name() / p;
            }
            else {
                return base / p;
            }
        }
    }
    ec = detail::make_system_error();
    return path();
#endif
}

void splitWithStl(const std::string &str, const char &pattern, std::vector<std::string> &resVec) {
    if ("" == str) {
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

std::string PurePath::normal(std::string s) {
    if (s.empty())
        return "";

    std::vector<std::string> vec;
    bool firstDot = false;
    if (s[0] == preferred_separator) {
        vec.push_back(std::string() += preferred_separator);
    }
    splitWithStl(s, preferred_separator, vec);

    std::string end_preferred_separator = "";


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
    std::string retStr;
    for (auto &E: retVec) {
        retStr = retStr + E;
        if (retStr[retStr.length() - 1] != preferred_separator)
            retStr += preferred_separator;
    }
    while (!retStr.empty() && retStr[retStr.length() - 1] == preferred_separator) {
        retStr.pop_back();
    }
    return retStr;
}

std::string PurePath::commonprefix(const std::vector<PurePath> &strs) {
    if (strs.empty()) return "";
#ifdef OS_WINDOWS
    std::vector<PurePath> as = strs;
    for (auto &E :as) {
        E._path = normal(E._path);
        std::for_each(E._path.begin(), E._path.end(), [](char &c) {
            c = ::tolower(static_cast<unsigned char>(c));
        });
    }
#else
    const std::vector<path>& as = strs;
#endif
    std::string res = as[0]._path;
    for (auto &s :as) {
        for (int i = 0; i < res.size(); i++) {
            if (s._path[i] != res[i]) {
                res = res.substr(0, i);
                break;
            }
        }
    }
    return res;
}

std::string PurePath::dirname(const std::string &strs) {
    PurePath x(strs);
    auto str = normal(x._path);

    if (str.empty()) return "";
    while (str[str.length() - 1] == preferred_separator)
        str.pop_back();

    int pos = str.find_last_of(preferred_separator);
    if (pos == str.npos)
        return "";
    else
        return str.substr(0, pos + 1);

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
    if(home.empty())
        return retStr;
    std::string retstr2 = "";
    for(int i = 0; i < vec.size(); i++)
    {
        if(vec[i] == "~")
            vec[i] = home;
        else if(vec[i] == "%userprofile%")
            vec[i] = home;
        if(i != vec.size() - 1)
            retstr2 = retstr2+vec[i]+preferred_separator;
        else
            retstr2 = retstr2+vec[i];
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
    if(home.empty())
        return retStr;
    std::string retstr2 = "";
    for(int i = 0; i < vec.size(); i++)
    {
        if(vec[i] == "~")
            vec[i] = home;
        else if(vec[i] == "%userprofile%")
            vec[i] = home;
        if(i != vec.size() - 1)
            retstr2 = retstr2+vec[i]+preferred_separator;
        else
            retstr2 = retstr2+vec[i];
    }

    return retstr2;

#ifdef OS_WINDOWS

#else

#endif
}

bool PurePath::is_absolute(const std::string& str)
{
    return false;
}


bool PurePath::is_relative_to(const std::string& str){
    return false;
}

bool PurePath::is_reserved(){
    return false;
}

bool PurePath::match(const std::string& str)
{
    return false;
}

std::string PurePath::relative_to(const std::string& strParent, const std::string& strChild)
{
    return "";
}

std::string PurePath::with_name(const std::string& oldStr, const std::string& name){
    return "";
}

std::string PurePath::with_stem(const std::string& oldStr, const std::string& stem){
    return "";
}

std::string PurePath::with_suffix(const std::string& oldStr, const std::string& suffix){
    return "";
}
