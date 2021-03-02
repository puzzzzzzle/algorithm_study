//
// Created by khalidzhang on 2021/3/1.
//
// !!! 注意, 这个文件每个可执行文件只能包含一次, 本来应该是cpp/inc的,
// 但是为了多个间复用, 强行用了.h, inc文件IDE没有代码提示... !!!
#pragma once
#include <dirent.h>
#include <sys/types.h>

#include <exception>
#include <fstream>

#include "common_includes.h"
#include "path_find_common.h"
#include "time_gap.hpp"

struct PathDataInit : public testing::Test {
  protected:
  DataType data{};
  std::set<Point> setPos{};
  bool needCheckData = false;
  PathFindTask tasks{};
  std::vector<PathFindResult> results{};
  virtual void SetUp() {}
  virtual void TearDown() {}
  void init_data(int x, int y) { data.init(x, y); }
  void add_block(int x, int y) {
    Point pos(x, y);
    if (needCheckData) {
      setPos.insert(pos);
    }
    data.set(x, y, false);
    //    LOG_DEBUG("addBlock \t" << pos.to_string() << "\tsize\t" <<
    //    setPos.size())
    if (needCheckData) {
      ASSERT_TRUE(setPos.find(pos) != setPos.end());
      ASSERT_TRUE(!data.check(x, y));
    }
  };
  void check_data() {
    for (int x = 0; x < data.m_xLen; ++x) {
      for (int y = 0; y < data.m_yLen; ++y) {
        if (data.check(x, y)) {
          ASSERT_TRUE(setPos.find(Point(x, y)) == setPos.end());
        } else {
          ASSERT_TRUE(setPos.find(Point(x, y)) != setPos.end());
        }
      }
    }
  }
  void build_map_1() {
    init_data(10, 10);
    add_block(8, 2);
    add_block(4, 2);
    add_block(5, 2);
    add_block(8, 7);
    add_block(8, 8);
    add_block(0, 9);
  }
  void build_from_string(const std::string &str) {
    std::vector<std::string> lines{}, currLine{}, filterLine{};
    CommonFuncs::Split(str, "\n", lines);
    ASSERT_TRUE(lines.size() >= 3);
    CommonFuncs::Split(lines[0], ",", currLine);
    ASSERT_TRUE(currLine.size() == 2);
    int xLen = atoi(currLine[0].c_str());
    int yLen = atoi(currLine[1].c_str());
    ASSERT_TRUE(xLen > 0 && yLen > 0);
    init_data(xLen, yLen);
    for (int y = 0; y < yLen; ++y) {
      CommonFuncs::Split(lines[y + 1], ",", currLine);
      filterLine.clear();
      std::copy_if(currLine.begin(), currLine.end(),
                   std::back_inserter(filterLine),
                   [](const std::string &item) { return !item.empty(); });
      ASSERT_TRUE(filterLine.size() == xLen);
      for (int x = 0; x < xLen; ++x) {
        if (filterLine[x] != "0") {
          add_block(x, y);
        }
      }
    }
  }
  void build_from_file(const std::string &path) {
    auto allStr = CommonFuncs::LoadFileStr(path);
    ASSERT_TRUE(allStr != "");
    build_from_string(allStr);
  }
  int load_tasks(const std::string &path) { return tasks.load(path); }
  void run_tasks() {
    for (auto &item : tasks.tasks) {
      PathFindResult result{};
      result.task = item;
      TimeGap gap{};
      PathFindType search(item.first, item.second, &data);
      result.ret = search.searching();
      result.result = search.extract_path();
      result.time = gap.gap();
      if (result.ret) {
        LOG_DEBUG("one path error " << result.to_string())
      }
      results.push_back(result);
    }
  }
  std::string get_report() {
    std::ostringstream oss{};
    for (auto &item : results) {
      oss << item.to_string();
    }
    return oss.str();
  }
  void run_tasks_file(const std::string &taskPath) {
    ASSERT_TRUE(load_tasks(taskPath) == 0);
    LOG_INFO("\trun in map " << tasks.mapPath)
    build_from_file(tasks.mapPath);
    run_tasks();
    LOG_INFO("\treport:\n" << VecLikeToString(results) << "\n")
  }
};

TEST_F(PathDataInit, arrayInit) {
  needCheckData = true;
  build_map_1();
  check_data();
  LOG_DEBUG(data.to_string())
}
TEST_F(PathDataInit, loadTest) {
  needCheckData = true;
  build_from_file("data/path_find/map_file_simple.txt");
  check_data();
  LOG_DEBUG(data.to_string())
}

TEST_F(PathDataInit, pathFind_1) {
  run_tasks_file("data/path_find/path_find_1.txt");
}
TEST_F(PathDataInit, pathFind_all) {
  auto dirname = "data/path_find/";
  DIR *dir_ptr;
  struct dirent *direntp;
  dir_ptr = opendir(dirname);
  ASSERT_TRUE(dir_ptr != nullptr);
  while ((direntp = readdir(dir_ptr))) {
    std::string name = direntp->d_name;
    //    LOG_DEBUG("curr at " << name)
    if (CommonFuncs::startsWith(name, "path_find")) {
      LOG_INFO("\t\trun path find test file: " << name)
      tasks.clear();
      results.clear();
      run_tasks_file(dirname + name);
    }
  }
  closedir(dir_ptr);
}