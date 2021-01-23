# python3
# -*- coding: utf-8 -*-
# @author khalidzhang
# @email khalidzhang@tencent.com
# @desc
# @time 2021/1/22
# @file astar.py
# @version
# version        author            desc
# 1.0            khalidzhang       create
import math
import heapq

from consts import get_logger

logger = get_logger(__name__)


class AStar:
    def __init__(self, s_start, s_end, s_data):
        self.s_start = s_start
        self.s_end = s_end
        self.s_data = s_data
        self.open = []
        self.close = []
        self.parent = {}
        self.g = {}

    def h(self, pos):
        return math.hypot(self.s_end[0] - pos[0], self.s_end[1] - pos[1])

    def f(self, pos):
        return self.g[pos] + self.h(pos)

    def valid_pos(self, pos):
        shape = self.s_data.shape
        if not (0 <= pos[0] < shape[0] and 0 <= pos[1] < shape[1]):
            return False
        if self.s_data[pos[0], pos[1]] != 0:
            return False
        return True

    def get_neighbor(self, pos):
        temp = [
            (pos[0], pos[1] + 1),
            (pos[0] + 1, pos[1]),
            (pos[0], pos[1] - 1),
            (pos[0] - 1, pos[1]),
        ]
        return list(filter(self.valid_pos, temp))

    def extract_path(self, parent):
        """
        Extract the path based on the parent set.
        :return: The planning path
        """

        path = [self.s_end]
        end = self.s_end
        if end not in parent:
            logger.info(f"path find fail")
            return []

        while True:
            end = parent[end]
            path.append(end)

            if end == self.s_start:
                break

        return list(path)

    def cost(self, s_start, s_goal):
        return math.hypot(s_goal[0] - s_start[0], s_goal[1] - s_start[1])

    def searching(self):
        # prepare
        l_open = self.open  # open
        l_close = self.close  # close
        l_parent = self.parent  # parent dict
        l_g = self.g  # cost to com

        start = self.s_start
        end = self.s_end
        if start == end:
            return [start, end]
        # data = self.s_data

        # init
        l_parent[start] = start
        l_g[start] = 0
        l_g[end] = math.inf
        heapq.heappush(l_open, (self.f(start), start))

        # start

        while l_open:
            _, s = heapq.heappop(l_open)
            l_close.append(s)
            if s == end:
                # get result
                break
            for s_n in self.get_neighbor(s):
                new_cost = l_g[s] + self.cost(s, s_n)
                if s_n not in l_g:
                    l_g[s_n] = math.inf
                if new_cost < l_g[s_n]:
                    l_g[s_n] = new_cost
                    l_parent[s_n] = s
                    heapq.heappush(l_open, (self.f(s_n), s_n))
        return self.extract_path(l_parent)


def main():
    s_start = (5, 5)
    s_goal = (45, 25)
    from src.path_plan import plotting
    import numpy as np
    import time
    plot = plotting.Plotting(s_start, s_goal)
    mat = np.zeros((plot.env.x_range, plot.env.y_range), dtype=int)
    for it in plot.obs:
        mat[it[0], it[1]] = 1
    astar = AStar(s_start, s_goal, mat)
    start_t = time.time()
    path = astar.searching()
    if len(path) <= 0:
        logger.info(f"cannot find path ")
        return
    logger.info(f"time use {time.time() - start_t}")
    visited = astar.close
    plot.animation(path, visited, "A*")  # animation


if __name__ == '__main__':
    main()
