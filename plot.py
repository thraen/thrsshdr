#!/bin/python3 -u


import matplotlib.pyplot as plt


## read line by line from stdin

def read_lines():
    while True:
        line = input()
        splits = line.split()

        ## read_batch
        ## whole_ring
        ## windowed__

#         if splits[0] == "read_batch":
#         if splits[0] == "whole_ring":
#         if splits[0] == "windowed__":
        labl = splits[0]
        if labl == "whole_ring" or labl == "windowed__":
            plot_line(line)

def plot_line(line):
    splits = line.split()
    labl = splits[0] + splits[1]
    xs = [float(x) for x in splits[2:]]
    plot_array(xs, labl)
    plt.show()

def plot_array(xs, labl):
    plt.plot(xs)
    plt.xlabel('Index')
    plt.ylabel('Value')
    plt.title(labl)
    plt.grid(True)

read_lines()
