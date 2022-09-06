#include <stddef.h>
#include <stdint.h>
#ifdef __linux__
#error "Please use cross-compiler"
#endif

#ifndef __i386__
#error "Need ix86-elf compiler"
#endif

namespace vga {
enum vga_color {
    BLACK = 0,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHT_GREY,
    DARK_GREY,
    LIGHT_BLUE,
    LIGHT_GREEN,
    LIGHT_CYAN,
    LIGHT_RED,
    LIGHT_MAGENTA,
    LIGHT_BROWN,
    WHITE
};
inline uint8_t entry_color(vga_color fg, vga_color bg) { return fg | bg << 4; }
inline uint16_t entry(unsigned char uc, uint8_t color) {
    return static_cast<uint16_t>(uc) | static_cast<uint16_t>(color) << 8;
}
constexpr size_t VAG_WIDTH = 80;
constexpr size_t VAG_HEIGHT = 24;
}  // namespace vga

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}
size_t strlen(char* str, size_t n) {
    size_t len = 0;
    while (str[len] && len < n) len++;
    return len;
}

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize() {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color =
        vga::entry_color(vga::vga_color::LIGHT_GREY, vga::vga_color::BLACK);
    terminal_buffer = reinterpret_cast<uint16_t*>(0xB8000);
    for (size_t y = 0; y < vga::VAG_HEIGHT; y++) {
        for (size_t x = 0; x < vga::VAG_WIDTH; x++) {
            const size_t index = y * vga::VAG_WIDTH + x;
            terminal_buffer[index] = vga::entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color) { terminal_color = color; }

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * vga::VAG_WIDTH + x;
    terminal_buffer[index] = vga::entry(c, color);
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_row++;
        terminal_column = 0;
    } else {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        terminal_column++;
        if (terminal_column == vga::VAG_WIDTH) {
            terminal_column = 0;
            if (terminal_row < vga::VAG_HEIGHT)
                terminal_row++;
            else {
                for (size_t i = 0; i < vga::VAG_HEIGHT - 1; i++) {
                    for (size_t j = 0; j < vga::VAG_WIDTH; j++) {
                        terminal_putentryat(
                            terminal_buffer[(i + 1) * vga::VAG_WIDTH + j],
                            terminal_color, j, i);
                    }
                }
                for (size_t i = 0; i < vga::VAG_WIDTH; i++) {
                    terminal_putentryat(' ', terminal_color, i, terminal_row);
                }
            }
        }
    }
}

void terminal_write(const char* data, size_t n) {
    for (size_t i = 0; i < n; i++) {
        terminal_putchar(data[i]);
    }
}

void terminal_write(const char* data) { terminal_write(data, strlen(data)); }

void terminal_write(char* data, size_t n) {
    size_t size = strlen(data, n);
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}

extern "C" void kernel_main() {
    constexpr size_t MAXLEN = 100;
    char buf[MAXLEN];
    terminal_initialize();
    terminal_setcolor(vga::entry_color(vga::LIGHT_BLUE, vga::BLACK));
    terminal_write("Jllobvemy\n");
    for (size_t i = 0; i < vga::VAG_WIDTH * vga::VAG_HEIGHT; i++) {
        char c = i % 16;
        terminal_setcolor(vga::entry_color(static_cast<vga::vga_color>(c), vga::BLACK));
        if (c >= 10) {
            buf[0] = c - 10 + 'A';
        } else {
            buf[0] = c + '0';
        }
        terminal_write(buf, MAXLEN);
        if (i >= SIZE_MAX - 1) i = 0;
    }
    terminal_setcolor(vga::entry_color(vga::WHITE, vga::BLACK));
    terminal_write("Hello, kernel World!\n");
}
