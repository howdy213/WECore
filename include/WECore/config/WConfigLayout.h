/**
 * @author howdy213
 * @date 2026-09-25
 * @version 2.1.0
 *
 * Copyright 2025-2026 howdy213
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
 */
#ifndef WCONFIGLAYOUT_H
#define WCONFIGLAYOUT_H

namespace we::config {
namespace Layout {
// Centralized spacing constants to keep WConfig UI row spacing / margins consistent.
inline constexpr int ContentsMargin = 4; // padding inside a config item widget
inline constexpr int ItemSpacing = 2;    // spacing between rows inside a config item
inline constexpr int RowSpacing = 3;     // spacing between the title and description rows
inline constexpr int PageSpacing = 4;    // spacing between content pages of WConfigWidget
inline constexpr int DialogMargin = 6;   // main layout margin of dialogs
inline constexpr int PageLeftMargin = 8; // left inset of the content area
inline constexpr int GroupGuideGap = 8;  // gap between the group's hierarchy guide line and its members
} // namespace Layout
} // namespace we::config

#endif // WCONFIGLAYOUT_H