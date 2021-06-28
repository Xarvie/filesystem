//
// Created by caspase on 6/17/2021.
//

/*
os.path.abspath(path) // 	返回绝对路径
os.path.basename(path) // 	返回文件名
os.path.commonprefix(list) // 	返回list(多个路径) //中，所有path共有的最长的路径
os.path.dirname(path) // 	返回文件路径
os.path.exists(path) // 	路径存在则返回True,路径损坏返回False
os.path.lexists 	路径存在则返回True,路径损坏也返回True
os.path.expanduser(path) // 	把path中包含的"~"和"~user"转换成用户目录
os.path.expandvars(path) // 	根据环境变量的值替换path中包含的"$name"和"${name}"
os.path.getatime(path) // 	返回最近访问时间（浮点型秒数）
os.path.getmtime(path) // 	返回最近文件修改时间
os.path.getctime(path) // 	返回文件 path 创建时间
os.path.getsize(path) // 	返回文件大小，如果文件不存在就返回错误
os.path.isabs(path) // 	判断是否为绝对路径
os.path.isfile(path) // 	判断路径是否为文件
os.path.isdir(path) // 	判断路径是否为目录
os.path.islink(path) // 	判断路径是否为链接
os.path.ismount(path) // 	判断路径是否为挂载点
os.path.join(path1[, path2[, ...]]) // 	把目录和文件名合成一个路径
os.path.normcase(path) // 	转换path的大小写和斜杠
os.path.normpath(path) // 	规范path字符串形式
os.path.realpath(path) // 	返回path的真实路径
os.path.relpath(path[, start]) // 	从start开始计算相对路径
os.path.samefile(path1, path2) // 	判断目录或文件是否相同
os.path.sameopenfile(fp1, fp2) // 	判断fp1和fp2是否指向同一文件
os.path.samestat(stat1, stat2) // 	判断stat tuple stat1和stat2是否指向同一个文件
os.path.split(path) // 	把路径分割成 dirname 和 basename，返回一个元组
os.path.splitdrive(path) // 	一般用在 windows 下，返回驱动器名和路径组成的元组
os.path.splitext(path) // 	分割路径中的文件名与拓展名
os.path.splitunc(path) // 	把路径分割为加载点与文件
os.path.walk(path, visit, arg) // 	遍历path，进入每个目录都调用visit函数，visit函数必须有3个参数(arg, dirname, names) //，dirname表示当前目录的目录名，names代表当前目录下的所有文件名，args则为walk的第三个参数
os.path.supports_unicode_filenames 	设置是否支持unicode路径名
 *
 * */

#include <iostream>
#include "PurePath.h"
#include <list>
#include <string>

#include <iostream>
#include "toml++/toml.h"


