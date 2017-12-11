#include "chip8.h"
#include <SFML\Graphics.hpp>
#include <fstream>
#include <iostream>
#include <random>

chip8::chip8(const std::string &romPath) {
  FILE *in = fopen(romPath.c_str(), "rb");
  fread(&m_gameMemory[m_programCounter], 0xfff, 1, in);
  fclose(in);
}

word chip8::getNextOpCode() noexcept {
  word res = m_gameMemory[m_programCounter] << 8;
  res |= m_gameMemory[m_programCounter + 1];
  m_programCounter += 2;
  return res;
}

std::tuple<word, word, word, word> chip8::splitOpCode(word opcode) noexcept {
  word first = (opcode & 0xF000) >> 12;
  word second = (opcode & 0x0F00) >> 8;
  word third = (opcode & 0x00F0) >> 4;
  word fourth = opcode & 0x000F;
  return {first, second, third, fourth};
}

void chip8::executeOpCode(word opcode) noexcept {
  auto [first, second, third, fourth] = splitOpCode(opcode);
  switch (opcode & 0xF000) {
  case 0x000:
    switch (opcode & 0x00FF) {
    case 0xE0:
      clearScreen();
      break;
    case 0xEE:
      returnFromSubRoutine();
      break;
    default:
      // TODO
      std::cerr << "Not implemented yet: " << opcode << '\n';
      break;
    }
    break;
  case 0x1000:
    jumpToAddress(opcode & 0x0FFF);
    break;
  case 0x2000:
    callSubroutine(opcode & 0x0FFF);
    break;
  case 0x3000:
    equal(second, opcode & 0x00FF);
    break;
  case 0x4000:
    notEqual(second, opcode & 0x00FF);
    break;
  case 0x6000:
    setRegister(second, opcode & 0x00FF);
    break;
  case 0x7000:
    add(second, opcode & 0x00FF);
    break;
  case 0x8000:
    switch (fourth) {
    case 0x0:
      assign(second, third);
      break;
    case 0x2:
      andRegisters(second, third);
      break;
    case 0x3:
      xorRegisters(second, third);
      break;
    case 0x4:
      addRegisters(second, third);
      break;
    case 0x5:
      subRegisters(second, third);
      break;
    case 0x6:
      shiftVx(second);
      break;
    default:
      // TODO
      std::cerr << "Not implemented yet: " << opcode << '\n';
      break;
    }
    break;
  case 0x9000:
    notEqualReg(second, third);
    break;
  case 0xA000:
    setI(opcode & 0x0FFF);
    break;
  case 0xC000:
    setXRandAndNN(second, opcode & 0x00FF);
    break;
  case 0xD000:
    draw(second, third, fourth);
    break;
  case 0xE000:
    switch (opcode & 0x00FF) {
    case 0xA1:
      keyNotPressed(second);
      break;
    case 0x9E:
      keyPressed(second);
      break;
    default:
      // TODO
      std::cerr << "Not implemented yet: " << opcode << '\n';
      break;
    }
    break;
  case 0xF000:
    switch (opcode & 0x00FF) {
    case 0x07:
      setRegisterDelayTimer(second);
      break;
    case 0x0A:
      getKey(second);
      break;
    case 0x15:
      setDelayTimer(second);
      break;
    case 0x18:
      setSoundTimer(second);
      break;
    case 0x1E:
      addRegisterToAddressRegister(second);
      break;
    case 0x29:
      addressRegisterToSprite(second);
      break;
    case 0x33:
      binaryCodedDecimal(second);
      break;
    case 0x55:
      dumpRegisters(second);
      break;
    case 0x65:
      fillRegisters(second);
      break;
    default:
      // TODO
      std::cerr << "Not implemented yet: " << opcode << '\n';
      break;
    }
    break;
  default:
    std::cerr << "Not implemented yet: " << opcode << '\n';
    exit(1);
  }
}

void log(const std::string &str) { /*std::cout << str << '\n';*/
}

// Clears screen 00E0
void chip8::clearScreen() noexcept {
  log("Clear screen");
  for (int x = 0; x < m_screen.size(); x++) {
    for (int y = 0; y < m_screen[x].size(); y++) {
      m_screen[x][y] = sf::Color::White;
    }
  }
}

