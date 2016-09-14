/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gtest/gtest.h"

#include "TROOT.h"
#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/DataStructure/ImageData/ImageData.hh"
#include "src/common_cpp/DataStructure/ImageData/Image.hh"
#include "src/common_cpp/DataStructure/DateTime.hh"

namespace MAUS {

TEST(ImageTest, ImageTest) {
    Image image;
    EXPECT_EQ(image.GetRunNumber(), 0);
    EXPECT_EQ(image.GetSpillNumber(), 0);
    EXPECT_EQ(image.GetInputTime().GetDateTime(), DateTime().GetDateTime());
    EXPECT_EQ(image.GetOutputTime().GetDateTime(), DateTime().GetDateTime());
    EXPECT_EQ(image.GetCanvasWrappers().size(), size_t(0));

    // Mutators, etc
    image.CanvasWrappersPushBack(new MAUS::CanvasWrapper());
    EXPECT_EQ(image.GetCanvasWrappers().size(), size_t(1));

    CanvasWrapper* wrap = new CanvasWrapper();
    wrap->SetCanvas(new TCanvas("name", "title"));
    image.CanvasWrappersPushBack(wrap);
    EXPECT_EQ(image.GetCanvasWrappers().size(), size_t(2));
    CanvasWrapper* wrap_copy = image.GetCanvasWrappers()[1];
    EXPECT_EQ(std::string(wrap_copy->GetCanvas()->GetTitle()),
              std::string("title"));

    DateTime dt_test1, dt_test2;
    dt_test1.SetDateTime("1989-04-04T00:00:00.000000");
    dt_test2.SetDateTime("1991-04-04T00:00:00.000000");

    image.SetRunNumber(99);
    EXPECT_EQ(image.GetRunNumber(), 99);
    image.SetSpillNumber(100);
    EXPECT_EQ(image.GetSpillNumber(), 100);
    image.SetInputTime(dt_test1);
    EXPECT_EQ(image.GetInputTime().GetDateTime(), dt_test1.GetDateTime());
    image.SetOutputTime(dt_test2);
    EXPECT_EQ(image.GetOutputTime().GetDateTime(), dt_test2.GetDateTime());

    // Copy constructor
    Image image_cpy_ctor(image);
    EXPECT_EQ(image.GetRunNumber(), image_cpy_ctor.GetRunNumber());
    EXPECT_EQ(image.GetSpillNumber(), image_cpy_ctor.GetSpillNumber());
    EXPECT_EQ(image.GetInputTime().GetDateTime(),
              image_cpy_ctor.GetInputTime().GetDateTime());
    EXPECT_EQ(image.GetOutputTime().GetDateTime(),
              image_cpy_ctor.GetOutputTime().GetDateTime());
    ASSERT_EQ(image_cpy_ctor.GetCanvasWrappers().size(), 2);
    wrap_copy = image_cpy_ctor.GetCanvasWrappers()[1];
    EXPECT_EQ(std::string(wrap_copy->GetCanvas()->GetTitle()),
              std::string("title"));

    // Assignment
    Image image_ass_op;
    image_ass_op = image;
    EXPECT_EQ(image.GetRunNumber(), image_ass_op.GetRunNumber());
    EXPECT_EQ(image.GetSpillNumber(), image_ass_op.GetSpillNumber());
    EXPECT_EQ(image.GetInputTime().GetDateTime(),
              image_ass_op.GetInputTime().GetDateTime());
    EXPECT_EQ(image.GetOutputTime().GetDateTime(),
              image_ass_op.GetOutputTime().GetDateTime());
    ASSERT_EQ(image_ass_op.GetCanvasWrappers().size(), 2);
    wrap_copy = image_ass_op.GetCanvasWrappers()[1];
    EXPECT_EQ(std::string(wrap_copy->GetCanvas()->GetTitle()),
              std::string("title"));
}

TEST(ImageTest, ImageDataTest) {
    ImageData image_data;
    Image* image = new Image();
    image->SetRunNumber(99);
    EXPECT_EQ(image_data.GetEventType(), "ImageData");
    image_data.SetImage(image);
    EXPECT_EQ(image_data.GetImage(), image);
    // check we deep copy the image_data
    ImageData image_data_copy_ctor(image_data);
    EXPECT_EQ(image_data.GetImage()->GetRunNumber(),
              image_data_copy_ctor.GetImage()->GetRunNumber());
    EXPECT_NE(image_data.GetImage(),
              image_data_copy_ctor.GetImage());
    // check we deep copy on assignment operator
    ImageData image_data_ass_op;
    image_data_ass_op = image_data;
    EXPECT_EQ(image_data.GetImage()->GetRunNumber(),
              image_data_copy_ctor.GetImage()->GetRunNumber());
    EXPECT_NE(image_data.GetImage(),
              image_data_copy_ctor.GetImage());
}

TEST(ImageTest, CanvasWrapperTest) {
    CanvasWrapper cwrap_def;
    CanvasWrapper cwrap_values;
    cwrap_values.SetDescription("description");
    TCanvas* canv = new TCanvas("name", "title");
    cwrap_values.SetCanvas(canv);
    EXPECT_EQ(cwrap_values.GetDescription(), "description");
    EXPECT_EQ(cwrap_values.GetCanvas(), canv);

    CanvasWrapper cwrap_eq;
    cwrap_eq = cwrap_def;
    EXPECT_EQ(cwrap_def.GetDescription(), cwrap_eq.GetDescription());
    EXPECT_EQ(cwrap_def.GetCanvas(), cwrap_eq.GetCanvas());
    cwrap_eq = cwrap_values;
    EXPECT_EQ(cwrap_values.GetDescription(), cwrap_eq.GetDescription());
    EXPECT_EQ(std::string("title"),
              std::string(cwrap_eq.GetCanvas()->GetTitle()));

    CanvasWrapper cwrap_copy(cwrap_def);
    EXPECT_EQ(cwrap_def.GetDescription(), cwrap_copy.GetDescription());
    EXPECT_EQ(cwrap_def.GetCanvas(), cwrap_copy.GetCanvas());

    CanvasWrapper cwrap_copy_vals(cwrap_values);
    EXPECT_EQ(cwrap_values.GetDescription(), cwrap_copy_vals.GetDescription());
    EXPECT_EQ(std::string("title"),
              std::string(cwrap_copy_vals.GetCanvas()->GetTitle()));
}

class TestMainFrame : public TGMainFrame {
  public:
    TestMainFrame(const TGWindow *p, UInt_t w, UInt_t h) {}
    virtual ~TestMainFrame() {}
};

// Reported unstable; note this requires TApplication to work, so may be a
// problem there
TEST(ImageTest, DISABLED_EmbedCanvasWrapperTest) {
    bool batch = gROOT->IsBatch();
    gROOT->SetBatch(false);
    TestMainFrame window(gClient->GetRoot(), 100, 100);
    TRootEmbeddedCanvas embed("embed", &window, 100, 100);
    CanvasWrapper cwrap_def;
    CanvasWrapper cwrap_values;
    TCanvas* canv = new TCanvas("name", "title");
    cwrap_values.SetCanvas(canv);
    EXPECT_THROW(cwrap_def.EmbedCanvas(&embed), MAUS::Exceptions::Exception);
    EXPECT_THROW(cwrap_values.EmbedCanvas(NULL), MAUS::Exceptions::Exception);
    cwrap_values.EmbedCanvas(&embed);
    EXPECT_EQ(embed.GetCanvas(), canv);
    gROOT->SetBatch(batch);
}
}