//int main()
//{
//    auto config = toml::parse_file( "../code/config.toml" );
//
//// get key-value pairs
//    std::string_view library_name = config["owner"]["name"].value_or("");
//    toml::date library_author = config["owner"]["dob"].value_or(toml::date());
//
//    std::string_view server_ip = config["database"]["server"].value_or("");
//    short ports1 = config["ports"][0].value_or(0);
//    auto ports2 = config["ports"][1].value<short>();
//    short ports3 = config["ports"][2].value_or(0);
//    int connection_max = config["connection_max"].value_or(0);
//    std::string_view ip = config["servers"]["alpha"]["ip"].value_or("");
//    std::cout << ip << std::endl;
////// modify the data
////    config.insert_or_assign("alternatives", toml::array{
////            "cpptoml",
////            "toml11",
////            "Boost.TOML"
////    });
////
////// iterate & visit over the data
//    for (auto&& [k, v] : config)
//    {
//        v.visit([](auto& node) noexcept
//                {
//                    std::cout << node << "\n";
//                    if constexpr (toml::is_string<decltype(node)>)
//                        ;
////                        do_something_with_string_values(node);
//                });
//    }
////
////// re-serialize as TOML
////    std::cout << config << "\n";
////
////// re-serialize as JSON
////    std::cout << toml::json_formatter{ config } << "\n";
//
//    if("目录连接")
//    {
//        PurePath a("c:/");
//        PurePath b("bbbb");
//        PurePath c("cccc");
//        PurePath d("dddd");
//        PurePath e("c:\\", "aaaa", "cccc", "dddd");
//
//        std::cout <<
//        os_path.joinpath(a).c_str() << '\n' <<
//        os_path.joinpath(a, b).c_str() << '\n' <<
//        os_path.joinpath(a, b, c).c_str() << '\n' <<
//        "by construct(A,B,C ....)  ->  " << e.c_str() << '\n' <<
//        os_path.joinpath(a, b, c, d).c_str() << std::endl;
//    }
//    if("取绝对路径")
//    {
//        std::cout <<
//                  os_path.abspath("./a.txt").c_str() << '\n' <<
//                  os_path.abspath("../b.txt").c_str() << '\n' <<
//                  os_path.abspath(PurePath(__file__)).c_str() << std::endl;
//    }
//    if("取文件名字带后缀")
//    {
//        std::cout <<
//                  PurePath("c:/folder1/a.txt").basename().c_str() << '\n' <<
//                  PurePath("c:/folder1/folder2").basename().c_str() << '\n' <<
//                  PurePath("c:/folder1/.").basename().c_str() << std::endl;
//    }
//    if("去除目录中的.|..")
//    {
//        std::cout << PurePath::normal("\\.\\ab\\cd\\ef\\") << std::endl; //                   \ab\cd\ef
//        std::cout << PurePath::normal("\\.\\..\\b\\..\\ab\\cd\\ef\\") << std::endl;//         \..\ab\cd\ef
//        std::cout << PurePath::normal("\\..\\..\\b\\..\\ab\\cd\\ef\\") << std::endl;//         \..\..\ab\cd\ef
//        std::cout << PurePath::normal("\\..\\..\\b\\.\\..\\ab\\cd\\ef\\") << std::endl;//      \..\..\ab\cd\ef
//        std::cout << PurePath::normal(".\\..\\..\\b\\..\\ab\\cd\\ef\\") << std::endl;//       ..\..\ab\cd\ef
//        std::cout << PurePath::normal("\\..\\..\\b\\..\\ab\\cd\\ef\\") << std::endl;//        \..\..\ab\cd\ef
//        std::cout << PurePath::normal("\\.\\.\\b\\..\\ab\\cd\\ef\\") << std::endl;//          \ab\cd\ef
//        std::cout << PurePath::normal(".\\.\\..\\..\\b\\..\\ab\\cd\\ef\\") << std::endl;//    ..\..\ab\cd\ef
//        std::cout << PurePath::normal("..\\.\\..\\..\\b\\..\\ab\\cd\\ef\\") << std::endl;//    ..\..\..\ab\cd\ef
//    }
//    if("取N个路径最长公共部分")
//    {
//        std::cout << PurePath::commonprefix({"c:/aA/bb/cc", "c:\\Aa\\BB/dd", "c:\\aa/Bb/ee"}) << std::endl; // "windows c:\aa\bb\  Linux c:/"
//        std::cout << PurePath::commonprefix({"./aa/xx/../bB", ".\\aa\\ff/.././bB//cc/dd", ".\\aa/./BB/Cc/dd"}) << std::endl; // "windows aa\bb  Linux aa"
//    }
//    if("取文件路径")
//    {
//        std::cout << PurePath::dirname("C:/aA/bb/cc") << std::endl;
//        std::cout << PurePath::dirname("C:/aA/bb/a.txt") << std::endl;
//        std::cout << PurePath::dirname("a.txt") << std::endl;
//        std::cout << PurePath::dirname("/a.txt") << std::endl;
//        std::cout << PurePath::dirname("c:\\a.txt") << std::endl;
//    }
//
//
//    if("判断路径是否存在")
//    {
//        std::cout << PurePath::exists("c:\\") << std::endl;// true
//        std::cout << PurePath::exists("c:/") << std::endl;// true
//        std::cout << PurePath::exists("c:") << std::endl;// true
//        std::cout << PurePath::exists("c") << std::endl;// false
//        std::cout << PurePath::exists("R:\\") << std::endl; // false
//        std::cout << PurePath::exists("c:\\Users") << std::endl;// true
//        std::cout << PurePath::exists("C:\\Documents and Settings") << std::endl;// true
//        std::cout << PurePath::exists(" \t \r    C:\\Windows/System32\\notepad.exe \t \r") << std::endl;// true
//    }
//    if("home(~)符号替换为完整路径")
//    {
//        std::cout << PurePath::expanduser("~/a.txt") << std::endl;
//        std::cout << PurePath::expanduser("~vv/a.txt") << std::endl;
//        std::cout << PurePath::expanduser("~~/a.txt") << std::endl;
//        std::cout << PurePath::expanduser("%USERPROFILE%/b.txt") << std::endl;
//    }
//
//    return 0;
//}

