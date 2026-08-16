#pragma once

#include "jsonutil.h"
#include "Renderer.h"
#include "script.h"

#define NEWUI_ELEMENT(Name) void Name(Renderer& renderer, const pugi::xml_node& node, const nlohmann::json& props, nlohmann::json& context)
