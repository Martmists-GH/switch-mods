#pragma once

void romfs_hooks();
void lua_hooks();

static void common_hooks() {
    romfs_hooks();
    lua_hooks();
}
