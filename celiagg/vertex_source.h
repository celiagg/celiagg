// The MIT License (MIT)
//
// Copyright (c) 2016 WUSTL ZPLAB
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Authors: John Wiggins

#ifndef CELIAGG_VERTEX_SOURCE_H
#define CELIAGG_VERTEX_SOURCE_H

#include <math.h>
#include <agg_path_storage.h>
#include <agg_conv_bspline.h>
#include <agg_conv_curve.h>
#include <agg_trans_affine.h>
#include <ctrl/agg_polygon_ctrl.h>

// The AGG Vertex Source interface as an abstract base class
class VertexSource
{
public:
    virtual ~VertexSource() {}

    virtual void        rewind(unsigned path_id) = 0;
    virtual unsigned    vertex(double* x, double* y) = 0;
    virtual unsigned    total_vertices() const = 0;

};

class BsplineSource : public VertexSource
{
    typedef agg::conv_bspline<agg::simple_polygon_vertex_source> conv_bspline_t;

    agg::simple_polygon_vertex_source m_verts;
    conv_bspline_t m_spline;
    size_t m_vert_count;

public:
    BsplineSource(const double* points, const size_t point_count);

    virtual void        rewind(unsigned path_id);
    virtual unsigned    vertex(double* x, double* y);
    virtual unsigned    total_vertices() const;

private:
    // disable
    BsplineSource(const BsplineSource&);
    const BsplineSource& operator=(const BsplineSource&);
};

class PathSource : public VertexSource
{
    typedef agg::conv_curve<agg::path_storage> conv_curve_t;

    agg::path_storage m_path;
    conv_curve_t m_curve;

public:
    virtual void        rewind(unsigned path_id);
    virtual unsigned    vertex(double* x, double* y);
    virtual unsigned    total_vertices() const;

public:
    PathSource();

    void begin();
    void close();
    void reset();

    unsigned last_vertex(double* x, double* y) const;

    template<class VertexSource>
    void concat_path(VertexSource& vs, unsigned path_id)
    {
        m_path.concat_path(vs, path_id);
    }

    void move_to(double x, double y);
    void line_to(double x, double y);

    void arc_to(double x1, double y1, double x2, double y2, double radius);
    void quadric_to(double x_ctrl, double y_ctrl, double x_to, double y_to);
    void cubic_to(double x_ctrl1, double y_ctrl1, double x_ctrl2,
                  double y_ctrl2,  double x_to, double y_to);

    void arc(double x, double y, double radius,
             double start_angle, double end_angle, bool cw);
    void ellipse(double cx, double cy, double rx, double ry);

private:
    void _normalize(double& x, double& y);

private:
    // disable
    PathSource(const PathSource&);
    const PathSource& operator=(const PathSource&);
};

class RepeatedSource : public VertexSource
{
    VertexSource* m_source;
    agg::simple_polygon_vertex_source m_points;
    size_t m_point_count;
    size_t m_current_point;
    agg::trans_affine m_current_trans;

public:
    RepeatedSource(VertexSource& source, const double* points, const size_t point_count);

    virtual void        rewind(unsigned path_id);
    virtual unsigned    vertex(double* x, double* y);
    virtual unsigned    total_vertices() const;

private:
    void _get_transform();

    // disable
    RepeatedSource(const RepeatedSource&);
    const RepeatedSource& operator=(const RepeatedSource&);
};

#endif // CELIAGG_VERTEX_SOURCE_H