//TEST_CASE("PurePath::exists") {
//
//    CHECK(PurePath::exists("c:\\")  == true);
//    CHECK(PurePath::exists("c:/") == true);
//    CHECK(PurePath::exists("c:") == true);
//    CHECK(PurePath::exists("c") == false);
//    CHECK(PurePath::exists("R:\\") == false);
//    CHECK(PurePath::exists("c:\\Users") == true);
//    CHECK(PurePath::exists("C:\\Documents and Settings") == true);
//    CHECK(PurePath::exists(" \t \r    C:\\Windows/System32\\notepad.exe \t \r") == true);
//
//}





#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

TEST_CASE("PurePath()") {
	PurePath rr = "";
	std::cout << rr.name() << std::endl;
	std::cout << rr.suffix() << std::endl;


//	Path p = rr;
//	p.fopen("w+");
//    auto ppp = _wfopen(longPathTxt,L"rw");
//	auto ret = p.fwrite("abc", 4, 1);
//    p.fclose();

	std::error_code ec;
	file_status sls;
	uintmax_t sz;
	uintmax_t nhl;
	time_t lwt;
	status_ex(rr, ec, &sls, &sz, &nhl, &lwt ,0);

		DWORD attr = GetFileAttributesW(rr.wstr().c_str());
	CHECK(PurePath("c:/").parts[0] == "c:");
	CHECK(PurePath("./").parts.size() == 0);
	CHECK(PurePath("bbbb").parts[0] == "bbbb");
	CHECK(PurePath("c:\\", "aaaa", "cccc/", "dddd\\").parts == PurePath("c:\\aaaa\\cccc\\dddd").parts);
	CHECK(PurePath("\\\\", "aaaa", "cccc/", "dddd\\").parts == PurePath("\\\\aaaa\\cccc\\dddd").parts);
}

TEST_CASE("operator=") {
	CHECK(PurePath(R"(./a\b\c\)").asPosix() == R"(a/b/c)");
	CHECK(PurePath(R"(a\b\c\)").asPosix() == R"(a/b/c)");
	CHECK(PurePath(R"(a/b\c/)").asPosix() == R"(a/b/c)");

	CHECK(PurePath(R"(./a\b\skip\..\c\)").asPosix() == R"(a/b/c)");
	CHECK(PurePath(R"(a\skip\..\b\c\)").asPosix() == R"(a/b/c)");
	CHECK(PurePath(R"(a\skip\../b\c/)").asPosix() == R"(a/b/c)");

	CHECK((PurePath(R"(c:/a/b/c/)") == R"(c:\a\b\c)") == true);
	CHECK((PurePath(R"(\a\b\c\)") == R"(/a/b/c)") == true);
	CHECK((PurePath(R"(/a/b/c/)") == R"(/a/b/c)") == true);
	CHECK((PurePath(R"(c:\a\b\c\)") == R"(c:\a\b\c)") == true);
	CHECK((PurePath(R"(c:/a\b\c\)") == R"(c:\a\b\c)") == true);
	CHECK((PurePath(R"(c:/a\b\c/)") == R"(c:\a\b\c)") == true);

	CHECK((PurePath(R"(file:///c:/Windows)") == R"(file:///c:/Windows)") == true);
	CHECK((PurePath(R"(file:///c:/Windows/)") == R"(file:///c:/Windows)") == true);
	CHECK((PurePath(R"(file:///etc/local.d)") == R"(file:///etc/local.d)") == true);
	CHECK((PurePath(R"(file:///etc/local.d/)") == R"(file:///etc/local.d)") == true);
	CHECK((PurePath(R"(file:///etc/abc/../local.d/)") == R"(file:///etc/local.d)") == true);

	CHECK((PurePath(R"(\\a\b\c)") == R"(\\a\b\c)") == true);

	CHECK((PurePath(R"(http://www.a.com/b\c/)") == R"(http://www.a.com/b/c)") == true);
	CHECK((PurePath(R"(https://www.a.com/b\c)") == R"(https://www.a.com/b/c)") == true);

	CHECK((PurePath(R"(ftp://www.a.com/b\c/)") == R"(ftp://www.a.com/b/c)") == true);
	CHECK((PurePath(R"(sftp://www.a.com/b\c)") == R"(sftp://www.a.com/b/c)") == true);

	CHECK((PurePath(R"(ftp://localhost/b\c/)") == R"(ftp://localhost/b/c)") == true);
	CHECK((PurePath(R"(sftp://127.0.0.1/b\c)") == R"(sftp://127.0.0.1/b/c)") == true);
}

