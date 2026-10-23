#pragma once

#include <string>

enum class CardType
{
    DEBIT,
    CREDIT
};
enum class CardStatus
{
    ACTIVE,
    BLOCKED
};

struct Card
{
    std::string _id{};
    std::string card_number{};
    CardType card_type{CardType::DEBIT};
    std::string cvv{};
    CardStatus status{CardStatus::ACTIVE};
    std::string expiry_date{};
    std::string linked_account{};
};
