/*
* Copyright 2019-2020, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/
#ifndef MODEL_SETTINGS_H_
#define MODEL_SETTINGS_H_

#define kNumCols        28
#define kNumRows        28
#define kNumChannels    1
#define kImageSize      (kNumCols * kNumRows * kNumChannels)

#define kCategoryCount  26

extern const char* kCategoryLabels[kCategoryCount];
#endif // MODEL_SETTINGS_H_