TEST_CASE("PurePath::str()") {
	CHECK(PurePath(R"(./a\b\c\)").asPosix().str() == R"(a/b/c)");
	CHECK(PurePath(R"(a\b\c\)").asPosix().str() == R"(a/b/c)");
	CHECK(PurePath(R"(a/b\c/)").asPosix().str() == R"(a/b/c)");

	CHECK(PurePath(R"(./a\b\skip\..\c\)").asPosix().str() == R"(a/b/c)");
	CHECK(PurePath(R"(a\skip\..\b\c\)").asPosix().str() == R"(a/b/c)");
	CHECK(PurePath(R"(a\skip\../b\c/)").asPosix().str() == R"(a/b/c)");

	CHECK(PurePath(R"(c:/a/b/c/)").str() == R"(c:\a\b\c)");
	CHECK(PurePath(R"(\a\b\c\)").str() == R"(/a/b/c)");
	CHECK(PurePath(R"(/a/b/c/)").str() == R"(/a/b/c)");
	CHECK(PurePath(R"(c:\a\b\c\)").str() == R"(c:\a\b\c)");
	CHECK(PurePath(R"(c:/a\b\c\)").str() == R"(c:\a\b\c)");
	CHECK(PurePath(R"(c:/a\b\c/)").str() == R"(c:\a\b\c)");

	CHECK(PurePath(R"(file:///c:/Windows)").str() == R"(file:///c:/Windows)");
	CHECK(PurePath(R"(file:///c:/Windows/)").str() == R"(file:///c:/Windows)");
	CHECK(PurePath(R"(file:///etc/local.d)").str() == R"(file:///etc/local.d)");
	CHECK(PurePath(R"(file:///etc/local.d/)").str() == R"(file:///etc/local.d)");
	CHECK(PurePath(R"(file:///etc/abc/../local.d/)").str() == R"(file:///etc/local.d)");

	CHECK(PurePath(R"(\\a\b\c)").str() == R"(\\a\b\c)");

	CHECK(PurePath(R"(http://www.a.com/b\c/)").str() == R"(http://www.a.com/b/c)");
	CHECK(PurePath(R"(https://www.a.com/b\c)").str() == R"(https://www.a.com/b/c)");

	CHECK(PurePath(R"(ftp://www.a.com/b\c/)").str() == R"(ftp://www.a.com/b/c)");
	CHECK(PurePath(R"(sftp://www.a.com/b\c)").str() == R"(sftp://www.a.com/b/c)");

	CHECK(PurePath(R"(ftp://localhost/b\c/)").str() == R"(ftp://localhost/b/c)");
	CHECK(PurePath(R"(sftp://127.0.0.1/b\c)").str() == R"(sftp://127.0.0.1/b/c)");
}

TEST_CASE("PurePath::is_absolute()") {
	CHECK(PurePath("file:///c:/Windows").is_absolute() == true);
	CHECK(PurePath("c:/a/b").is_absolute() == true);
	CHECK(PurePath("c:/a/b/").is_absolute() == true);
	CHECK(PurePath("/a/b").is_absolute() == true);
	CHECK(PurePath("./a/b").is_absolute() == false);
	CHECK(PurePath("./").is_absolute() == false);
	CHECK(PurePath(".").is_absolute() == false);
	CHECK(PurePath("./").parts.size() == 0);
	CHECK(PurePath("bbbb").parts[0] == "bbbb");
	CHECK(PurePath("c:\\", "aaaa", "cccc/", "dddd\\").parts == PurePath("c:\\aaaa\\cccc\\dddd").parts);
	CHECK(PurePath("\\\\", "aaaa", "cccc/", "dddd\\").parts == PurePath("\\\\aaaa\\cccc\\dddd").parts);
}