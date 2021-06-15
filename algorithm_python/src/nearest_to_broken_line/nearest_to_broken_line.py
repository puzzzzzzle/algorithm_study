import logging

import matplotlib.pyplot as plt
import math

from consts import get_logger

logger = get_logger(__name__)


def get_foot_point(x0, y0, x1, y1, x2, y2):
    """
    求垂足
    Ax+By+C=0
    """
    A = y2 - y1
    B = x1 - x2
    if A == 0 and B == 0:
        return x1, y1
    C = x2 * y1 - x1 * y2
    D = A * y0 - B * x0
    xd = -(B * D + A * C) / (B ** 2 + A ** 2)
    if B == 0:
        yd = y0
    else:
        yd = -(A * xd + C) / B
    return xd, yd


def get_line_len(x1, y1, x2, y2):
    """
    两点间距离
    """
    return math.sqrt((x1 - x2) ** 2 + (y1 - y2) ** 2)


def on_segment(x0, y0, x1, y1, x2, y2):
    """
    点是否在线段上
    如果想判断一个点是否在线段上，那么要满足以下两个条件：
    （Q - P1） * （P2 - P1）= 0；
    Q在以P1，P2为对角顶点的矩形内；
    """
    return (x0 - x1) * (y2 - y1) == (x2 - x1) * (y0 - y1) \
           and min(x1, x2) <= x0 <= max(x1, x2) \
           and min(y1, y2) <= y0 <= max(y1, y2)


def get_nearest_to_segment(x0, y0, x1, y1, x2, y2):
    """
    求 点 与线段的最近点
    """
    # 求垂足
    xf, yf = get_foot_point(x0, y0, x1, y1, x2, y2)
    # 判断 端点 与 垂足的距离哪个近
    curr_len = float("inf")
    pos = (0, 0)

    def check(inx, iny):
        nonlocal pos, curr_len
        temp_len = get_line_len(x0, y0, inx, iny)
        if temp_len < curr_len:
            pos = (inx, iny)
            curr_len = temp_len

    check(x1, y1)
    check(x2, y2)
    if on_segment(xf, yf, x1, y1, x2, y2):
        check(xf, yf)
    return curr_len, pos


def get_nearest_to_broken_line(x0, y0, broken_line: list):
    """
    求 点 与折线的最近点
    """
    assert len(broken_line) >= 2
    curr_len = float("inf")
    pos = (0, 0)
    curr_index = 0
    for index in range(len(broken_line) - 1):
        curr_pos = broken_line[index]
        next_pos = broken_line[index + 1]
        temp_len, temp_pos = get_nearest_to_segment(x0, y0, curr_pos[0], curr_pos[1], next_pos[0], next_pos[1])
        if temp_len < curr_len:
            curr_len = temp_len
            pos = temp_pos
            curr_index = index
    return curr_len, pos, curr_index


def fix_broken_line(x0, y0, x1, y1, index, broken_line: list):
    """
    x0, y0 : 外点
    x1,y1 : 垂足
    """
    assert index < len(broken_line) - 1
    result = []
    result.append([x0, y0])
    result.append([x1, y1])
    # 忽略 所在线段的起点
    for index in range(index + 1, len(broken_line)):
        result.append(broken_line[index])
    return result


#########  测试代码相关
def draw_fig(x0, y0, x1, y1, x2, y2):
    fig, ax = plt.subplots()  # type:plt.Figure, plt.Axes
    ax.axis("equal")
    ax.set_title(f"{x0},{y0}   {x1},{y1}    {x2},{y2}")
    foot_point = get_foot_point(x0, y0, x1, y1, x2, y2)
    _, nearest = get_nearest_to_segment(x0, y0, x1, y1, x2, y2)
    xl = [x0, x1, x2, foot_point[0], nearest[0]]
    yl = [y0, y1, y2, foot_point[1], nearest[1]]
    ax.plot(xl, yl, "o")

    xl = [x1, x2]
    yl = [y1, y2]
    ax.plot(xl, yl, "-")

    xl = [x0, foot_point[0]]
    yl = [y0, foot_point[1]]
    ax.plot(xl, yl, "-.")

    xl = [x0, nearest[0]]
    yl = [y0, nearest[1]]
    ax.plot(xl, yl, ":")
    logger.debug(f"{x0},{y0}   {x1},{y1}    {x2},{y2} {foot_point} {nearest}")


def broken_line_test(x0, y0, broken_line: list):
    fig, ax = plt.subplots()  # type:plt.Figure, plt.Axes
    ax.axis("equal")
    ax.set_title(f"{x0},{y0} {str(broken_line)}")

    ps_x = [pos[0] for pos in broken_line]
    ps_y = [pos[1] for pos in broken_line]
    ax.plot(ps_x, ps_y, "-")

    ps_x.append(x0)
    ps_y.append(y0)
    ax.plot(ps_x, ps_y, "o")

    _, nearest, index = get_nearest_to_broken_line(x0, y0, broken_line)
    xl = [x0, nearest[0]]
    yl = [y0, nearest[1]]
    ax.plot(xl, yl, ":")
    ax.plot(xl, yl, "o")
    logger.debug(f"{x0},{y0}    {str(broken_line)}    {index} {nearest}")

    fixed_line = fix_broken_line(x0,y0,nearest[0],nearest[1],index,broken_line)
    ps_x = [pos[0] for pos in fixed_line]
    ps_y = [pos[1] for pos in fixed_line]
    ax.plot(ps_x, ps_y, "o")
    ax.plot(ps_x, ps_y, "-.")

if __name__ == '__main__':
    # fig = plt.figure(figsize=(100, 100),dpi=10)  # type:plt.Figure
    # ax = fig.add_subplot(3, 1, 1)  # type:plt.Axes
    if False:
        draw_fig(2, 0, 0, 0, 5, 5)
        draw_fig(2, 0, 0, 0, 5, 0)
        draw_fig(2, 0, 0, 0, 0, 5)
        draw_fig(2, 0, 0, 0, 0, 0)
        draw_fig(2, 3, 0, 0, 5, 3)
        draw_fig(-5, 1, 0, 0, 5, 3)
        draw_fig(-5, 1, 0, 0, 5, 0)
        draw_fig(-5, 1, 0, 4, 0, 5)
    if True:
        broken_line_test(2, 3, [[0, 0], [0, 5], [5, 5]])
        broken_line_test(2, 3, [[10, 10], [0, 5], [5, 5], [10, 10]])
        broken_line_test(10, 3, [[10, 10], [0, 5], [5, 5], [10, 10]])
        broken_line_test(9, 4, [[10, 10], [0, 5], [5, 5], [10, 10], [10, 0]])
        broken_line_test(9, 3, [[10, 10], [0, 5], [5, 5], [10, 10], [10, 4]])

    plt.show()
