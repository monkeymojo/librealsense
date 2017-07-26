// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#pragma once

#include <realsense_msgs/stream_info.h>
#include "stream_data.h"
namespace librealsense
{
    namespace file_format
    {
        namespace ros_data_objects
        {
            struct image_stream_data
            {
                uint32_t width;
                uint32_t height;
                rs2_intrinsics intrinsics;
                file_types::stream_extrinsics stream_extrinsics;
                uint32_t device_id;
                rs2_stream type;
                uint32_t fps;
                rs2_format format;
            };

            class image_stream_info : public stream_info
            {
            public:
                image_stream_info(const image_stream_data& info) : m_info(info)
                {
                    assign(info, m_info);
                }
                static std::string get_topic(const uint32_t& device_id)
                {
                    return "/camera/rs_stream_info/" + std::to_string(device_id);
                }
                void write_data(data_object_writer& file) override
                {
                    realsense_msgs::stream_info msg;
                    conversions::convert(m_info.type, msg.stream_type);
                    msg.fps = m_info.fps;
                    msg.width = m_info.width;
                    msg.height = m_info.height;
                    conversions::convert(m_info.format, msg.encoding);
                    msg.camera_info.height = m_info.height;
                    msg.camera_info.width  = m_info.width;
                    msg.camera_info.K[0]   = m_info.intrinsics.fx;
                    msg.camera_info.K[2]   = m_info.intrinsics.ppx;

                    msg.camera_info.K[4] = m_info.intrinsics.fy;
                    msg.camera_info.K[5] = m_info.intrinsics.ppy;
                    msg.camera_info.K[8] = 1;
                    msg.camera_info.D.assign(m_info.intrinsics.coeffs, m_info.intrinsics.coeffs + 5);

                    conversions::convert(m_info.intrinsics.model, msg.camera_info.distortion_model);
                    conversions::convert(m_info.stream_extrinsics.extrinsics_data, msg.stream_extrinsics.extrinsics);
                    msg.stream_extrinsics.reference_point_id = m_info.stream_extrinsics.reference_point_id;
                    file.write(get_topic(m_info.device_id), file_types::nanoseconds::min(), msg);
                }

                const image_stream_data& get_info() const
                {
                    return m_info;
                }

                void set_info(const image_stream_data &info)
                {
                    assign(info, m_info);
                }


            private:
                void assign(const image_stream_data& in_info, image_stream_data& out_info)
                {
                    out_info = in_info;
                    out_info.intrinsics.fx = in_info.intrinsics.fx;
                    out_info.intrinsics.ppx = in_info.intrinsics.ppx;
                    out_info.intrinsics.fy = in_info.intrinsics.fy;
                    out_info.intrinsics.ppy = in_info.intrinsics.ppy;
                    std::memcpy(out_info.intrinsics.coeffs,
                                in_info.intrinsics.coeffs,
                                sizeof(out_info.intrinsics.coeffs));

                    std::memcpy(out_info.stream_extrinsics.extrinsics_data.rotation,
                                in_info.stream_extrinsics.extrinsics_data.rotation,
                                sizeof(out_info.stream_extrinsics.extrinsics_data.rotation));
                    std::memcpy(out_info.stream_extrinsics.extrinsics_data.translation,
                                in_info.stream_extrinsics.extrinsics_data.translation,
                                sizeof(out_info.stream_extrinsics.extrinsics_data.translation));
                    out_info.stream_extrinsics.reference_point_id = in_info.stream_extrinsics.reference_point_id;
                }

                image_stream_data m_info;

            };
        }
    }
}
