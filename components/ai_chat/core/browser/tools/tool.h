// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef BRAVE_COMPONENTS_AI_CHAT_CORE_BROWSER_TOOLS_TOOL_H_
#define BRAVE_COMPONENTS_AI_CHAT_CORE_BROWSER_TOOLS_TOOL_H_

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "base/functional/callback_forward.h"
#include "base/values.h"
#include "brave/components/ai_chat/core/common/mojom/ai_chat.mojom-forward.h"

namespace ai_chat {

// Base class for Tools that are exposed to the Assistant
class Tool {
 public:
  using ToolResult = std::optional<std::vector<mojom::ContentBlockPtr>>;
  using UseToolCallback = base::OnceCallback<void(ToolResult&& output)>;

  Tool() = default;
  virtual ~Tool() = default;

  Tool(const Tool&) = delete;
  Tool& operator=(const Tool&) = delete;

  // Should be a unique name for the tool
  virtual std::string_view Name() const = 0;

  // Description for the Assistant to understand the purpose of the tool
  virtual std::string_view Description() const = 0;

  // Type of the tool, usually left as default "function"
  virtual std::string_view Type() const;

  // If the tool accepts parameters, they should be defined in JSON Schema
  // format, e.g. `{ "location": { "type": "string", "description": "location
  // for weather" } }`
  // TODO(petemill): Use base::Value::Dict to avoid JSON parsing in api
  // clients. Provide static helpers to build the schema, e.g.
  // StringProperty(description, optional_values), ArrayProperty(description,
  // items), ObjectProperty(description, properties).
  virtual std::optional<std::string> InputProperties() const;

  // A list of properties contained within GetInputSchemaJson that are required
  virtual std::optional<std::vector<std::string>> RequiredProperties() const;

  // Parameters for remote-defined tools that this client provides, e.g. screen
  // width, location, etc.
  virtual std::optional<base::Value::Dict> ExtraParams() const;

  // If this tool requires content associated, it won't be provided if
  // used in a conversation without content association.
  virtual bool IsContentAssociationRequired() const;

  virtual bool IsAgentTool() const;

  virtual bool IsSupportedByModel(const mojom::Model& model) const;

  // If this tool requires a user to interact with it before a response will
  // be sent to the Assistant.
  virtual bool RequiresUserInteractionBeforeHandling() const;

  // Implementers should handle tool execution unless it is a built-in
  // tool handled directly by the ConversationHandler.
  virtual void UseTool(const std::string& input_json, UseToolCallback callback);
};

}  // namespace ai_chat

#endif  // BRAVE_COMPONENTS_AI_CHAT_CORE_BROWSER_TOOLS_TOOL_H_
