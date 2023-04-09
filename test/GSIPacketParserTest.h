#ifndef GSIPACKETPARSERTEST_H
#define GSIPACKETPARSERTEST_H

void assertVec3(const GameState::Vec3& lhs, const GameState::Vec3& rhs);

void assertBombInfo(const GameState::BombInfo& lhs, const GameState::BombInfo& rhs);

void assertSideStats(const GameState::MapInfo::SideStats& lhs, const GameState::MapInfo::SideStats& rhs);

void assertMapInfo(const GameState::MapInfo& lhs, const GameState::MapInfo& rhs);

void assertPlayerRoundState(const GameState::Player::RoundState& lhs, const GameState::Player::RoundState& rhs);

void assertMatchStats(const GameState::Player::MatchStats& lhs, const GameState::Player::MatchStats& rhs);

void assertWeapon(const GameState::Weapon& lhs, const GameState::Weapon& rhs);

void assertPlayer(const GameState::Player& lhs, const GameState::Player& rhs);

void assertProvider(const GameState::Provider& lhs, const GameState::Provider& rhs);

void assertGameState(const GameState& lhs, const GameState& rhs);

#endif //GSIPACKETPARSERTEST_H
