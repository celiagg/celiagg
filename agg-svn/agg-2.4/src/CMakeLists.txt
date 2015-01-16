# The MIT License (MIT)
#
# Copyright (c) 2014 WUSTL ZPLAB
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# Authors: Erik Hvatum <ice.rikh@gmail.com>


SET( antigrain_HEADERS
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_alpha_mask_u8.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_arc.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_array.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_arrowhead.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_basics.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_bezier_arc.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_bitset_iterator.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_blur.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_bounding_rect.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_bspline.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_clip_liang_barsky.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_color_gray.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_color_rgba.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_config.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_adaptor_vcgen.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_adaptor_vpgen.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_bspline.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_clip_polygon.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_clip_polyline.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_close_polygon.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_concat.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_contour.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_curve.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_dash.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_gpc.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_marker.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_marker_adaptor.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_segmentator.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_shorten_path.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_smooth_poly1.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_stroke.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_transform.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_conv_unclose_polygon.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_curves.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_dda_line.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_ellipse.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_ellipse_bresenham.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_embedded_raster_fonts.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_font_cache_manager.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_gamma_functions.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_gamma_lut.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_glyph_raster_bin.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_gradient_lut.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_gsv_text.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_image_accessors.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_image_filters.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_line_aa_basics.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_math.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_math_stroke.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_path_length.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_path_storage.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_path_storage_integer.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_pattern_filters_rgba.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_pixfmt_amask_adaptor.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_pixfmt_gray.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_pixfmt_rgb.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_pixfmt_rgb_packed.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_pixfmt_rgba.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_pixfmt_transposer.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_rasterizer_cells_aa.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_rasterizer_compound_aa.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_rasterizer_outline.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_rasterizer_outline_aa.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_rasterizer_scanline_aa.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_rasterizer_sl_clip.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_renderer_base.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_renderer_markers.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_renderer_mclip.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_renderer_outline_aa.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_renderer_outline_image.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_renderer_primitives.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_renderer_raster_text.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_renderer_scanline.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_rendering_buffer.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_rendering_buffer_dynarow.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_rounded_rect.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_scanline_bin.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_scanline_boolean_algebra.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_scanline_p.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_scanline_storage_aa.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_scanline_storage_bin.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_scanline_u.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_shorten_path.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_simul_eq.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_allocator.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_converter.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_gouraud.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_gouraud_gray.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_gouraud_rgba.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_gradient.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_gradient_alpha.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_image_filter.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_image_filter_gray.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_image_filter_rgb.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_image_filter_rgba.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_interpolator_adaptor.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_interpolator_linear.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_interpolator_persp.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_interpolator_trans.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_pattern_gray.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_pattern_rgb.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_pattern_rgba.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_solid.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_subdiv_adaptor.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_trans_affine.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_trans_bilinear.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_trans_double_path.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_trans_perspective.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_trans_single_path.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_trans_viewport.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_trans_warp_magnifier.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_vcgen_bspline.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_vcgen_contour.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_vcgen_dash.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_vcgen_markers_term.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_vcgen_smooth_poly1.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_vcgen_stroke.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_vcgen_vertex_sequence.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_vertex_sequence.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_vpgen_clip_polygon.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_vpgen_clip_polyline.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_vpgen_segmentator.h

    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_gradient_contour.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/agg_span_gradient_image.h

    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/agg2d/agg2d.h

    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/ctrl/agg_slider_ctrl.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/ctrl/agg_spline_ctrl.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/ctrl/agg_scale_ctrl.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/ctrl/agg_rbox_ctrl.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/ctrl/agg_polygon_ctrl.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/ctrl/agg_gamma_spline.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/ctrl/agg_gamma_ctrl.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/ctrl/agg_ctrl.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/ctrl/agg_cbox_ctrl.h
    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/ctrl/agg_bezier_ctrl.h

    ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/gpc/gpc.h
)

SET( antigrain_SOURCES
    agg_arc.cpp
    agg_arrowhead.cpp
    agg_bezier_arc.cpp
    agg_bspline.cpp
    agg_curves.cpp
    agg_embedded_raster_fonts.cpp
    agg_gsv_text.cpp
    agg_image_filters.cpp
    agg_line_aa_basics.cpp
    agg_line_profile_aa.cpp
    agg_rounded_rect.cpp
    agg_sqrt_tables.cpp
    agg_trans_affine.cpp
    agg_trans_double_path.cpp
    agg_trans_single_path.cpp
    agg_trans_warp_magnifier.cpp
    agg_vcgen_bspline.cpp
    agg_vcgen_contour.cpp
    agg_vcgen_dash.cpp
    agg_vcgen_markers_term.cpp
    agg_vcgen_smooth_poly1.cpp
    agg_vcgen_stroke.cpp
    agg_vpgen_clip_polygon.cpp
    agg_vpgen_segmentator.cpp

    ../agg2d/agg2d.cpp

    ctrl/agg_spline_ctrl.cpp
    ctrl/agg_slider_ctrl.cpp
    ctrl/agg_scale_ctrl.cpp
    ctrl/agg_rbox_ctrl.cpp
    ctrl/agg_polygon_ctrl.cpp
    ctrl/agg_gamma_spline.cpp
    ctrl/agg_gamma_ctrl.cpp
    ctrl/agg_cbox_ctrl.cpp
    ctrl/agg_bezier_ctrl.cpp

    ../gpc/gpc.c

    ${antigrain_HEADERS}
)

if(AGG2D_USE_FREETYPE)
    list( APPEND antigrain_SOURCES
         ../font_freetype/agg_font_freetype.cpp
         ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/font_freetype/agg_font_freetype.h
        )
else()
    list( APPEND antigrain_SOURCES
         ../font_win32_tt/agg_font_win32_tt.cpp
         ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/font_win32_tt/agg_font_win32_tt.h
        )
endif()

add_library(antigrain STATIC ${antigrain_SOURCES})
target_link_libraries(antigrain "")
