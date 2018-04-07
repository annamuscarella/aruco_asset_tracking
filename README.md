# Asset Tracking for Hackathon

The current dictionary size is 4x4 which allows us to use 50 different markers. For a demo, this should be sufficient. More markers lead to less accuracy and more errors.
Sample markers can be found here: http://jevois.org/moddoc/DemoArUco/screenshot2.png

### Installation on Mac

DO NOT use Homebrew, since the library for aruco mappers has to be included manually.

1. Download opencv:

2. Download opencv_contrib

3. Build opencv

cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib-3.4.1/modules  -D BUILD_EXAMPLES=ON ..

4. Install opencv

