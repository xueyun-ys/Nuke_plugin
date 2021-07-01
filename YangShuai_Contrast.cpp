// Contrast.C
// Copyright (c) 2009 The Foundry Visionmongers Ltd.  All Rights Reserved.

static const char* const HELP = "change the contrast of the image";

#include "DDImage/PixelIop.h"
#include "DDImage/Row.h"
#include "DDImage/Knobs.h"

using namespace DD::Image;

class Contrast : public PixelIop
{
  float value[4];
  float p1[4];
  float p2[4];
  //float p1, p2;
public:
  void in_channels(int input, ChannelSet& mask) const;
  Contrast(Node* node) : PixelIop(node)
  {
    value[0] = value[1] = value[2] = value[3] = 0;
    p1[0] = p1[1] = p1[2] = p1[3]= 0.33;
    p2[0] = p2[1] = p2[2] = p2[3]= 3;
  }
  bool pass_transform() const { return true; }
  void pixel_engine(const Row &in, int y, int x, int r, ChannelMask, Row & out);
  virtual void knobs(Knob_Callback);
  static const Iop::Description d;
  const char* Class() const { return d.name; }
  const char* node_help() const { return HELP; }
  void _validate(bool);
};

void Contrast::_validate(bool for_real)
{
  copy_info();
  for (unsigned i = 0; i < 4; i++) {
    if (p1[i] || p2[i]) {
      set_out_channels(Mask_All);
      info_.black_outside(false);
      return;
    }
  }
  set_out_channels(Mask_None);
}

void Contrast::in_channels(int input, ChannelSet& mask) const
{
  // mask is unchanged
}

void Contrast::pixel_engine(const Row& in, int y, int x, int r,
                       ChannelMask channels, Row& out)
{
  foreach (z, channels) {
    //const float c = value[colourIndex(z)];
    const float pp1 = p1[colourIndex(z)];
    const float pp2 = p2[colourIndex(z)];
    const float* inptr = in[z] + x;
    const float* END = inptr + (r - x);
    float* outptr = out.writable(z) + x;
    while (inptr < END)
      *outptr++ = (*inptr++ - pp1) * pp2;
  }
}

void Contrast::knobs(Knob_Callback f)
{
  //AColor_knob(f, value, IRange(0, 4), "value");
  //AColor_knob(f, &p1, IRange(0, 1), "parameter1");
  AColor_knob(f, p1, IRange(0, 1), "parameter1");
  AColor_knob(f, p2, IRange(0, 100), "parameter2");
}

#include "DDImage/NukeWrapper.h"

static Iop* build(Node* node) { return new NukeWrapper(new Contrast(node)); }
const Iop::Description Contrast::d("Contrast", "Color/Math/Contrast", build);
