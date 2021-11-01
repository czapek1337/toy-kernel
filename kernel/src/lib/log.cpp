#include "log.h"
#include "console.h"

static void print_integer(uint64_t value, const FormatOptions &options) {
    char buffer[72] = {0};

    auto alphabet = options.get_repr() == FMT_REPR_HEX_UPPER ? "0123456789ABCDEF" : "0123456789abcdef";
    auto ptr = buffer + sizeof(buffer) - 1;
    auto write = [&](char ch) { *ptr-- = ch; };
    auto base = 10;

    write('\x00');

    switch (options.get_repr()) {
    case FMT_REPR_BINARY: base = 2; break;
    case FMT_REPR_OCTAL: base = 8; break;
    case FMT_REPR_HEX:
    case FMT_REPR_HEX_UPPER: base = 16; break;
    default: break;
    }

    if (options.get_alternate() && base != 10) {
        switch (base) {
        case 2: detail::format_arg("0b", {}); break;
        case 8: detail::format_arg("0o", {}); break;
        case 16: detail::format_arg("0x", {}); break;
        default: break;
        }
    }

    if (value == 0)
        write('0');

    while (value > 0) {
        write(alphabet[value % base]);
        value /= base;
    }

    if (options.get_zero_pad() && options.get_width() > 0) {
        auto length = (int) __builtin_strlen(ptr + 1);
        auto pad_count = options.get_width() - length;

        while (pad_count-- > 0) {
            detail::format_arg('0', {});
        }
    }

    detail::format_arg(ptr + 1, {});
}

static void print_integer(int64_t value, const FormatOptions &options) {
    if (options.get_sign() == FMT_SIGN_PLUS) {
        detail::format_arg(value < 0 ? '-' : '+', options);
    } else if (options.get_sign() == FMT_SIGN_SPACE) {
        detail::format_arg(value < 0 ? '-' : ' ', options);
    } else if (options.get_sign() == FMT_SIGN_MINUS && value < 0) {
        detail::format_arg('-', options);
    }

    if (value < 0) {
        print_integer((uint64_t) -value, options);
    } else {
        print_integer((uint64_t) value, options);
    }
}

const char *FormatOptions::parse_align(const char *options) {
    set_fill_char(' ');
    set_alignment(FMT_ALIGN_NONE);

    if (*options == '}')
        return options;

    auto current = *options;
    auto next = *(options + 1);

    switch (next) {
    case '<': set_alignment(FMT_ALIGN_LEFT); break;
    case '>': set_alignment(FMT_ALIGN_RIGHT); break;
    case '=': set_alignment(FMT_ALIGN_CENTER); break;
    }

    if (get_alignment() != FMT_ALIGN_NONE) {
        set_fill_char(current);
        return options + 2;
    } else {
        switch (current) {
        case '<': set_alignment(FMT_ALIGN_LEFT); break;
        case '>': set_alignment(FMT_ALIGN_RIGHT); break;
        case '=': set_alignment(FMT_ALIGN_CENTER); break;
        }

        if (get_alignment() != FMT_ALIGN_NONE) {
            set_fill_char(' ');
            return options + 1;
        }
    }

    return options;
}

const char *FormatOptions::parse_sign(const char *options) {
    set_sign(FMT_SIGN_NONE);

    if (*options == '}')
        return options;

    switch (*options) {
    case '+': set_sign(FMT_SIGN_PLUS); break;
    case '-': set_sign(FMT_SIGN_MINUS); break;
    case ' ': set_sign(FMT_SIGN_SPACE); break;
    }

    if (get_sign() != FMT_SIGN_NONE)
        options++;

    return options;
}

const char *FormatOptions::parse_width(const char *options) {
    set_width(0);

    while (*options != '}' && *options >= '0' && *options <= '9') {
        set_width(get_width() * 10 + *options++ - '0');
    }

    return options;
}

const char *FormatOptions::parse_repr(const char *options) {
    set_repr(FMT_REPR_NONE);

    if (*options == '}')
        return options;

    switch (*options) {
    case 'b': set_repr(FMT_REPR_BINARY); break;
    case 'o': set_repr(FMT_REPR_OCTAL); break;
    case 'd': set_repr(FMT_REPR_DECIMAL); break;
    case 'x': set_repr(FMT_REPR_HEX); break;
    case 'X': set_repr(FMT_REPR_HEX_UPPER); break;
    }

    if (get_repr() != FMT_REPR_NONE)
        options++;

    return options;
}

