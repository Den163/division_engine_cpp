#!/bin/bash

CURR_DIR=$(pwd)
PLIST_PATH="${CURR_DIR}/build/examples/resources/debug.plist"

codesign -s - -v -f --entitlements "${PLIST_PATH}" "${CURR_DIR}/build/examples/division_view_tree_example"
codesign -s - -v -f --entitlements "${PLIST_PATH}" "${CURR_DIR}/build/examples/division_canvas_example"
codesign -s - -v -f --entitlements "${PLIST_PATH}" "${CURR_DIR}/build/examples/division_core_example"