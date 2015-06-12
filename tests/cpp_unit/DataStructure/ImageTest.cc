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

#include "TApplication.h"
#include "TROOT.h"
#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/DataStructure/ImageData.hh"
#include "src/common_cpp/DataStructure/Image.hh"
#include "src/common_cpp/DataStructure/DateTime.hh"

namespace MAUS {

TEST(ImageTest, ImageTest) {
    Image image;
    EXPECT_EQ(image.GetRunNumber(), 0);
    EXPECT_EQ(image.GetSpillNumber(), 0);
    EXPECT_EQ(image.GetInputTime().GetDateTime(), DateTime().GetDateTime());
    EXPECT_EQ(image.GetOutputTime().GetDateTime(), DateTime().GetDateTime());
    EXPECT_EQ(image.GetCanvasWrappers().size(), size_t(0));
    image.CanvasWrappersPushBack(new MAUS::CanvasWrapper());
    EXPECT_EQ(image.GetCanvasWrappers().size(), size_t(1));

    CanvasWrapper* wrap = new CanvasWrapper();
    wrap->SetCanvas(new TCanvas("name", "title"));
    image.CanvasWrappersPushBack(wrap);
    EXPECT_EQ(image.GetCanvasWrappers().size(), size_t(2));
    CanvasWrapper* wrap_copy = image.GetCanvasWrappers()[1];
    EXPECT_EQ(std::string(wrap_copy->GetCanvas()->GetTitle()),
              std::string("title"));
}

TEST(ImageTest, ImageDataTest) {
    EXPECT_TRUE(false);
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


TEST(ImageTest, EmbedCanvasWrapperTest) {
    // This fails due to ROOT segmentation fault, because window is not
    // sufficiently well defined...
    bool batch = gROOT->IsBatch();
    gROOT->SetBatch(false);
    std::cerr << "A" << std::endl;
    int argc = 1;
    char* argv[] = {const_cast<char*>("")};
    TApplication theApp("App", &argc, argv);
    std::cerr << "A1" << std::endl;
    TestMainFrame window(gClient->GetRoot(), 100, 100);
    std::cerr << "B" << std::endl;
    TRootEmbeddedCanvas embed("embed", &window, 100, 100);
    std::cerr << "1" << std::endl;
    CanvasWrapper cwrap_def;
    std::cerr << "2" << std::endl;
    CanvasWrapper cwrap_values;
    std::cerr << "3" << std::endl;
    TCanvas* canv = new TCanvas("name", "title");
    std::cerr << "4" << std::endl;
    cwrap_values.SetCanvas(canv);
    std::cerr << "5" << std::endl;
    EXPECT_THROW(cwrap_def.EmbedCanvas(&embed), MAUS::Exception);
    std::cerr << "6" << std::endl;
    EXPECT_THROW(cwrap_values.EmbedCanvas(NULL), MAUS::Exception);
    std::cerr << "B" << std::endl;
    cwrap_values.EmbedCanvas(&embed);
    std::cerr << "C" << std::endl;
    EXPECT_EQ(embed.GetCanvas(), canv);
    gROOT->SetBatch(batch);
}
}
