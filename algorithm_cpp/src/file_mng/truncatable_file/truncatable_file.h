/************************************************
 * @author puzzzzzzle
 * @email 2359173906@qq.com
 * @desc
 * @time 2021/6/24
 * @file truncatable_file.h
 * @version
 * version        author            desc
 * 1.0            puzzzzzzle       create
 ************************************************/

#pragma once
#include <string>
#include <fstream>
#include <iostream>
class TruncatableFile {
  private:
  std::fstream fs_{};
  std::string name_{};
  int open_file(std::ios_base::openmode mode) {
    fs_.close();
    fs_.open(name_, mode);
    fs_ >> std::noskipws;
    return fs_.is_open() && fs_.good() ? 0 : -1;
  }

  public:
  std::fstream &fs() { return fs_; }
  int init(const std::string &file,
           std::ios_base::openmode mode = std::ios::in | std::ios::out) {
    name_ = file;
    return open_file(mode);
  }
  int truncate_file() {
    fs_.close();
    open_file(std::ios::in | std::ios::out | std::ios::trunc);
    return fs_.is_open() && fs_.good() ? 0 : -1;
  }
  int truncate_write(const char *buf, size_t n) {
    truncate_file();
    fs_.write(buf, n);
    fs_.flush();
    return fs_.good() ? 0 : -1;
  }
  // 只是使用string 作为容器
  std::string read_all() {
    fs_.seekg(0);
    return std::string(std::istream_iterator<char>(fs_),
                       std::istream_iterator<char>());
  }
};