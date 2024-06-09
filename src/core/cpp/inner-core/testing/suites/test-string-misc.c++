// -*- c++ -*-
//==============================================================================
/// @file test-string-misc.c++
/// @brief C++ core - test routines
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "string/misc.h++"

#include <gtest/gtest.h>

namespace core::str
{
    const std::locale POSIX("C");
    const std::locale LATIN1("en_US.iso8859-1");
    const std::locale UTF8("en_US.utf8");

    TEST(StringTest, UpperLowerCase)
    {
        std::string ascii_mixed{"qwertyQWERTY"};
        std::string ascii_upper("QWERTYQWERTY");
        std::string ascii_lower("qwertyqwerty");
        EXPECT_EQ(toupper(ascii_mixed, POSIX), ascii_upper);
        EXPECT_EQ(tolower(ascii_mixed, POSIX), ascii_lower);

        std::string latin1_mixed = "\xe6\xf8\xe5\xc6\xd8\xc5";
        std::string latin1_upper = "\xc6\xd8\xc5\xc6\xd8\xc5";
        std::string latin1_lower = "\xe6\xf8\xe5\xe6\xf8\xe5";
        EXPECT_EQ(toupper(latin1_mixed, LATIN1), latin1_upper);
        EXPECT_EQ(tolower(latin1_mixed, LATIN1), latin1_lower);
        EXPECT_NE(toupper(latin1_mixed, POSIX), latin1_upper);
        EXPECT_NE(tolower(latin1_mixed, POSIX), latin1_lower);

        // std::string utf8_mixed = "\xc3\xa6\xc3\xb8\xc3\xa5\xc3\x86\xc3\x98\xc3\x85";
        // std::string utf8_upper = "\xc3\x86\xc3\x98\xc3\x85\xc3\x86\xc3\x98\xc3\x85";
        // std::string utf8_lower = "\xc3\xa6\xc3\xb8\xc3\xa5\xc3\xa6\xc3\xb8\xc3\xa5";
        // std::locale locale_utf8("nb_NO.UTF-8");
        // EXPECT_EQ(toupper(utf8_mixed, locale_utf8), utf8_upper);
        // EXPECT_EQ(tolower(utf8_mixed, locale_utf8), utf8_lower);
    }

    // TEST(StringTest, WideString)
    // {
    //     std::string latin1_no("abcdefghijklmnopqrstuvwxyz\xe6\xf8\xe5");
    //     std::string utf8_no("abcdefghijklmnopqrstuvwxyz\xc3\xa6\xc3\xb8\xc3\xa5");

    //     std::wstring widened_latin1_no(to_wstring(latin1_no, LATIN1));
    //     std::wstring widened_utf8_no(to_wstring(utf8_no, UTF8));

    //     EXPECT_THROW(to_wstring(latin1_no, UTF8), std::invalid_argument);

    //     EXPECT_EQ(widened_latin1_no.size(), 29);
    //     EXPECT_EQ(widened_utf8_no.size(), 29);
    //     EXPECT_EQ(widened_latin1_no, widened_utf8_no);

    //     std::string narrowed_latin1_no(from_wstring(widened_latin1_no, LATIN1));
    //     std::string narrowed_utf8_no(from_wstring(widened_utf8_no, UTF8));

    //     EXPECT_EQ(narrowed_latin1_no, latin1_no);
    //     EXPECT_EQ(narrowed_utf8_no, utf8_no);
    // }

    // TEST(StringTest, ObfuscateWide)
    // {
    //     std::string input{"My#Top*Secret%Code"};
    //     std::string result = obfuscated(input);
    //     std::wstring wide_result = to_wstring(result, UTF8);

    //     EXPECT_EQ(input.size(), wide_result.size());
    //     EXPECT_EQ(wide_result.front(), wide_result.back());
    // }

    TEST(StringTest, SplitJoin)
    {
        std::vector<std::string> words{
            "Seven", "words", "do", "", "not", "a", "sentence", "make",
        };

        std::size_t nwords = words.size();
        std::string joined_no_empties = join(words, " ");
        EXPECT_EQ(joined_no_empties, "Seven words do not a sentence make");

        std::string joined_with_empties = join(words, ",", true);
        EXPECT_EQ(joined_with_empties, "Seven,words,do,,not,a,sentence,make");

        std::vector<std::string> partial_split = split(joined_with_empties, ",", 4, true);
        std::vector<std::string> partial_expected(words.begin(), words.begin()+4);
        partial_expected.push_back(join(words.begin()+4, words.end(), ",", true));
        EXPECT_EQ(partial_split, partial_expected);
    }

    TEST(StringTest, Unescape)
    {
        EXPECT_EQ(",\a,\f,\n,\r,\t,\v,\x1b,\x33,",
                  unescaped(",\\a,\\f,\\n,\\r,\\t,\\v,\\e,\\x33,"));
    }

    TEST(StringTest, Substitute)
    {
        std::string s{"smalfalfa"};
        substitute("alfa", "alpha", &s);
        EXPECT_EQ(s, "smalphalfa");
    }

    TEST(StringTest, Stem)
    {
        std::string original{"  mixed Case String  "};
        std::string elim_string{" abcdefghijklmnopqrstuvwxyz"};
        std::set<char> elim_set(elim_string.begin(), elim_string.end());
        std::string stripped = strip(original, elim_set, elim_set);
        EXPECT_EQ(stripped, "Case S");
    }

    TEST(StringTest, Hex)
    {
        std::uint8_t w8{0x42};
        EXPECT_EQ(hex(w8), "0x42");

        std::uint16_t w16{0x42};
        EXPECT_EQ(hex(w16), "0x0042");

        std::uint32_t w32{0x42};
        EXPECT_EQ(hex(w32), "0x00000042");

        std::uint64_t w64{0x42};
        EXPECT_EQ(hex(w64), "0x0000000000000042");
    }

}  // namespace core::str
