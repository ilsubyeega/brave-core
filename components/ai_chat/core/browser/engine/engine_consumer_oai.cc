// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/components/ai_chat/core/browser/engine/engine_consumer_oai.h"

#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/functional/callback_helpers.h"
#include "base/i18n/time_formatting.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/strings/strcat.h"
#include "base/strings/string_tokenizer.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/time/time.h"
#include "base/types/expected.h"
#include "base/values.h"
#include "brave/components/ai_chat/core/browser/engine/engine_consumer.h"
#include "brave/components/ai_chat/core/common/mojom/ai_chat.mojom-forward.h"
#include "brave/components/ai_chat/core/common/mojom/ai_chat.mojom-shared.h"
#include "brave/components/ai_chat/core/common/mojom/ai_chat.mojom.h"
#include "components/grit/brave_components_strings.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"
#include "ui/base/l10n/l10n_util.h"

namespace {
constexpr char kQuestionPrompt[] =
    "Propose up to 3 very short questions that a reader may ask about the "
    "content. Wrap each in <question> tags.";
}  // namespace

namespace ai_chat {

namespace {

using mojom::CharacterType;
using mojom::ConversationTurn;

base::Value::List BuildMessages(
    const mojom::CustomModelOptions& model_options,
    const std::string& page_content,
    const std::optional<std::string>& selected_text,
    const bool& is_video,
    const EngineConsumer::ConversationHistory& conversation_history) {
  base::Value::List messages;

  // Append system message
  {
    std::string system_message;
    std::string date_and_time_string =
        base::UTF16ToUTF8(TimeFormatFriendlyDateAndTime(base::Time::Now()));

    if (model_options.model_system_prompt &&
        !model_options.model_system_prompt->empty()) {
      system_message = model_options.model_system_prompt.value();
      // Let the user optionally specify the datetime placeholder
      base::ReplaceSubstringsAfterOffset(&system_message, 0, "%datetime%",
                                         date_and_time_string);
    } else {
      system_message = base::ReplaceStringPlaceholders(
          l10n_util::GetStringUTF8(
              IDS_AI_CHAT_DEFAULT_CUSTOM_MODEL_SYSTEM_PROMPT),
          {date_and_time_string}, nullptr);
    }

    base::Value::Dict message;
    message.Set("role", "system");
    message.Set("content", system_message);
    messages.Append(std::move(message));
  }

  // Append page content, if exists
  if (!page_content.empty()) {
    const std::string prompt_segment_article = base::ReplaceStringPlaceholders(
        l10n_util::GetStringUTF8(
            is_video ? IDS_AI_CHAT_LLAMA2_VIDEO_PROMPT_SEGMENT
                     : IDS_AI_CHAT_LLAMA2_ARTICLE_PROMPT_SEGMENT),
        {page_content}, nullptr);

    base::Value::Dict message;
    message.Set("role", "user");
    message.Set("content", prompt_segment_article);
    messages.Append(std::move(message));
  }

  for (const mojom::ConversationTurnPtr& turn : conversation_history) {
    if (turn->uploaded_files) {
      base::Value::List content_uploaded_images;
      base::Value::List content_screenshots;
      content_uploaded_images.Append(
          base::Value::Dict()
              .Set("type", "text")
              .Set("text", "These images are uploaded by the user"));
      content_screenshots.Append(
          base::Value::Dict()
              .Set("type", "text")
              .Set("text", "These images are screenshots"));
      for (const auto& uploaded_file : turn->uploaded_files.value()) {
        if (uploaded_file->type != mojom::UploadedFileType::kImage &&
            uploaded_file->type != mojom::UploadedFileType::kScreenshot) {
          continue;
        }
        base::Value::Dict image;
        image.Set("type", "image_url");
        base::Value::Dict image_url_dict;
        image_url_dict.Set(
            "url", EngineConsumer::GetImageDataURL(uploaded_file->data));
        image.Set("image_url", std::move(image_url_dict));
        if (uploaded_file->type == mojom::UploadedFileType::kImage) {
          content_uploaded_images.Append(std::move(image));
        } else {
          content_screenshots.Append(std::move(image));
        }
      }
      if (content_uploaded_images.size() > 1) {
        messages.Append(
            base::Value::Dict()
                .Set("role", "user")
                .Set("content", std::move(content_uploaded_images)));
      }
      if (content_screenshots.size() > 1) {
        messages.Append(base::Value::Dict()
                            .Set("role", "user")
                            .Set("content", std::move(content_screenshots)));
      }
    }
    base::Value::Dict message;
    message.Set("role", turn->character_type == CharacterType::HUMAN
                            ? "user"
                            : "assistant");

    message.Set(
        "content",
        turn->selected_text
            ? base::StrCat(
                  {base::ReplaceStringPlaceholders(
                       l10n_util::GetStringUTF8(
                           IDS_AI_CHAT_LLAMA2_SELECTED_TEXT_PROMPT_SEGMENT),
                       {*turn->selected_text}, nullptr),
                   "\n\n", EngineConsumer::GetPromptForEntry(turn)})
            : EngineConsumer::GetPromptForEntry(turn));
    messages.Append(std::move(message));
  }

  return messages;
}

}  // namespace

EngineConsumerOAIRemote::EngineConsumerOAIRemote(
    const mojom::CustomModelOptions& model_options,
    scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory,
    ModelService* model_service)
    : EngineConsumer(model_service) {
  model_options_ = model_options;
  max_associated_content_length_ = model_options.max_associated_content_length;

  // Initialize the API client
  api_ = std::make_unique<OAIAPIClient>(url_loader_factory);
}

EngineConsumerOAIRemote::~EngineConsumerOAIRemote() = default;

void EngineConsumerOAIRemote::ClearAllQueries() {
  api_->ClearAllQueries();
}

bool EngineConsumerOAIRemote::SupportsDeltaTextResponses() const {
  return true;
}

void EngineConsumerOAIRemote::UpdateModelOptions(
    const mojom::ModelOptions& options) {
  if (options.is_custom_model_options()) {
    model_options_ = *options.get_custom_model_options();
    max_associated_content_length_ =
        model_options_.max_associated_content_length;
  }
}

void EngineConsumerOAIRemote::GenerateRewriteSuggestion(
    std::string text,
    const std::string& question,
    const std::string& selected_language,
    GenerationDataCallback received_callback,
    GenerationCompletedCallback completed_callback) {
  const std::string& truncated_text =
      text.substr(0, max_associated_content_length_);
  std::string rewrite_prompt = base::ReplaceStringPlaceholders(
      l10n_util::GetStringUTF8(
          IDS_AI_CHAT_LLAMA2_GENERATE_REWRITE_SUGGESTION_PROMPT),
      {truncated_text, question}, nullptr);

  base::Value::List messages;

  {
    base::Value::Dict message;
    message.Set("role", "user");
    message.Set("content", rewrite_prompt);
    messages.Append(std::move(message));
  }

  api_->PerformRequest(model_options_, std::move(messages),
                       std::move(received_callback),
                       std::move(completed_callback));
}

void EngineConsumerOAIRemote::GenerateQuestionSuggestions(
    const bool& is_video,
    const std::string& page_content,
    const std::string& selected_language,
    SuggestedQuestionsCallback callback) {
  const std::string& truncated_page_content =
      page_content.substr(0, max_associated_content_length_);
  std::string content_segment = base::ReplaceStringPlaceholders(
      l10n_util::GetStringUTF8(is_video
                                   ? IDS_AI_CHAT_CLAUDE_VIDEO_PROMPT_SEGMENT
                                   : IDS_AI_CHAT_CLAUDE_ARTICLE_PROMPT_SEGMENT),
      {truncated_page_content}, nullptr);

  base::Value::List messages;

  {
    base::Value::Dict message;
    message.Set("role", "user");
    message.Set("content", base::StrCat({content_segment, kQuestionPrompt}));
    messages.Append(std::move(message));
  }

  {
    base::Value::Dict message;
    message.Set("role", "assistant");
    message.Set("content",
                "Here are three questions the user may ask about the content "
                "in <question> tags:\n");
    messages.Append(std::move(message));
  }

  api_->PerformRequest(
      model_options_, std::move(messages), base::NullCallback(),
      base::BindOnce(
          &EngineConsumerOAIRemote::OnGenerateQuestionSuggestionsResponse,
          weak_ptr_factory_.GetWeakPtr(), std::move(callback)));
}

void EngineConsumerOAIRemote::OnGenerateQuestionSuggestionsResponse(
    SuggestedQuestionsCallback callback,
    GenerationResult result) {
  if (!result.has_value()) {
    // Query resulted in error
    std::move(callback).Run(base::unexpected(std::move(result.error())));
    return;
  }

  if (!result->event || !result->event->is_completion_event() ||
      result->event->get_completion_event()->completion.empty()) {
    // No questions were generated
    std::move(callback).Run(base::unexpected(mojom::APIError::InternalError));
    return;
  }

  base::StringTokenizer tokenizer(
      result->event->get_completion_event()->completion, "</>");
  tokenizer.set_options(base::StringTokenizer::RETURN_DELIMS);

  std::vector<std::string> questions;

  while (tokenizer.GetNext()) {
    std::string token = tokenizer.token();
    std::string_view trimmed_token =
        base::TrimWhitespaceASCII(token, base::TrimPositions::TRIM_ALL);

    if (*tokenizer.token_begin() == '\n') {
      continue;
    }

    if (!tokenizer.token_is_delim() && trimmed_token != "question") {
      questions.emplace_back(trimmed_token);
    }
  }

  std::move(callback).Run(std::move(questions));
}

void EngineConsumerOAIRemote::GenerateAssistantResponse(
    const bool& is_video,
    const std::string& page_content,
    const ConversationHistory& conversation_history,
    const std::string& selected_language,
    const std::vector<base::WeakPtr<Tool>>& tools,
    std::optional<std::string_view> preferred_tool_name,
    GenerationDataCallback data_received_callback,
    GenerationCompletedCallback completed_callback) {
  if (!CanPerformCompletionRequest(conversation_history)) {
    std::move(completed_callback).Run(base::unexpected(mojom::APIError::None));
    return;
  }

  const auto& last_turn = conversation_history.back();
  std::optional<std::string> selected_text = std::nullopt;
  if (last_turn->selected_text.has_value()) {
    selected_text =
        last_turn->selected_text->substr(0, max_associated_content_length_);
  }

  const std::string& truncated_page_content = page_content.substr(
      0, selected_text ? max_associated_content_length_ - selected_text->size()
                       : max_associated_content_length_);

  base::Value::List messages =
      BuildMessages(model_options_, truncated_page_content, selected_text,
                    is_video, conversation_history);
  api_->PerformRequest(model_options_, std::move(messages),
                       std::move(data_received_callback),
                       std::move(completed_callback));
}

void EngineConsumerOAIRemote::SanitizeInput(std::string& input) {}

void EngineConsumerOAIRemote::GetSuggestedTopics(
    const std::vector<Tab>& tabs,
    GetSuggestedTopicsCallback callback) {
  std::move(callback).Run(base::unexpected(mojom::APIError::InternalError));
}

void EngineConsumerOAIRemote::GetFocusTabs(const std::vector<Tab>& tabs,
                                           const std::string& topic,
                                           GetFocusTabsCallback callback) {
  std::move(callback).Run(base::unexpected(mojom::APIError::InternalError));
}

}  // namespace ai_chat
