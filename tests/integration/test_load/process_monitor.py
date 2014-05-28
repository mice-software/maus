import sys
import os
import time
import datetime
import xboa.Common

def print_mem_usage(pid):
    """
    Get system resources used by process pid

      @param pid process id

    @returns dict of {"pid":pid, "memory":mem_usage} or None on failure
    """
    columns = "%cpu,%mem,etime"
    ps_out = os.popen('ps -p %d -o %s' % (pid, columns)).readlines()
    if len(ps_out) == 2:
        out_list = ps_out[1].rstrip().split()
        col_list = columns.split(",")
        out = dict(zip(col_list, out_list))
        out["pid"] = pid
        return out
    else:
        raise ValueError("Failed to get resource usage")

def main(pid_list, title, wait_time = 5, max_time = None):
    """
    Iterate over list of pids; while pids exist, print cpu usage etc

      @param pid_list list of pids to iterate over
      @param delta_time in seconds for monitoring steps
      @param max_time maximum time in seconds over which to monitor
    """
    if max_time == None:
        n_points = 100
    else:
        n_points = max_time/wait_time
    start = datetime.datetime.now()
    mem_list = [[{"pid":pid} for pid in pid_list]]
    graph_dict = {}
    canvas = xboa.Common.make_root_canvas("memory usage")
    for pid in pid_list:
        hist, graph = xboa.Common.make_root_graph(
                           "pid "+str(pid),
                           [0.], "time [s]", 
                           [0.], "memory usage %", 
                           xmin=wait_time/2, xmax=max_time, 
                           ymin=-5, ymax=105)
        graph_dict[pid] = graph
    hist.Draw()
    xboa.Common.make_root_legend(canvas, graph_dict.values())
    index = 0
    td = datetime.timedelta.max
    if max_time:
        td = datetime.timedelta(0, max_time)
    while len(mem_list[-1]) > 0 and (max_time and \
                                 datetime.datetime.now()-start < td):
        index += 1
        time.sleep(wait_time)
        mem_list_step = []
        for i, item in enumerate(mem_list[-1]):
            pid = int(item["pid"])
            mem = print_mem_usage(pid)
            if mem:
                mem_list_step.append(mem)
                graph_dict[pid].Expand(index)
                graph_dict[pid].SetPoint(index,
                    (datetime.datetime.now()-start).seconds, float(mem["%mem"]))
            graph_dict[pid].SetLineStyle(i+1)
            graph_dict[pid].Draw()
            print "draw pid", pid
        mem_list.append(mem_list_step)
        print mem_list[-1]
        canvas.Update()
        canvas.Print(str(title)+"_resource_usage.root")
        canvas.Print(str(title)+"_resource_usage.png")
        canvas.Print(str(title)+"_resource_usage.eps")
    return mem_list

if __name__ == "__main__":
    main(sys.argv[1:])

