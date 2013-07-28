#!/bin/bash
#
# Invoked build.xml, overriding the lolua++ property
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
TOLUA="$QUICK_COCOS2DX_ROOT/bin/mac/tolua++"

cd "$SCRIPT_DIR/"
${TOLUA} -L "$SCRIPT_DIR/basic.lua" -o "$SCRIPT_DIR/../cocos2dx/script_support/LuaCocos2d.cpp" Cocos2d.tolua