// 00EE
void chip8::returnFromSubRoutine() noexcept {
  m_programCounter = m_stack.back();
  m_stack.pop_back();
}

// Jump to address 1NNN
void chip8::jumpToAddress(word address) noexcept {
  log("Jump to address: " + std::to_string(address));
  m_programCounter = address;
}

// Calls subroutine at 2NNN
void chip8::callSubroutine(word nnn) noexcept {
  log("Call subroutine at: " + std::to_string(nnn));
  m_stack.push_back(m_programCounter);
  m_programCounter = nnn;
}

// 3XNN skips next instruction if vX == NN
void chip8::equal(word x, word nn) noexcept {
  log("if equal reg " + std::to_string(x) + ": " +
      std::to_string(m_registers[x]) + " == " + std::to_string(nn));
  if (m_registers[x] == nn) {
    m_programCounter += 2;
  }
}

// 4XNN skips next instruction if vX != NN
void chip8::notEqual(word x, word nn) noexcept {
  log("if not equal reg " + std::to_string(x) + ": " +
      std::to_string(m_registers[x]) + " != " + std::to_string(nn));
  if (m_registers[x] != nn) {
    m_programCounter += 2;
  }
}

// 6XNN Vx = NN
void chip8::setRegister(word x, word nn) noexcept {
  log("Set reg " + std::to_string(x) + ": " + std::to_string(m_registers[x]) +
      " = " + std::to_string(nn));
  m_registers[x] = nn;
}

// 7XNN Vx += NN
void chip8::add(word x, word nn) noexcept {
  log("Add " + std::to_string(x) + ": " + std::to_string(m_registers[x]) +
      " += " + std::to_string(nn));
  m_registers[x] += nn;
}

// 8XY0 Vx = Vy
void chip8::assign(word x, word y) noexcept { m_registers[x] = m_registers[y]; }

// 8XY2 Vx &= Vy
void chip8::andRegisters(word x, word y) noexcept {
  m_registers[x] &= m_registers[y];
}

// 8XY3 Vx ^= Vy
void chip8::xorRegisters(word x, word y) noexcept {
  m_registers[x] ^= m_registers[y];
}

// 8XY4 Vx += Vy VF is set to 1 when there's a carry, and to 0 when there isn't.
void chip8::addRegisters(word x, word y) noexcept {
  m_registers[0xF] = 0;
  int value = m_registers[x] + m_registers[y];

  if (value > 255) {
    m_registers[0xF] = 1;
  }
  m_registers[x] += m_registers[y];
}

// 8XY5 Vx -= Vy VF is set to 0 when there's a borrow, and to 1 when there
// isn't.
void chip8::subRegisters(word x, word y) noexcept {
  m_registers[0xF] = 1;
  int value = m_registers[x] - m_registers[y];

  if (m_registers[x] < m_registers[y]) {
    m_registers[0xF] = 0;
  }
  m_registers[x] -= m_registers[y];
}

// 8XY6 Shifts VX right by one. VF is set to the value of the least significant
// bit of VX before the shift.
void chip8::shiftVx(word x) noexcept {
  m_registers[0xF] = m_registers[x] & 0x1;
  m_registers[x] >>= 1;
}

// 9XY0 Vx != Vy
void chip8::notEqualReg(word x, word y) noexcept {
  if (m_registers[x] != m_registers[y]) {
    m_programCounter += 2;
  }
}

// ANNN set I to NNN
void chip8::setI(word nnn) noexcept {
  log("Set I: " + std::to_string(nnn));
  m_addressRegister = nnn;
}

// CXNN Vx = rand() & NN
void chip8::setXRandAndNN(word x, word nn) noexcept {
  log("Rand " + std::to_string(x) + ": " + std::to_string(m_registers[x]) +
      " & " + std::to_string(nn));
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<int> uni(0, 0xFF);

  auto random_integer = uni(rng);
  m_registers[x] = random_integer & nn;
}

