#pragma once



#include <geos/geom/Coordinate.h>
#include <geos/geom/Envelope.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/index/quadtree/Quadtree.h>
#include <geos/index/strtree/TemplateSTRtree.h>

#ifdef HEAP_PROFILE
#include <gperftools/heap-profiler.h>
#endif

#include <chrono>

#include "../base_index.hpp"
#include "../../utils/type.hpp"


namespace bench { namespace index {
    
struct GEOSGeometry {
    std::vector<geos::geom::Geometry *> geometry;
    geos::geom::GeometryFactory::Ptr factory;

    GEOSGeometry() {
        factory = geos::geom::GeometryFactory::create();
    }

    ~GEOSGeometry() {
        for (geos::geom::Geometry * g : geometry) {
            factory->destroyGeometry(g);
        }
        geometry.clear();
    }
};


template<size_t Dim>
void points_to_geos(vec_of_point_t<Dim>& points, GEOSGeometry& geos_points) {
    geos_points.geometry.reserve(points.size());

    for (auto& p : points) {
        geos::geom::Geometry * temp = geos_points.factory->createPoint(geos::geom::Coordinate(std::get<0>(p), std::get<1>(p)));
        geos_points.geometry.push_back(temp);
    }
}


template<size_t Dim=2>
class QDTree : public BaseIndex {

using QDTree_t = geos::index::quadtree::Quadtree;
using Point = point_t<Dim>;
using Points = std::vector<Point>;
using Box = box_t<Dim>;

public:
QDTree(Points& points) { 
    std::cout << "Construct QDTree " << "Dim=" << Dim << std::endl;

    points_to_geos(points, geos_points);

    auto start = std::chrono::steady_clock::now();

#ifdef HEAP_PROFILE
HeapProfilerStart("qdtree");
#endif

    _qdtree = new QDTree_t();

    for (size_t i=0; i<points.size(); ++i) {
        _qdtree->insert(geos_points.geometry[i]->getEnvelopeInternal(), reinterpret_cast<void *>(geos_points.geometry[i]));
    }

#ifdef HEAP_PROFILE
HeapProfilerDump("final");
HeapProfilerStop();
#endif

    auto end = std::chrono::steady_clock::now();
    build_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Build Time: " << get_build_time() << " [ms]" << std::endl;
}


~QDTree() {
    delete _qdtree;
}


Points range_query(Box& box) {
    // prepare the query geometry
    geos::geom::GeometryFactory::Ptr gf = geos::geom::GeometryFactory::create();

    geos::geom::Envelope geos_box(box.min_corner()[0], box.max_corner()[0], box.min_corner()[1], box.max_corner()[1]);
    auto query_geometry = gf->toGeometry(&geos_box);

    // query the geometry envolope
    auto start = std::chrono::steady_clock::now();

    std::vector<void*> results;
    _qdtree->query(query_geometry->getEnvelopeInternal(), results);

    auto end = std::chrono::steady_clock::now();
    range_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    range_count ++;

    // collect results
    // the geos libaray returns geometries **may** intersect the query rectangle
    Points point_results;
    for (size_t i=0; i<results.size(); ++i) {
        auto pg = static_cast<geos::geom::Geometry *>(results[i]);
        if (query_geometry->contains(pg)) {
            Point temp;
            temp[0] = pg->getCoordinate()->x;
            temp[1] = pg->getCoordinate()->y;
            point_results.emplace_back(temp);
        }
        
    }

    return point_results;
}


inline size_t count() {
    return _qdtree->size();
}


private:
QDTree_t* _qdtree;
GEOSGeometry geos_points;
};


}
}
