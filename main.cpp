#include "chip8.h"
#include <SFML/Graphics.hpp>
#include <iostream>
void draw(sf::RenderWindow &window,
          const std::array<std::array<sf::Color, 640>, 320> &screen) {

  sf::Image buffer;
  buffer.create(640, 320);
  // Sprite buffer
  sf::Texture texture;
  texture.loadFromImage(buffer);
  // texture.update(chip.m_screen);
  sf::Sprite sprite(texture);
  for (int i = 0; i < screen.size(); i++) {
    for (int j = 0; j < screen[i].size(); j++) {
      buffer.setPixel(j, i, screen[i][j]);
    }
  }
  texture.loadFromImage(buffer);
  window.draw(sprite);
  window.display();
}

void handleKey(chip8 &chip, sf::Keyboard::Key key, bool pressed) {
  if (key == sf::Keyboard::Num0) {
    chip.m_keys[0x0] = pressed;
  } else if (key == sf::Keyboard::Num1) {
    chip.m_keys[0x1] = pressed;
  } else if (key == sf::Keyboard::Up) {
    chip.m_keys[0x2] = pressed;
  } else if (key == sf::Keyboard::Num3) {
    chip.m_keys[0x3] = pressed;
  } else if (key == sf::Keyboard::Left) {
    chip.m_keys[0x4] = pressed;
  } else if (key == sf::Keyboard::Space) {
    chip.m_keys[0x5] = pressed;
  } else if (key == sf::Keyboard::Right) {
    chip.m_keys[0x6] = pressed;
  } else if (key == sf::Keyboard::Num7) {
    chip.m_keys[0x7] = pressed;
  } else if (key == sf::Keyboard::Down) {
    chip.m_keys[0x8] = pressed;
  } else if (key == sf::Keyboard::Num9) {
    chip.m_keys[0x9] = pressed;
  } else if (key == sf::Keyboard::A) {
    chip.m_keys[0xA] = pressed;
  } else if (key == sf::Keyboard::B) {
    chip.m_keys[0xB] = pressed;
  } else if (key == sf::Keyboard::C) {
    chip.m_keys[0xC] = pressed;
  } else if (key == sf::Keyboard::D) {
    chip.m_keys[0xD] = pressed;
  } else if (key == sf::Keyboard::E) {
    chip.m_keys[0xE] = pressed;
  } else if (key == sf::Keyboard::F) {
    chip.m_keys[0xF] = pressed;
  }
}

int main() {
  chip8 chip("C:/TETRIS");
  sf::RenderWindow window(sf::VideoMode(640, 320), "Chip8 emulator");
  sf::Event event;
  int fps = 60;
  int numopcodes = 20000;

  sf::Time interval(sf::milliseconds(1000 / fps));
  // number of opcodes to execute a frame
  int numframe = numopcodes / fps;
  sf::Clock clock;
  sf::Time time2 = clock.getElapsedTime();
  while (window.isOpen()) {
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::KeyPressed) {
        handleKey(chip, event.key.code, 1);
      } else if (event.type == sf::Event::KeyReleased) {
        handleKey(chip, event.key.code, 0);
      }
    }
    sf::Time currentTime = clock.getElapsedTime();

    if ((time2 + interval) < currentTime) {
      chip.decreaseTimers();
      for (int i = 0; i < numframe; i++) {
        chip.executeOpCode(chip.getNextOpCode());
      }

      time2 = currentTime;
      if (chip.m_draw) {
        window.clear();
        draw(window, chip.m_screen);
        chip.m_draw = false;
      }
    }
  }
}
