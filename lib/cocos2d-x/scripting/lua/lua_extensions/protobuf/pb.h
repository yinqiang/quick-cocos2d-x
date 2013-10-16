/*
 * =====================================================================================
 *
 *      Filename:  pb.c
 *
 *      Description: protoc-gen-lua
 *      Google's Protocol Buffers project, ported to lua.
 *      https://code.google.com/p/protoc-gen-lua/
 *
 *      Copyright (c) 2010 , 林卓毅 (Zhuoyi Lin) netsnail@gmail.com
 *      All rights reserved.
 *
 *      Use, modification and distribution are subject to the "New BSD License"
 *      as listed at <url: http://www.opensource.org/licenses/bsd-license.php >.
 *
 *      Created:  2010年08月02日 18时04分21秒
 *
 *      Company:  NetEase
 *
 * =====================================================================================
 */

#ifndef __LUA_PB_H_
#define __LUA_PB_H_

#include "lauxlib.h"

LUALIB_API int luaopen_pb (lua_State *L);

#endif // __LUA_PB_H_