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

#include <agg_bezier_arc.h>
#include "vertex_source.h"

// ----------------------------------------------------------------------------

BsplineSource::BsplineSource(const double* points, const size_t point_count)
: m_verts(points, point_count, false, false)
, m_spline(m_verts)
, m_vert_count(point_count)
{
}

void
BsplineSource::rewind(unsigned path_id)
{
    m_spline.rewind(path_id);
}

unsigned
BsplineSource::vertex(double* x, double* y)
{
    return m_spline.vertex(x, y);
}

unsigned
BsplineSource::total_vertices() const
{
    return m_vert_count;
}

// ----------------------------------------------------------------------------

PathSource::PathSource()
: m_path()
, m_curve(m_path)
{
}

void
PathSource::rewind(unsigned path_id)
{
    m_curve.rewind(path_id);
}

unsigned
PathSource::vertex(double* x, double* y)
{
    return m_curve.vertex(x, y);
}

unsigned
PathSource::total_vertices() const
{
    return m_path.total_vertices();
}

void PathSource::begin()
{
    m_path.start_new_path();
}

void PathSource::close()
{
    m_path.close_polygon();
}

void PathSource::reset()
{
    m_path.remove_all();
}

unsigned PathSource::last_vertex(double* x, double* y) const
{
    return m_path.last_vertex(x, y);
}

void PathSource::move_to(double x, double y)
{
    m_path.move_to(x, y);
}

void PathSource::line_to(double x, double y)
{
    m_path.line_to(x, y);
}

void PathSource::arc_to(double rx, double ry, double angle, bool large_arc_flag,
                         bool sweep_flag, double x, double y)
{
    m_path.arc_to(rx, ry, angle, large_arc_flag, sweep_flag, x, y);
}

void PathSource::quadric_to(double x_ctrl, double y_ctrl, double x_to, double y_to)
{
    m_path.curve3(x_ctrl, y_ctrl, x_to, y_to);
}

void PathSource::cubic_to(double x_ctrl1, double y_ctrl1, double x_ctrl2,
                           double y_ctrl2,  double x_to, double y_to)
{
    m_path.curve4(x_ctrl1, y_ctrl1, x_ctrl2, y_ctrl2, x_to, y_to);
}

void PathSource::ellipse(double cx, double cy, double rx, double ry)
{
    agg::bezier_arc arc(cx, cy, rx, ry, 0, M_PI + M_PI);
    m_path.concat_path(arc, 0);
    m_path.close_polygon();
}

// ----------------------------------------------------------------------------

RepeatedSource::RepeatedSource(VertexSource& source, const double* points, const size_t point_count)
: m_source(&source)
, m_points(points, point_count, false, false)
, m_point_count(point_count)
, m_current_point(0)
, m_current_trans()
{
    _get_transform();
}

void
RepeatedSource::rewind(unsigned path_id)
{
    m_current_point = 0;
    m_points.rewind(0);
    m_source->rewind(path_id);
    _get_transform();
}

unsigned
RepeatedSource::vertex(double* x, double* y)
{
    if (m_current_point == m_point_count)
    {
        return agg::path_cmd_stop;
    }

    unsigned cmd = m_source->vertex(x, y);
    if (agg::is_vertex(cmd))
    {
        m_current_trans.transform(x, y);
    }
    else if (agg::is_stop(cmd))
    {
        m_source->rewind(0);
        _get_transform();
        m_current_point += 1;
        cmd = agg::path_cmd_end_poly;
    }

    return cmd;
}

unsigned
RepeatedSource::total_vertices() const
{
    return m_source->total_vertices() * m_point_count;
}

void
RepeatedSource::_get_transform()
{
    double x = 0.0, y = 0.0;
    m_points.vertex(&x, &y);
    m_current_trans = agg::trans_affine_translation(x, y);
}
