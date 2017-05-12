#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

"""
Watch a process and plot its resource usage
"""

import sys
import os
import time
import datetime
import xboa.common

def print_mem_usage(pid):
    """
    Get system resources used by process pid

      @param pid process id

    @returns dict of {"pid":pid, "memory":mem_usage} or None on failure
    """
    columns = "%cpu,%mem,etime,sz,rsz,vsz"
    ps_out = os.popen('ps -p %d -o %s' % (pid, columns)).readlines()
    if len(ps_out) == 2:
        out_list = ps_out[1].rstrip().split()
        col_list = columns.split(",")
        out = dict(zip(col_list, out_list))
        out["pid"] = pid
        return out
    else:
        raise ValueError("Failed to get resource usage")

def make_graphs(pid_list, wait_time, max_time):
    """Make ROOT image stuff"""
    graph_dict = {}
    canvas = xboa.common.make_root_canvas("memory usage")
    for pid in pid_list:
        hist, graph = xboa.common.make_root_graph(
                           "pid "+str(pid),
                           [0.], "time [s]", 
                           [0.], "memory usage (rsz)", 
                           xmin=wait_time/2, xmax=max_time, 
                           ymin=0, ymax=600000)
        graph_dict[pid] = graph
    hist.Draw()
    xboa.common.make_root_legend(canvas, graph_dict.values())
    return canvas, graph_dict

def monitor_step(delta_t, mem_list, graph_dict):
    """
    Make a single step in the monitoring; append output to graph
    """
    mem_list_step = []
    for i, item in enumerate(mem_list[-1]):
        pid = int(item["pid"])
        mem = print_mem_usage(pid)
        # mem_p = float(mem["%mem"])
        rsz = float(mem["rsz"])
        index = graph_dict[pid].GetN()
        # if mem and (mem_p > 1e-9 or index == 1):
        if mem and (rsz > 1e-9 or index == 1):
            mem_list_step.append(mem)
            graph_dict[pid].Expand(index+1)
            graph_dict[pid].SetPoint(index,
                                    delta_t.seconds,
                                    rsz)
        graph_dict[pid].SetLineStyle(i+1)
        graph_dict[pid].Draw()
    return mem_list_step

def main(pid_list, title, wait_time, max_time):
    """
    Iterate over list of pids; while pids exist, print cpu usage etc

      @param pid_list list of pids to iterate over
      @param delta_time in seconds for monitoring steps
      @param max_time maximum time in seconds over which to monitor
    """
    start = datetime.datetime.now()
    mem_list = [[{"pid":pid} for pid in pid_list]]
    canvas, graph_dict = make_graphs(pid_list, wait_time, max_time)
    delta = datetime.timedelta.max
    if max_time:
        delta = datetime.timedelta(0, max_time)
    delta_t = datetime.datetime.now()-start
    while len(mem_list[-1]) > 0 and delta_t < delta:
        time.sleep(wait_time)
        mem_list_step = monitor_step(delta_t, mem_list, graph_dict)
        mem_list.append(mem_list_step)
        print mem_list[-1]
        canvas.Update()
        canvas.Print(str(title)+"_resource_usage.root")
        canvas.Print(str(title)+"_resource_usage.png")
        canvas.Print(str(title)+"_resource_usage.eps")
        delta_t = datetime.datetime.now()-start
    return mem_list

if __name__ == "__main__":
    main(sys.argv[1:], "maus", 5, 600)

