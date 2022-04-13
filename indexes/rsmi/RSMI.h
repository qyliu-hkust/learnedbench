#include <iostream>
#include <vector>
#include "./Node.h"
#include "./Point.h"
#include "./Mbr.h"
#include "./NonLeafNode.h"
#include "./LeafNode.h"
#include <typeinfo>
#include "./hilbert.H"
#include "./hilbert4.H"
#include "./z.H"
#include "./ModelTools.h"
#include "./ExpRecorder.h"
#include <map>
#include <chrono>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <torch/script.h>
#include <ATen/ATen.h>
#include <torch/torch.h>

#include <torch/optim.h>
#include <torch/types.h>
#include <torch/utils.h>

using namespace at;
using namespace torch::nn;
using namespace torch::optim;
using namespace std;
using namespace rsmiutils;
using namespace rsmientities;

class RSMI
{

private:
    int level;
    int index;
    int max_partition_num;
    long long N = 0;
    int max_error = 0;
    int min_error = 0;
    int width = 0;
    int leaf_node_num;
    
    bool is_last;
    Mbr mbr;
    std::shared_ptr<Net> net;

public:
    string model_path;
    static string model_path_root;
    map<int, RSMI> children;
    vector<LeafNode> leafnodes;

    RSMI();
    RSMI(int index, int max_partition_num);
    RSMI(int index, int level, int max_partition_num);

    void build(ExpRecorder &exp_recorder, vector<Point> points);
    
    void print_index_info(ExpRecorder &exp_recorder);

    bool point_query(ExpRecorder &exp_recorder, Point query_point);
    void point_query(ExpRecorder &exp_recorder, vector<Point> query_points);

    void window_query(ExpRecorder &exp_recorder, vector<Mbr> query_windows);
    // vector<Point> window_query(ExpRecorder &exp_recorder, Mbr query_window);
    void window_query(ExpRecorder &exp_recorder, vector<Point> vertexes, Mbr query_window, float boundary, int k, Point query_point, float &);
    void window_query(ExpRecorder &exp_recorder, vector<Point> vertexes, Mbr query_window);
    void acc_window_query(ExpRecorder &exp_recorder, vector<Mbr> query_windows);
    vector<Point> acc_window_query(ExpRecorder &exp_recorder, Mbr query_windows);

    void kNN_query(ExpRecorder &exp_recorder, vector<Point> query_points, int k);
    vector<Point> kNN_query(ExpRecorder &exp_recorder, Point query_point, int k);
    void acc_kNN_query(ExpRecorder &exp_recorder, vector<Point> query_points, int k);
    vector<Point> acc_kNN_query(ExpRecorder &exp_recorder, Point query_point, int k);
    double cal_rho(Point point);
    double knn_diff(vector<Point> acc, vector<Point> pred);

    void insert(ExpRecorder &exp_recorder, Point);
    void insert(ExpRecorder &exp_recorder, vector<Point>);

    void remove(ExpRecorder &exp_recorder, Point);
    void remove(ExpRecorder &exp_recorder, vector<Point>);
};

