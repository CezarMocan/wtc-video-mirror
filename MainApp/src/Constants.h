#pragma once

// Kinect depth image resolution
#define DEPTH_WIDTH 512
#define DEPTH_HEIGHT 424
#define DEPTH_SIZE DEPTH_WIDTH * DEPTH_HEIGHT

// Kinect color image resolution
#define COLOR_WIDTH 1920
#define COLOR_HEIGHT 1080

// Eye-balled parameters for overlaying kinect depth feed with color feed.
// These can be changed from the GUI of the app at runtime
#define DEPTH_DEFAULT_SCALE 3.08
#define DEPTH_DEFAULT_OFFSET_X 160.0
#define DEPTH_DEFAULT_OFFSET_Y -112.0
