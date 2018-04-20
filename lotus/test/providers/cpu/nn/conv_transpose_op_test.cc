#include "core/graph/utils.h"
#include "gtest/gtest.h"
#include "test/providers/provider_test_utils.h"

namespace Lotus {
namespace Test {

namespace {

struct ConvTransposeOpAttributes {
  vector<int64_t> kernel_shape;
  vector<int64_t> output_padding;
  vector<int64_t> output_shape;
  vector<int64_t> pads;
  vector<int64_t> strides;
};

void TestConvTransposeOp(const ConvTransposeOpAttributes& attributes,
                         const vector<vector<float>>& inputs,
                         const vector<vector<int64_t>>& input_shapes,
                         const std::initializer_list<float>& expected_output,
                         const vector<int64_t>& expected_output_shape) {
  OpTester test("ConvTranspose");
  test.AddAttribute("kernel_shape", attributes.kernel_shape);
  if (!attributes.output_padding.empty()) {
    test.AddAttribute("output_padding", attributes.output_padding);
  }
  if (!attributes.output_shape.empty()) {
    test.AddAttribute("output_shape", attributes.output_shape);
  }
  test.AddAttribute("pads", attributes.pads);
  test.AddAttribute("strides", attributes.strides);

  LOTUS_ENFORCE(inputs.size() <= 3, "Our name array is only setup to handle 3 inputs");
  const char* szNames[] = {"X", "W", "B"};
  for (int i = 0; i < inputs.size(); i++) {
    test.AddInput<float>(szNames[i], input_shapes[i], inputs[i]);
  }
  test.AddOutput<float>("Y", expected_output_shape, expected_output);
  test.Run();
}
}  // namespace

TEST(ConvTransposeTest, ConvTranspose_1) {
  ConvTransposeOpAttributes attrs = {
      vector<int64_t>{3, 3},        // kernel_shape
      vector<int64_t>{1, 1},        // output_padding
      {},                           // output_shape
      vector<int64_t>{1, 1, 1, 1},  // pads
      vector<int64_t>{2, 2}         // strides
  };
  vector<float> X = {0.16857791f, -0.15161794f, 0.08540368f,
                     0.1820628f, -0.21746576f, 0.08245695f,
                     0.1431433f, -0.43156421f, 0.30591947f};
  vector<int64_t> X_shape = {1, 1, 3, 3};
  vector<float> W = {-0.06230065f, 0.37932432f, -0.25388849f,
                     0.33878803f, 0.43709868f, -0.22477469f,
                     0.04118127f, -0.44696793f, 0.06373066f};
  vector<int64_t> W_shape = {1, 1, 3, 3};
  vector<int64_t> Y_shape = {1, 1, 6, 6};
  auto expected_vals = {0.07368518f, -0.08925839f, -0.06627201f, 0.06301362f, 0.03732984f, -0.01919658f,
                        -0.00628807f, -0.02817563f, -0.01472169f, 0.04392925f, -0.00689478f, -0.01549204f,
                        0.07957941f, -0.11459791f, -0.09505399f, 0.07681622f, 0.03604182f, -0.01853423f,
                        -0.0270785f, -0.00680824f, -0.06650258f, 0.08004665f, 0.07918708f, -0.0724144f,
                        0.06256775f, -0.17838378f, -0.18863615f, 0.20064656f, 0.133717f, -0.06876295f,
                        -0.06398046f, -0.00864975f, 0.19289537f, -0.01490572f, -0.13673618f, 0.01949645f};
  TestConvTransposeOp(attrs, {X, W}, {X_shape, W_shape}, expected_vals, Y_shape);
}

TEST(ConvTransposeTest, ConvTranspose_Bias_1) {
  ConvTransposeOpAttributes attrs = {
      vector<int64_t>{3, 3},        // kernel_shape
      vector<int64_t>{0, 0},        // output_padding
      {},                           // output_shape
      vector<int64_t>{1, 1, 1, 1},  // pads
      vector<int64_t>{1, 1}         // strides
  };
  vector<float> X = {0.22572887f, -0.07105902f, -0.40399021f, -0.14461157f, 0.05367219f,
                     -0.08353302f, 0.41023391f, 0.42745841f, -0.3769345f, -0.42057109f,
                     -0.1372498f, 0.05485916f, 0.34602994f, -0.06402895f, -0.06000063f,
                     0.07891446f, -0.09410021f, 0.26251942f, -0.11043271f, 0.47966552f,
                     0.34682763f, -0.04511502f, 0.22414422f, 0.24618894f, -0.21480265f};
  vector<int64_t> X_shape = {1, 1, 5, 5};
  vector<float> W = {-0.0962126f, 0.19827795f, 0.03667754f,
                     0.36756599f, -0.01076147f, -0.11781135f,
                     -0.11574665f, -0.38404959f, 0.44403327f};
  vector<int64_t> W_shape = {1, 1, 3, 3};
  vector<float> B = {0.04676145f};
  vector<int64_t> B_shape = {1};
  vector<int64_t> Y_shape = {1, 1, 5, 5};
  auto expected_vals = {-0.03781903f, -0.09041066f, 0.14239404f, 0.09704495f, -0.03399426f,
                        0.08749044f, 0.35613984f, 0.07240347f, -0.27841991f, -0.00337578f,
                        0.07770107f, -0.09561026f, 0.13388641f, 0.30945939f, 0.14015588f,
                        0.13079405f, -0.00488365f, -0.06758944f, 0.45621645f, 0.01566098f,
                        0.00703105f, 0.12956856f, 0.0103332f, 0.04221053f, -0.21318194f};
  TestConvTransposeOp(attrs, {X, W, B}, {X_shape, W_shape, B_shape}, expected_vals, Y_shape);
}

TEST(ConvTransposeTest, ConvTranspose_Bias_2) {
  ConvTransposeOpAttributes attrs = {
      vector<int64_t>{2, 2},        // kernel_shape
      vector<int64_t>{0, 0},        // output_padding
      {},                           // output_shape
      vector<int64_t>{0, 0, 0, 0},  // pads
      vector<int64_t>{1, 1}         // strides
  };
  vector<float> X = {0.01270282f, 0.09657472f, -0.36909008f, -0.08085269f,
                     0.0242992f, 0.40873009f, -0.46927932f, 0.34412372f,
                     -0.39574206f, 0.26234281f, 0.27352369f, -0.22265741f,
                     0.43270493f, -0.24710381f, -0.03418651f, -0.04413456f,
                     -0.16414353f, 0.3158558f, 0.1087395f, -0.38577938f,
                     -0.38986659f, -0.09614426f, 0.17591673f, 0.40140027f,
                     -0.0869683f, -0.47193506f, -0.05010766f, 0.29325962f,
                     0.22680271f, -0.0793834f, -0.36764491f, 0.20451134f,
                     0.46361887f, -0.12190259f, 0.03413916f, 0.12307656f,
                     0.28569579f, -0.392129f, 0.17179191f, 0.27161086f,
                     -0.12766263f, 0.1371125f, 0.28137422f, -0.39899838f,
                     0.23824286f, -0.19693244f, 0.32956779f, 0.46209556f,
                     -0.46913007f};
  vector<int64_t> X_shape = {1, 1, 7, 7};
  vector<float> W = {-0.34922412f, 0.1114341f, -0.01778314f, 0.46861196f};
  vector<int64_t> W_shape = {1, 1, 2, 2};
  vector<float> B = {0.17402864f};
  vector<int64_t> B_shape = {1};
  vector<int64_t> Y_shape = {1, 1, 8, 8};
  auto expected_vals = {0.1695925f, 0.14171794f, 0.31368554f, 0.16113512f,
                        0.15653302f, 0.033998f, 0.38345876f, 0.12173492f,
                        0.05362644f, 0.35481372f, 0.09013268f, -0.06378071f,
                        0.24394518f, 0.00222442f, 0.50842237f, -0.07341707f,
                        0.17984779f, 0.35392997f, 0.03631867f, 0.16350585f,
                        0.30338728f, 0.2088346f, 0.47435546f, 0.0147884f,
                        0.20821247f, 0.08664516f, 0.03569011f, 0.16659322f,
                        0.47522858f, 0.19675478f, -0.10781619f, 0.02401161f,
                        0.0965334f, 0.1788421f, 0.36887163f, 0.2512877f,
                        0.00254938f, 0.04799958f, 0.11982619f, 0.31525785f,
                        0.12701407f, 0.19566584f, 0.31214368f, -0.10558143f,
                        0.18591091f, 0.46830338f, 0.05418756f, 0.20530567f,
                        0.07357728f, 0.39731777f, 0.1872202f, 0.08253923f,
                        0.11266428f, 0.17892915f, 0.32709083f, 0.1860041f,
                        0.16902491f, 0.3129794f, -0.01718347f, 0.28917417f,
                        0.07588299f, 0.32025051f, 0.39891475f, -0.04581133f};
  TestConvTransposeOp(attrs, {X, W, B}, {X_shape, W_shape, B_shape}, expected_vals, Y_shape);
}

TEST(ConvTransposeTest, ConvTranspose_Output_Shape) {
  ConvTransposeOpAttributes attrs = {
      vector<int64_t>{3, 3},        // kernel_shape
      {},                           // output_padding
      vector<int64_t>{1, 3, 4, 4},  // output_shape
      vector<int64_t>{0, 0, 0, 0},  // pads
      vector<int64_t>{1, 1}         // strides
  };
  int image_size = 4 * 4;
  int input_channels = 3;
  int output_channels = 3;
  std::vector<float> X;
  for (int i = 0; i < input_channels * image_size; i++)
    X.push_back(1.0f);
  std::vector<float> W;
  int kernel_size = output_channels * input_channels * 3 * 3;
  for (int i = 0; i < kernel_size; i++)
    W.push_back(1.0f);

  vector<int64_t> X_shape = {1, 3, 4, 4};
  vector<int64_t> W_shape = {3, 3, 3, 3};

  vector<int64_t> Y_shape = {1, 3, 4, 4};
  auto expected_vals = {12.0f, 18.0f, 18.0f, 12.0f,
                        18.0f, 27.0f, 27.0f, 18.0f,
                        18.0f, 27.0f, 27.0f, 18.0f,
                        12.0f, 18.0f, 18.0f, 12.0f,
                        12.0f, 18.0f, 18.0f, 12.0f,
                        18.0f, 27.0f, 27.0f, 18.0f,
                        18.0f, 27.0f, 27.0f, 18.0f,
                        12.0f, 18.0f, 18.0f, 12.0f,
                        12.0f, 18.0f, 18.0f, 12.0f,
                        18.0f, 27.0f, 27.0f, 18.0f,
                        18.0f, 27.0f, 27.0f, 18.0f,
                        12.0f, 18.0f, 18.0f, 12.0f};
  TestConvTransposeOp(attrs, {X, W}, {X_shape, W_shape}, expected_vals, Y_shape);
}

}  // namespace Test
}  // namespace Lotus