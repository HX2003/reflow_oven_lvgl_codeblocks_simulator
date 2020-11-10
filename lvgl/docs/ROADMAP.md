# Roadmap

This is a summary for thenew fatures of the major releases and a collection of ideas. 

This list indicates only the current intention and can be changed.

## v8
Planned to September/October 2020
- Create an `lv_components` repository for compley widgets
   - It makes the core LVGL leaner
   - In `lv_components` we can have a lot and specific widgets
   - Good place for contribution
- New scrolling:
  - See [feat/new-scroll](https://github.com/lvgl/lvgl/tree/feat/new-scroll) branch and  [#1614](https://github.com/lvgl/lvgl/issues/1614)) issue.
  - Remove `lv_page` and support scrolling on `lv_obj`
  - Support "elastic" scrolling when scrolled in
  - Support scroll chaining among any objects types (not only `lv_pages`s)
  - Remove `lv_drag`. Similar effect can be achieved by setting the position in `LV_EVENT_PRESSING`
  - Add snapping
  - Add snap stop to scroll max 1 snap point
  - Already working
- New layouts:
  - See [#1615](https://github.com/lvgl/lvgl/issues/1615) issue
  - [CSS Grid](https://css-tricks.com/snippets/css/a-guide-to-grid/)-like layout support
  - [CSS Flexbox](https://css-tricks.com/snippets/css/a-guide-to-flexbox/)-like layout support
  - Remove `lv_cont` and support layouts on `lv_obj`
- Simplified File system interface ([feat/new_fs_api](https://github.com/lvgl/lvgl/tree/feat/new-fs-api) branch) to make porting easier
  - Work in progress
- Remove the align parameter from `lv_canvas_draw_text`
- Make the `copy` parameter of `create` functions deprecated

## v8.1
- Add radio button widget

## v9
- Simplify `group`s. Discussion is [here](https://forum.lvgl.io/t/lv-group-tabindex/2927/3).
- Unit testing (gtest?). See [#1658](https://github.com/lvgl/lvgl/issues/1658)
- Benchmarking (gem5?). See [#1660](https://github.com/lvgl/lvgl/issues/1660)
- Consider direct binary font format support
- Remove the copy paramter from create functions
- Style selectors and style-based states See [#1832](https://github.com/lvgl/lvgl/issues/1832)

## v10
- Remove property level states


## Ideas
- 9-patch support for `lv_imgbtn`.
- lv_mem_alloc_aligned(size, align)
- Text node. See [#1701](https://github.com/lvgl/lvgl/issues/1701#issuecomment-699479408)
- RGB888 support [#1722](https://github.com/lvgl/lvgl/issues/1722)
- Consider ARGB4444, ARGB1555 format
- CPP binding. See [Forum](https://forum.lvgl.io/t/is-it-possible-to-officially-support-optional-cpp-api/2736)
- Optmize font decompression
- Switch to RGBA colors in styles
- Need coverage report for tests
- Need static analize (via coverity.io or somehing else)
- Support dot_begin and dot_middle long modes for labels
- Add new label alignment modes. [#1656](https://github.com/lvgl/lvgl/issues/1656)
