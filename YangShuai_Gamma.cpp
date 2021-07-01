// YangShuai_Gamma.C
// Copyright (c) 2009 The Foundry Visionmongers Ltd.  All Rights Reserved.

static const char* const HELP = "YangShuai_Gamma Correction to a set of channels";

#include "DDImage/PixelIop.h"
#include "DDImage/Row.h"
#include "DDImage/Knobs.h"

using namespace DD::Image;

class YangShuai_Gamma : public PixelIop
{
  float value[4];
public:
  void in_channels(int input, ChannelSet& mask) const;
  YangShuai_Gamma(Node* node) : PixelIop(node)
  {
    value[0] = value[1] = value[2] = value[3] = 1;//default
  }
  bool pass_transform() const { return true; }
  void pixel_engine(const Row &in, int y, int x, int r, ChannelMask, Row & out);
  virtual void knobs(Knob_Callback);
  static const Iop::Description d;
  const char* Class() const { return d.name; }
  const char* node_help() const { return HELP; }
  void _validate(bool);
};

void YangShuai_Gamma::_validate(bool for_real)
{
  copy_info();
  for (unsigned i = 0; i < 4; i++) {
    if (value[i]) {
      set_out_channels(Mask_All);
      info_.black_outside(false);
      return;
    }
  }
  set_out_channels(Mask_None);
}

void YangShuai_Gamma::in_channels(int input, ChannelSet& mask) const
{
  // mask is unchanged
}

void YangShuai_Gamma::pixel_engine(const Row& in, int y, int x, int r,
                       ChannelMask channels, Row& out)
{
  foreach (z, channels) {
    const float c = value[colourIndex(z)];
    const float* inptr = in[z] + x;
    const float* END = inptr + (r - x);
    float* outptr = out.writable(z) + x;
    while (inptr < END)
      *outptr++ = pow(*inptr++ , 1/c);
  }
}

void YangShuai_Gamma::knobs(Knob_Callback f)
{
  AColor_knob(f, value, IRange(0, 4), "value");
}

#include "DDImage/NukeWrapper.h"

static Iop* build(Node* node) { return new NukeWrapper(new YangShuai_Gamma(node)); }
const Iop::Description YangShuai_Gamma::d("YangShuai_Gamma", "Color/Math/YangShuai_Gamma", build);
