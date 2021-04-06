//
// Created by Mike Smith on 2021/4/6.
//

#include <opencv2/opencv.hpp>

#include <runtime/context.h>
#include <runtime/device.h>
#include <dsl/syntax.h>
#include <tests/fake_device.h>

using namespace luisa;
using namespace luisa::compute;
using namespace luisa::compute::dsl;

int main(int argc, char *argv[]) {

    log_level_verbose();

    Context context{argv[0]};

#if defined(LUISA_BACKEND_METAL_ENABLED)
    auto device = context.create_device("metal");
#elif defined(LUISA_BACKEND_DX_ENABLED)
    auto device = context.create_device("dx");
#else
    auto device = std::make_unique<FakeDevice>(context);
#endif

    Kernel2D fill_image = [](ImageView<uchar4_sRGB> image) noexcept {
        Var<uint2> coord{dispatch_id().x, dispatch_id().y};
        Var rg = Var<float2>{coord} / Var<float2>{launch_size().x, launch_size().y};
        image[coord] = Var<float4>{rg, 1.0f, 1.0f};
    };
    fill_image.prepare(*device);

    Image<uchar4_sRGB> device_image{*device, {1024u, 1024u}};
    cv::Mat host_image{1024u, 1024u, CV_8UC4, cv::Scalar::all(0)};

    Stream stream{*device};
    stream << fill_image(device_image).launch(1024u, 1024u)
           << device_image.copy_to(host_image.data)
           << synchronize();

    cv::cvtColor(host_image, host_image, cv::COLOR_RGBA2BGR);
    cv::imwrite("result.png", host_image);
}
