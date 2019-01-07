#pragma once

#ifndef GAURD_gstc_H
#define GAURD_gstc_H

namespace gstc {
    const int len = 70;
	const int len_s = 10;
    const double conv = 256.0 / len;
	const double conv_s = 256.0 / len_s;
    const char chars[len] = {' ', '.', '\'', '`', '^', '"', ',', ':', ';', 'I',
        'l', '!', 'i', '>', '<', '~', '+', '_', '-', '?',
        ']', '[', '}', '{', '1', ')', '(', '|', '\\', '/',
        't', 'f', 'j', 'r', 'x', 'n', 'u', 'v', 'c', 'z',
        'X', 'Y', 'U', 'J', 'C', 'L', 'Q', '0', 'O', 'Z',
        'm', 'w', 'q', 'p', 'd', 'b', 'k', 'h', 'a', 'o',
        '*', '#', 'M', 'W', '&', '8', '%', 'B', '@', '$'};
	const char chars_s[len_s] = { ' ', '.', ':', '-', '=', '+', '*', '#', '%', '@'};

	char convert(int intensity);
	char convert_s(int intensity);
}

#endif
