import matplotlib.pyplot as plt
import csv
import collections
from itertools import zip_longest, islice

Data = collections.namedtuple("data", ["size","avg", "std_dev"])
def parse_row(row_mean, row_median, row_std_dev):    
    tmp = row_mean[0].split("/")
    if (len(tmp) < 2):
        return (None, None)
    name = tmp[0]
    size = int(tmp[1].split("_")[0])

    #get cpu time -> index 3
    time_idx = 5

    avg = float(row_mean[time_idx])
    std_dev = float(row_std_dev[time_idx])

    return (name, Data(size,avg,std_dev))
    

def grouper(iterable, n, fillvalue=None):
    "Collect data into fixed-length chunks or blocks"
    # grouper('ABCDEFG', 3, 'x') --> ABC DEF Gxx"
    args = [iter(iterable)] * n
    return zip_longest(*args, fillvalue=fillvalue)

def consume(iterator, n):
    "Advance the iterator n-steps ahead. If n is none, consume entirely."
    # Use functions that consume iterators at C speed.
    if n is None:
        # feed the entire iterator into a zero-length deque
        collections.deque(iterator, maxlen=0)
    else:
        # advance to the empty slice starting at position n
        next(islice(iterator, n, n), None)


def plot_file(filename, plot_name):
    with open(filename) as f:
        data = csv.reader(f)
        data_dict = {}
        for row_mean, row_media, row_std_dev in grouper(data, 3):
            n, d = parse_row(row_mean, row_media, row_std_dev)
            if (n != None):
                #print((n,d))
                if (n in data_dict):
                    k = data_dict[n]
                    k[0].append(d.size)
                    k[1].append(d.avg)
                    k[2].append(d.std_dev)
                else:
                    data_dict[n] = [[d.size],[d.avg],[d.std_dev]]
    
    plt.xscale("log")
    plt.xlabel("container size");
    plt.ylabel("increments per second")
    plt.title(plot_name)
    for (k, v) in data_dict.items():
        x = plt.errorbar(v[0], v[1], yerr=v[2], linestyle="-.", fmt='o', capsize=10)
    #plt.legend([k for (k,v) in data_dict.items()])
    plt.legend(["Reference","any_iterator","any_iterator_virtual"])
         
    return plt

p1 = plot_file("benchmark_results/result_list.csv", "iteration over a list")
p1.savefig("benchmark_results/list_iteration.svg")
p1.clf()
p2 = plot_file("benchmark_results/result_map.csv", "iteration over a map")
p2.savefig("benchmark_results/map_iteration.svg")