const char *FormatOptions::parse(const char *options) {
    set_alternate(false);
    set_zero_pad(false);

    options = parse_align(options);
    options = parse_sign(options);

    if (*options == '#') {
        set_alternate(true);
        options++;
    }

    if (*options == '0') {
        set_zero_pad(true);
        options++;
    }

    options = parse_width(options);
    options = parse_repr(options);

    if (*options != '}')
        log_fatal("Invalid format string");

    return options + 1;
}

template <>
void Formatter<LogLevel>::format(const LogLevel &value, const FormatOptions &options) {
    switch (value) {
    case LOG_LEVEL_DEBUG: detail::format_arg("debug", options); break;
    case LOG_LEVEL_INFO: detail::format_arg("info", options); break;
    case LOG_LEVEL_WARN: detail::format_arg("warn", options); break;
    case LOG_LEVEL_ERROR: detail::format_arg("error", options); break;
    case LOG_LEVEL_FATAL: detail::format_arg("fatal", options); break;
    }
}

template <>
void Formatter<char>::format(const char &value, [[maybe_unused]] const FormatOptions &options) {
    arch::io_outb(0x3f8, value);

    console::write(value);
}

template <>
void Formatter<bool>::format(const bool &value, const FormatOptions &options) {
    detail::format_arg(value ? "true" : "false", options);
}

void Formatter<const char *>::format(const char *value, const FormatOptions &options) {
    if (options.get_width() != 0 && !options.get_zero_pad()) {
        auto length = (int) __builtin_strlen(value);
        auto diff = options.get_width() - length;

        if (options.get_width() < length) {
            for (auto i = 0; i < options.get_width(); i++) {
                detail::format_arg(value[i], {});
            }
        } else if (diff > 0) {
            if (options.get_alignment() == FMT_ALIGN_LEFT || options.get_alignment() == FMT_ALIGN_NONE) {
                format(value, {});

                for (auto i = 0; i < diff; i++) {
                    detail::format_arg((char) options.get_fill_char(), {});
                }
            } else if (options.get_alignment() == FMT_ALIGN_CENTER) {
                auto left_diff = diff / 2;
                auto right_diff = diff - left_diff;

                for (auto i = 0; i < left_diff; i++) {
                    detail::format_arg((char) options.get_fill_char(), {});
                }

                format(value, {});

                for (auto i = 0; i < right_diff; i++) {
                    detail::format_arg((char) options.get_fill_char(), {});
                }
            } else if (options.get_alignment() == FMT_ALIGN_RIGHT) {
                for (auto i = 0; i < diff; i++) {
                    detail::format_arg((char) options.get_fill_char(), {});
                }

                format(value, {});
            }
        } else {
            format(value, {});
        }
    } else {
        while (*value) {
            detail::format_arg(*value++, {});
        }
    }
}

void Formatter<char *>::format(char *value, const FormatOptions &options) {
    detail::format_arg((const char *) value, options);
}

template <>
void Formatter<core::String>::format(const core::String &value, const FormatOptions &options) {
    detail::format_arg(value.data(), options);
}

#define MAKE_INT_FORMATTER(type, base_type)                                                                                                \
    template <>                                                                                                                            \
    void Formatter<type>::format(const type &value, const FormatOptions &options) {                                                        \
        print_integer((base_type) value, options);                                                                                         \
    }

MAKE_INT_FORMATTER(uint8_t, uint64_t)
MAKE_INT_FORMATTER(uint16_t, uint64_t)
MAKE_INT_FORMATTER(uint32_t, uint64_t)
MAKE_INT_FORMATTER(uint64_t, uint64_t)

MAKE_INT_FORMATTER(int8_t, int64_t)
MAKE_INT_FORMATTER(int16_t, int64_t)
MAKE_INT_FORMATTER(int32_t, int64_t)
MAKE_INT_FORMATTER(int64_t, int64_t)

#undef MAKE_INT_FORMATTER
