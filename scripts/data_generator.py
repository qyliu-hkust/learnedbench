# !/usr/bin/python
# coding=utf-8

import os
import argparse
import numpy as np
import pandas as pd
from pandas.core.reshape.merge import merge
np.random.seed(999)


DATA_PATH = "./data/"


# generate n d-dimensional uniform points in range [0, r]
def uniform(n, d, r):
    data = np.random.uniform(0.0, r, [n, d])
    np.savetxt("{prefix}/synthetic/uniform_{n}_{d}_{r}.csv".format(prefix=DATA_PATH, n=n, d=d, r=r), data, delimiter=',')


# generate n d-dimensional points from a Gaussian distribution N(0, s^2)
def gaussian(n, d, s):
    data = np.random.normal(0.0, s, [n, d])
    np.savetxt("{prefix}/synthetic/gaussian_{n}_{d}_{s}.csv".format(prefix=DATA_PATH, n=n, d=d, s=s), data, delimiter=',')
    

# generate n d-dimensional points from a lognormal distribution mean=0 sigma=s
def lognormal(n, d, s):
    data = np.random.lognormal(0.0, s, [n, d])
    np.savetxt("{prefix}/synthetic/lognormal{n}_{d}_{s}.csv".format(prefix=DATA_PATH, n=n, d=d, s=s), data, delimiter=',')


# generate TPC-H data with a specified scale factor 
# 4 columns of lineitem table is used (l_quantity, l_extendedprice, l_discount, l_tax)
# scale factor effect: #rows = 6 * s Million
def tpc(s):
    if not os.path.exists("../tools/tpch-dbgen/dbgen"):
        print("Compile the tpch-dbgen first.")
        return
    os.system("../tools/tpch-dbgen/dbgen -T L -b ../tools/tpch-dbgen/dists.dss -s {s}".format(s=s))
    df = pd.read_csv("lineitem.tbl", sep='|', header=None, usecols=[4, 5, 6, 7])
    df.to_csv(DATA_PATH + "real/tpc_{s}.csv".format(s=s), header=False, index=False)
    os.system("rm lineitem.tbl")


# download and pre-process OpenStreatMap data
def osm():
    None


# download and pre-process NYC taxi data from 2016-01 to 2016-06 (2D locations)
def nytaxi():
    if not os.path.exists("./.download/"):
        os.mkdir("./.download/")

    url_prefix = "https://s3.amazonaws.com/nyc-tlc/trip+data/"
    files = []

    for i in range(1, 7):
        file_name = "yellow_tripdata_2016-" + str(i).zfill(2) + ".csv"
        files.append("./.download/" + file_name)
        if not os.path.exists("./.download/" + file_name):
            url = url_prefix + file_name
            os.system("wget -O ./.download/" + file_name + " " + url)

    dfs = []
    for file_name in files:
        df = pd.read_csv(file_name, usecols=["pickup_longitude", "pickup_latitude"])
        df = df.dropna()
        dfs.append(df)
    merged = pd.concat(dfs)
    merged.to_csv(DATA_PATH + "real/nytaxi.csv", header=False, index=False)


def checkdir():
    if not os.path.exists(DATA_PATH):
        os.mkdir(DATA_PATH)
    if not os.path.exists(DATA_PATH + "synthetic"):
        os.mkdir(DATA_PATH + "synthetic")
    if not os.path.exists(DATA_PATH + "real"):
        os.mkdir(DATA_PATH + "real")


if __name__ == "__main__":
    checkdir()
    
    parser = argparse.ArgumentParser(description="Generate datasets.")
    parser.add_argument("--dist", type=str, nargs="?", help="distribution")
    parser.add_argument("-n", type=int, nargs="?", help="number of points")
    parser.add_argument("-d", type=int, nargs="?", help="data dimension")
    parser.add_argument("-s", type=int, nargs="?", help="scale factor")
    parser.add_argument("--real", type=str, nargs="?", help="generation real data")

    args = parser.parse_args()
    if args.real is not None:
        if args.real == "tpc":
            tpc(args.s)
        elif args.real == "osm":
            osm()
        elif args.real == "nytaxi":
            nytaxi()
        else:
            print("Please indicate one of ['tpc', 'osm', 'nytaxi'].")
    elif args.dist is not None:
        if args.dist == "uniform":
            uniform(args.n, args.d, args.s)
        elif args.dist == "gaussian":
            gaussian(args.n, args.d, args.s)
        elif args.dist == "lognormal":
            lognormal(args.n, args.d, args.s)
        else:
            print("Please indicate one of ['uniform', 'gaussian', 'lognormal'].")
    else:
        print("Please indicate correct augments.")

