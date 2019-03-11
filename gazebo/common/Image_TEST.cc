/*
 * Copyright (C) 2012 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <gtest/gtest.h>
#include <ignition/math/Color.hh>

#include "gazebo/common/Image.hh"
#include "test/util.hh"

using namespace gazebo;

class ImageTest : public gazebo::testing::AutoLogFixture { };

TEST_F(ImageTest, Image)
{
  common::Image img;
  EXPECT_EQ(-1, img.Load("/file/shouldn/never/exist.png"));
  EXPECT_EQ(0, img.Load("file://media/materials/textures/wood.jpg"));
  EXPECT_EQ(static_cast<unsigned int>(496), img.GetWidth());
  EXPECT_EQ(static_cast<unsigned int>(329), img.GetHeight());
  EXPECT_EQ(static_cast<unsigned int>(24), img.GetBPP());
  EXPECT_EQ(img.Pixel(10, 10),
      ignition::math::Color(0.133333f, 0.376471f, 0.654902f, 1));
  EXPECT_EQ(img.AvgColor(),
      ignition::math::Color(0.260456f, 0.506047f, 0.758062f, 1));
  EXPECT_EQ(img.MaxColor(),
      ignition::math::Color(0.807843f, 0.909804f, 0.964706f, 1));
  EXPECT_TRUE(img.Valid());
  EXPECT_TRUE(img.GetFilename().find("materials/textures/wood.jpg") !=
      std::string::npos);

  unsigned char *data = nullptr;
  unsigned int size = 0;
  img.GetData(&data, size);
  EXPECT_EQ(static_cast<unsigned int>(489552), size);

  img.SetFromData(data, img.GetWidth(), img.GetHeight(),
                  common::Image::RGB_INT8);
}


/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