// DXYN draw at Vx Vy, width of 8, height of n
void chip8::draw(word xReg, word yReg, word n) noexcept {
  log("Draw xReg:" + std::to_string(xReg) + " yReg: " + std::to_string(yReg) +
      " n: " + std::to_string(n));
  const int SCALE = 10;
  word coordx = m_registers[xReg] * SCALE;
  word coordy = m_registers[yReg] * SCALE;

  m_registers[0xF] = 0;

  for (int yline = 0; yline < n; yline++) {
    // this is the data of the sprite stored at m_GameMemory[m_AddressI]
    // the data is stored as a line of bytes so each line is indexed by
    // m_AddressI + yline
    byte data = (m_gameMemory[m_addressRegister + yline]);

    // for each of the 8 pixels in the line
    int xpixel = 0;
    int xpixelinv = 7;
    for (xpixel = 0; xpixel < 8; xpixel++, xpixelinv--) {

      // is ths pixel set to 1? If so then the code needs to toggle its state
      int mask = 1 << xpixelinv;
      if (data & mask) {

        int x = (xpixel * SCALE) + coordx;
        int y = coordy + (yline * SCALE);

        sf::Color colour = sf::Color::Black;
        // a collision has been detected
        if (m_screen[y][x] == sf::Color::Black) {
          colour = sf::Color::White;
          m_registers[0xF] = 1;
        }
        for (int i = 0; i < SCALE; i++) {
          for (int j = 0; j < SCALE; j++) {
            m_screen[y + i][x + j] = colour;
          }
        }
      }
    }
  }
  m_draw = true;
}

// EXA1 Skip next instruction if key in Vx is not pressed
void chip8::keyNotPressed(word x) noexcept {
  if (!m_keys[m_registers[x]]) {
    m_programCounter += 2;
  }
}

// EX9E Skip next instruction if key in Vx is pressed
void chip8::keyPressed(word x) noexcept {
  if (m_keys[m_registers[x]]) {
    m_programCounter += 2;
  }
}

// FX07
void chip8::setRegisterDelayTimer(word x) noexcept {
  m_registers[x] = m_delayTimer;
}

// FX04 getKey
void chip8::getKey(word x) noexcept {
  int keypressed = getKeyPressed();

  if (keypressed == -1) {
    m_programCounter -= 2;
  } else {
    m_registers[x] = keypressed;
  }
}

// FX15 Set delay timer
void chip8::setDelayTimer(word x) noexcept { m_delayTimer = m_registers[x]; }

// FX18 Set sound timer
void chip8::setSoundTimer(word x) noexcept { m_soundTimer = m_registers[x]; }

// FX1E I += Vx
void chip8::addRegisterToAddressRegister(word x) noexcept {
  log("AddReg to I x:" + std::to_string(x));
  m_addressRegister += m_registers[x];
}

// FX29 Sets I to the location of the sprite for the character in VX
void chip8::addressRegisterToSprite(word x) noexcept {
  m_addressRegister = m_registers[x] * 5; // *5?
}

// FX33 Stores binary coded decimal value of Vx
void chip8::binaryCodedDecimal(word x) noexcept {

  int value = m_registers[x];

  m_gameMemory[m_addressRegister] = value / 100;
  m_gameMemory[m_addressRegister + 1] = (value / 10) % 10;
  m_gameMemory[m_addressRegister + 2] = value % 10;
}

// FX55 Dumps V0 to Vx with memory starting at I
void chip8::dumpRegisters(word x) noexcept {
  for (word i = 0; i <= x; i++) {
    m_gameMemory[m_addressRegister++] = m_registers[i];
  }
  // +1 at the end?
}

// FX65 Fills V0 to Vx with memory starting at I
void chip8::fillRegisters(word x) noexcept {
  for (word i = 0; i <= x; i++) {
    m_registers[i] = m_gameMemory[m_addressRegister++];
  }
  // +1 at the end?
}

void chip8::decreaseTimers() noexcept {
  if (m_delayTimer > 0) {
    m_delayTimer--;
  }

  if (m_soundTimer > 0) {
    m_soundTimer--;
  }
  if (m_soundTimer > 0) {
    playBeep();
  }
}

void chip8::playBeep() noexcept { std::cout << "BEEP!!\n"; }

int chip8::getKeyPressed() noexcept {
  int res = -1;

  for (int i = 0; i < 16; i++) {
    if (m_keys[i] > 0)
      return i;
  }

  return res;
}
