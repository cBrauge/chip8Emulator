#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <string>
#include <tuple>
#include <vector>

using byte = unsigned char;
using word = unsigned short int;

class chip8 {
public:
  chip8(const std::string &romPath);
  word getNextOpCode() noexcept;
  void executeOpCode(word opcode) noexcept;
  void clearScreen() noexcept;
  void returnFromSubRoutine() noexcept;
  void jumpToAddress(word address) noexcept;
  void callSubroutine(word nnn) noexcept;
  void equal(word x, word nn) noexcept;
  void notEqual(word x, word nn) noexcept;
  void setRegister(word x, word nn) noexcept;
  void add(word x, word nn) noexcept;
  void assign(word x, word y) noexcept;
  void andRegisters(word x, word y) noexcept;
  void xorRegisters(word x, word y) noexcept;
  void addRegisters(word x, word y) noexcept;
  void subRegisters(word x, word y) noexcept;
  void shiftVx(word x) noexcept;
  void notEqualReg(word x, word y) noexcept;
  void setI(word nnn) noexcept;
  void setXRandAndNN(word x, word nn) noexcept;
  void draw(word x, word y, word n) noexcept;
  void setRegisterDelayTimer(word x) noexcept;
  void getKey(word x) noexcept;
  void keyNotPressed(word x) noexcept;
  void keyPressed(word x) noexcept;
  void setDelayTimer(word x) noexcept;
  void setSoundTimer(word x) noexcept;
  void addRegisterToAddressRegister(word x) noexcept;
  void addressRegisterToSprite(word x) noexcept;
  void binaryCodedDecimal(word x) noexcept;
  void dumpRegisters(word x) noexcept;
  void fillRegisters(word x) noexcept;
  void decreaseTimers() noexcept;
  void playBeep() noexcept;
  int getKeyPressed() noexcept;
  std::tuple<word, word, word, word> splitOpCode(word opcode) noexcept;
  // const byte *getScreen() const noexcept { return m_screen; }
  std::array<std::array<sf::Color, 640>, 320> m_screen{{sf::Color::Black}};
  std::array<bool, 16> m_keys{false};

  bool m_draw = false;

private:
  std::array<byte, 16> m_registers{0};
  std::vector<word> m_stack;
  std::array<byte, 0xFFF> m_gameMemory{0};
  word m_addressRegister = 0;
  word m_programCounter = 0x200;
  byte m_delayTimer = 0;
  byte m_soundTimer = 0;
};
