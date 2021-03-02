/************************************************
 * @author khalidzhang
 * @email khalidzhang@tencent.com
 * @desc 简单的A*实现
 * @time 2021/2/27
 * @file a_star.h
 * @version
 ************************************************/
#pragma once
#include <algorithm>
#include <cmath>
#include <map>
#include <queue>
#include <vector>
namespace PathSearch {
template <class ItemType>
bool in_container(const ItemType& item,
                  const std::vector<ItemType>& container) {
  return std::find(container.cbegin(), container.cend(), item) !=
         container.cend();
}
template <class ItemType, class FindableContainerType>
bool in_container(const ItemType& item,
                  const FindableContainerType& container) {
  return container.find(item) != container.cend();
}
template <class PathDataWrapper, class PointT>
class AStar {
  private:
  using PathData = PathDataWrapper;
  using Point = PointT;
  using CPoint = const Point;
  struct OpenItem {
    double m_f{};
    Point m_pos{};
    OpenItem() = default;
    OpenItem(double f, CPoint& pos) : m_f(f), m_pos(pos) {}
    friend bool operator<(const OpenItem& lhs, const OpenItem& rhs) {
      return lhs.m_f < rhs.m_f;
    }
    friend bool operator>(const OpenItem& lhs, const OpenItem& rhs) {
      return lhs.m_f > rhs.m_f;
    }
  };

  Point m_start{}, m_end{};

  // 小顶堆
  std::priority_queue<OpenItem, std::vector<OpenItem>, std::greater<OpenItem>>
      m_open{};
  std::vector<Point> m_close{};
  std::map<Point, Point> m_parent{};
  std::map<Point, double> m_g{};
  const PathData* const m_data{};
  int m_searchRet{};
  std::vector<Point> m_result{};

  public:
  AStar(CPoint& start, CPoint& end, const PathData* const data)
      : m_start(start), m_end(end), m_data(data) {}

  private:
  inline double h(CPoint& pos) {
    return std::hypot(m_end.x - pos.x, m_end.y - pos.y);
  }
  inline double f(CPoint& pos) { return m_g[pos] + h(pos); }
  inline double cost(CPoint& start, CPoint& end) {
    return std::hypot(end.x - start.x, end.y - start.y);
  }
  inline bool valid_pos(CPoint& pos) { return m_data->check(pos.x, pos.y); }
  inline bool valid_pos(int x, int y) { return m_data->check(x, y); }

  std::vector<Point> get_neighbor(CPoint& pos) {
    std::vector<Point> result{};
    auto add = [&](int x, int y) {
      if (valid_pos(x, y)) {
        result.emplace_back(x, y);
      }
    };
    // 四角连通性
    add(pos.x, pos.y + 1);
    add(pos.x + 1, pos.y);
    add(pos.x, pos.y - 1);
    add(pos.x - 1, pos.y);
    return result;
  }

  public:
  int searching() {
    if (m_start == m_end) {
      m_searchRet = 100;
      return 0;
    }
    if (!valid_pos(m_start.x, m_start.y) || !valid_pos(m_end.x, m_end.y)) {
      m_searchRet = 101;
      return -100;
    }
    // init
    m_parent[m_start] = m_start;
    m_g[m_start] = 0;
    m_g[m_end] = INFINITY;
    m_open.push(OpenItem(f(m_start), m_start));
    // start
    while (!m_open.empty()) {
      // 当前点
      auto curr_pos = m_open.top().m_pos;
      m_open.pop();
      m_close.push_back(curr_pos);

      if (curr_pos == m_end) {
        break;
      }
      // 处理每个邻居
      for (auto& curr_neighbor : get_neighbor(curr_pos)) {
        auto new_cost = m_g[curr_pos] + cost(curr_pos, curr_neighbor);
        if (!in_container(curr_neighbor, m_g)) {
          m_g[curr_neighbor] = INFINITY;
        }
        if (new_cost < m_g[curr_neighbor]) {
          m_g[curr_neighbor] = new_cost;
          m_parent[curr_neighbor] = curr_pos;
          m_open.push(OpenItem(f(curr_neighbor), curr_neighbor));
        }
      }
    }
    m_searchRet = 0;
    return 0;
  }
  std::vector<Point> extract_path() {
    if (m_searchRet == 100) {
      return {m_start, m_end};
    }
    if (m_searchRet) {
      return {};
    }
    auto end = m_end;
    m_result.push_back(end);
    if (!in_container(m_end, m_parent)) {
      return {};
    }
    while (true) {
      end = m_parent[end];
      m_result.push_back(end);
      if (end == m_start) {
        break;
      }
    }
    std::reverse(std::begin(m_result), std::end(m_result));
    return m_result;
  }
};
}  // namespace PathSearch