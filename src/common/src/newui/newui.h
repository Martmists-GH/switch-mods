#pragma once

#include "jsonutil.h"
#include "Renderer.h"
#include "script.h"

#define NEWUI_ELEMENT(Name) void Name(Renderer& renderer, newui::CachedRenderNode& node, nlohmann::json& context)
